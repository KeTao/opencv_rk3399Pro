#ifndef __CAMERA_TEST_H
#define __CAMERA_TEST_H

#include <linux/videodev2.h>
#include <stdio.h>
#define TURE  0
#define FALSE -1

#define FILE_VIDEO "/dev/video0"
#define YUY2IMG  "/home/firefly/work_3399/chenlvqing/image/img_yuv.yuv"
#define RGBIMG   "/home/firefly/work_3399/chenlvqing/image/img_bmp.bmp"

#define IMAGEWIDTH   640
#define IMAGEHEIGHT  480

typedef struct VideoBuffer {
    void* start;//mmap start addr
    size_t length;
}VideoBuffer;

VideoBuffer* buffers;

//BMP FILE header
typedef  struct tagBITMAPFILEHEADER
{ 
	unsigned short int	bfType;      //位图文件的类型，必须为BM 
	unsigned long		bfSize;      //文件大小，以字节为单位
	unsigned short int  bfReserverd1; //位图文件保留字，必须为0 
	unsigned short int  bfReserverd2; //位图文件保留字，必须为0 
	unsigned long		bfOffBits;  //位图文件头到数据的偏移量，以字节为单位
}BITMAPFILEHEADER; 

//BMP info header
typedef  struct tagBITMAPINFOHEADER 
{ 
	long biSize;                       //该结构大小，字节为单位
	long biWidth;                    //图形宽度以象素为单位
	long biHeight;                    //图形高度以象素为单位
	short int biPlanes;              //目标设备的级别，必须为1 
	short int biBitcount;            //颜色深度，每个象素所需要的位数
	short int biCompression;       //位图的压缩类型
	long biSizeImage;             //位图的大小，以字节为单位
	long biXPelsPermeter;      //位图水平分辨率，每米像素数
	long biYPelsPermeter;      //位图垂直分辨率，每米像素数
	long biClrUsed;           //位图实际使用的颜色表中的颜色数
	long biClrImportant;      //位图显示过程中重要的颜色数
}BITMAPINFOHEADER; 

static int fd;
static struct v4l2_capability cap;
static struct v4l2_fmtdesc fmtdesc;
struct v4l2_format fmt,fmtack;
struct v4l2_streamparm setfps;
struct v4l2_requestbuffers req;
struct v4l2_buffer buf;
enum v4l2_buf_type type;
unsigned char frame_buffer[IMAGEWIDTH * IMAGEHEIGHT];  //RGB color data buffer

int init_v4l2(void); 
int v4l2_grab(FILE* fd_file);
int close_v4l2(FILE* fd_file);


#endif 
