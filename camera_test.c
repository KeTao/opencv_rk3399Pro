#include <stdio.h>
#include <stdlib.h>
#include "camera_test.h"
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <sys/mman.h>

int init_v4l2(void)
{
	//open video device
	int ret;

	fd = open(FILE_VIDEO,O_RDWR);
	if(fd == -1) {
		perror("open file");
		return fd;
	}	

	ret = ioctl(fd,VIDIOC_QUERYCAP,&cap);
	if(ret == -1) {
		printf("VIDIOC_QUERYCAP failed\n");
		return ret;
	}

	//print video capabilities
	printf("driver:\t\t%s\n",cap.driver);
	printf("card:\t\t%s\n",cap.card);
	printf("bus_info:\t\t%s\n",cap.bus_info);
	printf("version:\t\t%d\n",cap.version);
	printf("capabilities:\t\t%x\n",cap.capabilities);

	if((cap.capabilities & V4L2_CAP_VIDEO_CAPTURE) == V4L2_CAP_VIDEO_CAPTURE) {
		printf("Device %s : supports capture.\n",FILE_VIDEO);
	}

	if((cap.capabilities & V4L2_CAP_STREAMING) == V4L2_CAP_STREAMING) {
		printf("Device %s:supports streaming.\n",FILE_VIDEO);
	}

	//get vide0 pixel format
	fmtdesc.index = 0;
	fmtdesc.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
	printf("Supports format:\n");

	while(ioctl(fd,VIDIOC_ENUM_FMT,&fmtdesc) != -1) {
		printf("\t%d.%s\n",fmtdesc.index + 1,fmtdesc.description);
		fmtdesc.index++;
	}

	//set video pixel format
	fmt.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
	fmt.fmt.pix.pixelformat = V4L2_PIX_FMT_YUYV;
	fmt.fmt.pix.height = IMAGEHEIGHT;
	fmt.fmt.pix.width = IMAGEWIDTH;
	fmt.fmt.pix.field = V4L2_FIELD_INTERLACED;

	if(ioctl(fd,VIDIOC_S_FMT,&fmt) == -1)
	{
		printf("Unable to set format\n");
		return FALSE;
	}

	//get video pixel format
	if(ioctl(fd,VIDIOC_G_FMT,&fmt) == -1 ) {
		printf("Unable to get format\n");
		return FALSE;
	}

	printf("fmt.type:\t\t%d\n",fmt.type);
	printf("pix.pixelformat:\t%c%c%c%c\n",fmt.fmt.pix.pixelformat & 0xFF,(fmt.fmt.pix.pixelformat >> 8) & 0xFF,
										(fmt.fmt.pix.pixelformat >> 16) & 0xFF,(fmt.fmt.pix.pixelformat >> 24) & 0xFF);
	printf("pix.height:\t\t%d\n",fmt.fmt.pix.height);
	printf("pix.width:\t\t%d\n",fmt.fmt.pix.width);
	printf("pix.field:\t\t%d\n",fmt.fmt.pix.field);

	return 0;
}


int v4l2_grab(FILE* fd_file)
{
	unsigned int n_buffers;

	//1.request kernel buffers(4 frames)
	req.count = 4;
	req.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
	req.memory = V4L2_MEMORY_MMAP;
	if(ioctl(fd,VIDIOC_REQBUFS,&req) == -1) {
		printf("request for buffers failed\n");
	}
	printf("request buffers frame success!!\n");

	//2.get memory kernel address and length;then mmap into user memory space 
	buffers =(VideoBuffer*) malloc(req.count * sizeof(VideoBuffer));
	if(!buffers) {
		printf("malloc memory error\n");
		return FALSE;
	}

	for(n_buffers = 0;n_buffers < req.count;n_buffers++) {
		buf.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
		buf.memory = V4L2_MEMORY_MMAP;
		buf.index = n_buffers;
		printf("index = %d\n",buf.index);

		//mmp frame buffers into user space;read kernel frame buffers 
		if(ioctl(fd,VIDIOC_QUERYBUF,&buf) == -1) {
			printf("query buffer error\n");
			return(FALSE);
		}
		
		//get buffers length;change to address
		buffers[n_buffers].length = buf.length;
		buffers[n_buffers].start = mmap(NULL,buf.length,PROT_READ | PROT_WRITE,MAP_SHARED,fd,buf.m.offset);
		
		if(buffers[n_buffers].start == MAP_FAILED) {
			printf("buffer map error\n");
			return FALSE;
		}
		printf("Frame buffer %d:address = %p ,length = %ld \n",req.count,buffers[n_buffers].start,buffers[n_buffers].length);

		//put into buffer queue
		if(ioctl(fd,VIDIOC_QBUF,&buf) == -1) {
			printf("VIDIOC_QBUF put frame buffers into buffer queue\n");
			return FALSE;
		};
	}

	// start cap video stream
	type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
	if(ioctl(fd,VIDIOC_STREAMON,&type) == -1) {
		printf("start cap video stream failed\n");
		return FALSE;
	};
	printf("cap vifsdfeo stream success!\n");
	
	//get out frame buffer data from queue
	if(ioctl(fd,VIDIOC_DQBUF,&buf) == -1) {
		printf("VIDIOC_DQBUF get out frame buffer data from queue failed\n");
		return FALSE;
	};
	printf("GRAB YUYV ok\n");

	//save yuyv image file
	printf("n_buffers = %d\n",n_buffers);
	fwrite(buffers[0].start,buffers[0].length,1,fd_file);

	return TURE;
}


