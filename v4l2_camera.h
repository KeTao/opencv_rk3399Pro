#ifndef __V4L2_CAMERA_H
#define __V4L2_CAMERA_H

#include <linux/videodev2.h>
#include <stdio.h>
#define TURE  0
#define FALSE -1

#define FILE_VIDEO "/dev/video0"
#define YUY2IMG  "/home/firefly/work_3399/chenlvqing/opencv_rk3399Pro/image/img_yuv.yuv" //保存yuv格式的數據

//截取圖片的分辨率 
#define IMAGEWIDTH   1280
#define IMAGEHEIGHT  720

typedef struct VideoBuffer {
    void* start;//mmap start addr
    size_t length;
}VideoBuffer;

int v4l2_init_camera(void);
int v4l2_camera_reqbuff(void);
int v4l2_start_camera(FILE* file);
int v4l2_stop_camera(void);

#endif 
