#ifndef __V4L2_CAMERA_H
#define __V4L2_CAMERA_H

#include <linux/videodev2.h>
<<<<<<< HEAD
#include <linux/v4l2-controls.h>

#include <stdio.h>


#define FILE_VIDEO "/dev/video0"
#define YUY2IMG  "/home/firefly/work_3399/chenlvqing/opencv_rk3399Pro/image/img_yuv.yuv" //保存yuv格式的數據
#define COUNT  4
#define CAMERA_GET_DEVICE_INFO_ERROE -1
#define CAMERA_OPEN_ERROR  -2
#define VIDIOC_QUERYCAP_ERROR -3
#define VIDIOC_S_FMT_ERROR    -4
#define VIDIOC_G_FMT_ERROR    -5
#define VIDIOC_S_PARM_ERROR    -6
#define VIDIOC_G_PARM_ERROR    -7
#define VIDIOC_REQBUFS_ERROR    -8
#define MALLOC_ERROR            -9
#define VIDIOC_QUERYBUF_ERROR   -10
#define MMP_ERROR -11
#define VIDIOC_QBUF_ERROR  -12
#define VIDIOC_STREAMON_ERROR  -13
#define VIDIOC_DQBUF_ERROR   -14
#define UMMMP_ERROR -15
#define VIDIOC_STREAMOFF_ERROR -16


#define V4L2_UTILS_SET_EXPSURE_AUTO_TYPE_ERR  -55
#define V4L2_UTILS_SET_EXPSURE_ERR            -54
#define V4L2_UTILS_SET_WHITE_BALANCE_AUTO_TYPE_ERR  -57
#define V4L2_UTILS_SET_WHITE_BALANCE_ERR    -58
#define V4L2_UTILS_SET_BRIGHTNESS_ERR    -59

typedef struct camera_contrl_paras {
    unsigned short val_white_balance_temp;  //white balance temperature value
    unsigned short val_brightness; //brightness
    unsigned short val_contrast;
    unsigned short val_saturation;
    unsigned short val_sharpness;
	unsigned short val_exposure;
}camera_contrl_paras_t;

typedef struct camera_paras{
    unsigned short width;       //resolution width
    unsigned short height;      //resolution height
    unsigned short fps;         //frame per second
	camera_contrl_paras_t camera_ctrl_paras;    
}camera_paras_t;

enum camera_flag{
	camera1 = 1,camera2,camera3,camera4,camera5,
	camera6,camera7,camera8,camera9,camera10,
	camera11,camera12,camera13,camera14,camera15,
	camera16,camera17,camera18,camera19,camera20
};
#define MAXVIDEONUM 20
#define PATHLEN     300
typedef struct camera_device_info{
	char device_name[20];  //one camera have a device name
	int  camera_flag;
	camera_paras_t camera_paras_info; //one camera have it's parameters
}camera_device_info_t;

 typedef struct camera_list{
     camera_device_info_t camera_device_info[MAXVIDEONUM];
     unsigned int camera_num;
 }camera_list_t;                                                                 



//define defult resolution width * height 
#define IMAGEWIDTH   1280
#define IMAGEHEIGHT  720
#define FPS          30
#define EXPOSURE_DEFULT 300
=======
#include <stdio.h>
#define TURE  0
#define FALSE -1

#define FILE_VIDEO "/dev/video0"
#define YUY2IMG  "/home/firefly/work_3399/chenlvqing/opencv_rk3399Pro/image/img_yuv.yuv" //保存yuv格式的數據

//截取圖片的分辨率 
#define IMAGEWIDTH   1280
#define IMAGEHEIGHT  720

>>>>>>> 6a8396cae019b88c2eb1162583e05dfac5ab3e40
typedef struct VideoBuffer {
    void* start;//mmap start addr
    size_t length;
}VideoBuffer;

int v4l2_init_camera(void);
<<<<<<< HEAD
int v4l2_camera_reqbuff(int cam_fd,int camera_flag);
=======
int v4l2_camera_reqbuff(void);
>>>>>>> 6a8396cae019b88c2eb1162583e05dfac5ab3e40
int v4l2_start_camera(FILE* file);
int v4l2_stop_camera(void);

#endif 