/*
函数功能: 将YUV数据转为RGB格式
函数参数:
unsigned char *yuv_buffer: YUV源数据
unsigned char *rgb_buffer: 转换之后的RGB数据
int iWidth,int iHeight   : 图像的宽度和高度
*/
void yuyv_to_rgb(unsigned char *yuv_buffer,unsigned char *rgb_buffer,int iWidth,int iHeight)
{
    int x;
    int z=0;
    unsigned char *ptr = rgb_buffer;
    unsigned char *yuyv= yuv_buffer;
    for (x = 0; x < iWidth*iHeight; x++)
    {
        int r, g, b;
        int y, u, v;
 
        if (!z)
        y = yuyv[0] << 8;
        else
        y = yuyv[2] << 8;
        u = yuyv[1] - 128;
        v = yuyv[3] - 128;
 
        r = (y + (359 * v)) >> 8;
        g = (y - (88 * u) - (183 * v)) >> 8;
        b = (y + (454 * u)) >> 8;
 
        *(ptr++) = (r > 255) ? 255 : ((r < 0) ? 0 : r);
        *(ptr++) = (g > 255) ? 255 : ((g < 0) ? 0 : g);
        *(ptr++) = (b > 255) ? 255 : ((b < 0) ? 0 : b);
 
        if(z++)
        {
            z = 0;
            yuyv += 4;
        }
    }
}

int yuyv_2_rgb888(const void *p, int width,int height, unsigned char *frame_buffer)
{
    int i,j;
    unsigned char y1,y2,u,v;
    int r1,g1,b1,r2,g2,b2;
    const char *pointer;

    pointer = (const char *)p;

    for(i=0;i<height;i++)
    {
        for(j=0;j<width/2;j++)
        {
            y1 = *( pointer + (i*320+j)*4);
            u  = *( pointer + (i*320+j)*4 + 1);
            y2 = *( pointer + (i*320+j)*4 + 2);
            v  = *( pointer + (i*320+j)*4 + 3);

            r1 = y1 + 1.042*(v-128);
            g1 = y1 - 0.34414*(u-128) - 0.71414*(v-128);
            b1 = y1 + 1.772*(u-128);

            r2 = y2 + 1.042*(v-128);
            g2 = y2 - 0.34414*(u-128) - 0.71414*(v-128);
            b2 = y2 + 1.772*(u-128);

            if(r1>255)                r1 = 255;
            else if(r1<0)             r1 = 0;

            if(b1>255)                b1 = 255;
            else if(b1<0)             b1 = 0;

            if(g1>255)                g1 = 255;
            else if(g1<0)             g1 = 0;

            if(r2>255)                r2 = 255;
            else if(r2<0)             r2 = 0;

            if(b2>255)                b2 = 255;
            else if(b2<0)             b2 = 0;

            if(g2>255)                g2 = 255;
            else if(g2<0)             g2 = 0;

            *(frame_buffer + ((480-1-i)*320+j)*6    ) = (unsigned char)b1;
            *(frame_buffer + ((480-1-i)*320+j)*6 + 1) = (unsigned char)g1;
            *(frame_buffer + ((480-1-i)*320+j)*6 + 2) = (unsigned char)r1;
            *(frame_buffer + ((480-1-i)*320+j)*6 + 3) = (unsigned char)b2;
            *(frame_buffer + ((480-1-i)*320+j)*6 + 4) = (unsigned char)g2;
            *(frame_buffer + ((480-1-i)*320+j)*6 + 5) = (unsigned char)r2;
        }
    }
   // pr_debug("\tchange to RGB OK \n");
    return 0;
}

int main()
{
	FILE* fp_bmp;
	FILE* fp_yuv;
	BITMAPFILEHEADER bmp_file_header;
	BITMAPINFOHEADER bmp_info_header;

	fp_yuv = fopen(YUY2IMG,"wb");
	if(!fp_yuv) {
		printf("open YUY2IMG failed\n");
		exit(-1);
	}

	init_v4l2();
	v4l2_grab(fp_yuv);
/*
	unsigned char  frame_buffer[5000000] = {0};
	yuyv_2_rgb888((const void*)buffers[0].start,IMAGEWIDTH,IMAGEHEIGHT,frame_buffer);

	fp_bmp = fopen(RGBIMG,"wb");
	if(!fp_bmp) {
		printf("open bamp image failed\n");
		exit(-1);
	}
	bmp_info_header.biSize			= 40;
	bmp_info_header.biWidth			= IMAGEWIDTH;
	bmp_info_header.biHeight		= IMAGEHEIGHT;
	bmp_info_header.biPlanes		= 1;
	bmp_info_header.biBitcount		= 24;
	bmp_info_header.biCompression	= 0;
	bmp_info_header.biSizeImage		= IMAGEWIDTH * IMAGEHEIGHT * 3;
	bmp_info_header.biXPelsPermeter = 0;
	bmp_info_header.biYPelsPermeter = 0;
	bmp_info_header.biClrUsed		= 0;
	bmp_info_header.biClrImportant	= 0;
	
	bmp_file_header.bfType			= 0x4d42;
	bmp_file_header.bfSize			= 54 + bmp_info_header.biSizeImage;
	bmp_file_header.bfReserverd1	= 0;
	bmp_file_header.bfReserverd2	= 0;
	bmp_file_header.bfOffBits		= 54;
	
	fwrite(&bmp_file_header, 14, 1 ,fp_bmp);
	fwrite(&bmp_info_header, 40, 1, fp_bmp);
	fwrite(frame_buffer,bmp_info_header.biSizeImage,1,fp_bmp);
	printf("save bmp ok\n");
*/
	fclose(fp_yuv);
//	fclose(fp_bmp);

	return 0;

}
