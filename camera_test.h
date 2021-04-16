#ifndef __CAMERA_TEST_H
#define __CAMERA_TEST_H

#include <linux/videodev2.h>
#include <stdio.h>
#define TURE  0
#define FALSE -1

#define FILE_VIDEO "/dev/video0"
#define YUY2IMG  "/home/firefly/work_3399/chenlvqing/image/img_yuv.yuv"

#define IMAGEWIDTH   1280
#define IMAGEHEIGHT  720

typedef struct VideoBuffer {
    void* start;//mmap start addr
    size_t length;
}VideoBuffer;

int init_v4l2(void); 
int queue_v4l2(void);
int close_v4l2(void);
int cap_v4l2(FILE* file);


#endif 
