#include <stdio.h>
#include <stdlib.h>
#include "v4l2_camera.h"
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <sys/mman.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>

#define COUNT 4
int vidDevFd = -1;
static int number_frame = 0;
VideoBuffer* buffers; //user

int v4l2_init_camera(void)
{
	struct v4l2_capability cap;
	struct v4l2_fmtdesc fmtdesc;
	struct v4l2_format fmt;
	int ret;
	//1.open video device
	vidDevFd = open(FILE_VIDEO,O_RDWR);
	if(vidDevFd == -1) {
		printf("open %s failed;err info = %s\n",FILE_VIDEO,strerror(errno));
		return FALSE;
	}	

	//2.get the video info
	ret = ioctl(vidDevFd,VIDIOC_QUERYCAP,&cap);
	if(ret == -1) {
		printf("VIDIOC_QUERYCAP failed\n");
		return FALSE;
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
	while(ioctl(vidDevFd,VIDIOC_ENUM_FMT,&fmtdesc) != -1) {
		printf("\t%d.%s\n",fmtdesc.index + 1,fmtdesc.description);
		fmtdesc.index++;
	}

	//3.set default video capture parameters設置默認的攝像頭捕獲參數
	fmt.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
	fmt.fmt.pix.pixelformat = V4L2_PIX_FMT_YUYV;
	fmt.fmt.pix.height = IMAGEHEIGHT;
	fmt.fmt.pix.width = IMAGEWIDTH;
	fmt.fmt.pix.field = V4L2_FIELD_INTERLACED;

	if(ioctl(vidDevFd,VIDIOC_S_FMT,&fmt) == -1)
	{
		printf("Unable to set format\n");
		return FALSE;
	}

	//4.check having set video capture parameters
	if(ioctl(vidDevFd,VIDIOC_G_FMT,&fmt) == -1 ) {
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

//reqest frame buffer;mmp into user space;put into output queue
int v4l2_camera_reqbuff(void)
{
	struct v4l2_requestbuffers reqBuffer;//向內核申請幀緩衝
	struct v4l2_buffer buf[COUNT];//供用戶空間調用的幀緩衝 一个struct v4l2_buffer对应内核摄像头驱动中的一个缓存

	//1.request kernel buffers(4 frames)
	reqBuffer.count = COUNT;
	reqBuffer.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
	reqBuffer.memory = V4L2_MEMORY_MMAP;
	if(ioctl(vidDevFd,VIDIOC_REQBUFS,&reqBuffer) == -1) {
		printf("request for buffers failed\n");
		return FALSE;
	}
	printf("request buffers frame success!!\n");

	//2.get memory kernel address and length;then mmap into user memory space 
	buffers =(VideoBuffer*) malloc(reqBuffer.count * sizeof(VideoBuffer));
	if(!buffers) {
		printf("malloc memory error\n");
		return FALSE;
	}

	for(int i = 0;i < reqBuffer.count;i++) {
		buf[i].type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
		buf[i].memory = V4L2_MEMORY_MMAP;
		buf[i].index = i;

		//mmp frame buffers into user space;read kernel frame buffers 
		if(ioctl(vidDevFd,VIDIOC_QUERYBUF,&buf[i]) == -1) {
			printf("query buffer error\n");
			return(FALSE);
		}
		
		//get buffers length;change to address
		buffers[i].length = buf[i].length;
		buffers[i].start = mmap(NULL,buf[i].length,PROT_READ | PROT_WRITE,MAP_SHARED,vidDevFd,buf[i].m.offset);
		
		if(buffers[i].start == MAP_FAILED) {
			printf("buffer map error\n");
			return FALSE;
		}
		printf("Frame buffer %d:address = %p ,length = %ld \n",reqBuffer.count,buffers[i].start,buffers[i].length);

		//put into buffer queue
		if(ioctl(vidDevFd,VIDIOC_QBUF,&buf[i]) == -1) {
			printf("VIDIOC_QBUF put frame buffers into buffer queue\n");
			return FALSE;
		};
	}

	return TURE;
}

int v4l2_start_camera(FILE* file) 
{
	// start cap video stream
	enum v4l2_buf_type type;
	type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
	if(ioctl(vidDevFd,VIDIOC_STREAMON,&type) == -1) {
		printf("start cap video stream failed\n");
		return FALSE;
	}
	printf("cap vidieo stream success!\n");
  
	struct v4l2_buffer v4lbuf;
    memset(&v4lbuf,0, sizeof(v4lbuf));
    v4lbuf.type  = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    v4lbuf.memory= V4L2_MEMORY_MMAP;
	v4lbuf.index = number_frame % COUNT;
	printf("index = %d\n",v4lbuf.index);
	//get out frame buffer data from queue
	if(ioctl(vidDevFd,VIDIOC_DQBUF,&v4lbuf) == -1) {
		printf("VIDIOC_DQBUF get out frame buffer data from queue failed\n");
		return FALSE;
	};
	printf("GRAB YUYV ok\n");

	//save yuyv image file
	fwrite(buffers[v4lbuf.index].start,buffers[v4lbuf.index].length,1,file);
	printf("save yuyv ok\n");

	if (ioctl(vidDevFd, VIDIOC_QBUF, &v4lbuf) == -1) {
		printf("VIDIOC_QBUF error\n");
		return FALSE;
	}
  number_frame ++;

  return TURE;
}


int v4l2_stop_camera(void)
{
	//unmap
	for (int i = 0; i < COUNT; i++)
	{
		if (NULL != buffers[i].start)
		{
			if (-1 == munmap(buffers[i].start, buffers[i].length));
			{
				printf("unmap failed\n");
				return FALSE;
			}
			buffers[i].start = NULL;
		}
	}

	//stop camera and close fd
	enum v4l2_buf_type type;
	type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
	ioctl(vidDevFd,VIDIOC_STREAMOFF,&type);
	if(vidDevFd != -1) {
		close(vidDevFd);
		return TURE;
	}

	return FALSE;
}
