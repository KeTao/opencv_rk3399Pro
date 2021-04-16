#include <stdio.h>
#include <stdlib.h>
#include "camera_test.h"
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <sys/mman.h>
#include <unistd.h>

VideoBuffer* buffers;
int fd_camera;
static struct v4l2_capability cap;
static struct v4l2_fmtdesc fmtdesc;
struct v4l2_format fmt,fmtack;
struct v4l2_streamparm setfps;
struct v4l2_requestbuffers req;
struct v4l2_buffer buf;
enum v4l2_buf_type type;
unsigned char frame_buffer[IMAGEWIDTH * IMAGEHEIGHT];  //RGB color data buffer

int init_v4l2(void)
{
	//open video device
	int ret;

	fd_camera = open(FILE_VIDEO,O_RDWR);
	if(fd_camera == -1) {
		perror("open file");
		return FALSE;
	}	

	ret = ioctl(fd_camera,VIDIOC_QUERYCAP,&cap);
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

	while(ioctl(fd_camera,VIDIOC_ENUM_FMT,&fmtdesc) != -1) {
		printf("\t%d.%s\n",fmtdesc.index + 1,fmtdesc.description);
		fmtdesc.index++;
	}

	//set video pixel format
	fmt.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
	fmt.fmt.pix.pixelformat = V4L2_PIX_FMT_YUYV;
	fmt.fmt.pix.height = IMAGEHEIGHT;
	fmt.fmt.pix.width = IMAGEWIDTH;
	fmt.fmt.pix.field = V4L2_FIELD_INTERLACED;

	if(ioctl(fd_camera,VIDIOC_S_FMT,&fmt) == -1)
	{
		printf("Unable to set format\n");
		return FALSE;
	}

	//get video pixel format
	if(ioctl(fd_camera,VIDIOC_G_FMT,&fmt) == -1 ) {
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


int queue_v4l2(void)
{
	unsigned int n_buffers;

	//1.request kernel buffers(4 frames)
	req.count = 1;
	req.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
	req.memory = V4L2_MEMORY_MMAP;
	if(ioctl(fd_camera,VIDIOC_REQBUFS,&req) == -1) {
		printf("request for buffers failed\n");
		return FALSE;
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
		if(ioctl(fd_camera,VIDIOC_QUERYBUF,&buf) == -1) {
			printf("query buffer error\n");
			return(FALSE);
		}
		
		//get buffers length;change to address
		buffers[n_buffers].length = buf.length;
		buffers[n_buffers].start = mmap(NULL,buf.length,PROT_READ | PROT_WRITE,MAP_SHARED,fd_camera,buf.m.offset);
		
		if(buffers[n_buffers].start == MAP_FAILED) {
			printf("buffer map error\n");
			return FALSE;
		}
		printf("Frame buffer %d:address = %p ,length = %ld \n",req.count,buffers[n_buffers].start,buffers[n_buffers].length);

		//put into buffer queue
		if(ioctl(fd_camera,VIDIOC_QBUF,&buf) == -1) {
			printf("VIDIOC_QBUF put frame buffers into buffer queue\n");
			return FALSE;
		};
	}

	return TURE;
}

int cap_v4l2(FILE* file) 
{
// start cap video stream
  type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
  if(ioctl(fd_camera,VIDIOC_STREAMON,&type) == -1) {
      printf("start cap video stream failed\n");
      return FALSE;
  };
  printf("cap vifsdfeo stream success!\n");
  
  //get out frame buffer data from queue
  if(ioctl(fd_camera,VIDIOC_DQBUF,&buf) == -1) {
      printf("VIDIOC_DQBUF get out frame buffer data from queue failed\n");
      return FALSE;
  };
  printf("GRAB YUYV ok\n");

  //save yuyv image file
  fwrite(buffers[0].start,buffers[0].length,1,file);
  printf("save yuyv ok\n");

  if (ioctl(fd_camera, VIDIOC_QBUF, &buf) == -1) {
      printf("VIDIOC_QBUF error\n");
      return FALSE;
  }



}
int close_v4l2(void)
{
	ioctl(fd_camera,VIDIOC_STREAMOFF,&type);
	if(fd_camera != -1) {
		close(fd_camera);
		return TURE;
	}

	return FALSE;
}
