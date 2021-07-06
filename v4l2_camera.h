#ifndef __V4L2_CAMERA_H
#define __V4L2_CAMERA_H

#include <linux/videodev2.h>
#include <linux/v4l2-controls.h>
#include <stdio.h>

#define _DT_CHR     2
#define COUNT      5 //capture video buffer frame number

//define return error value
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
#define MAXVIDEONUM 20
#define PATHLEN     300

//define defult resolution width * height 
#define IMAGEWIDTH   2000
#define IMAGEHEIGHT  1300
#define FPS          30
#define EXPOSURE_DEFULT 300


const char lamp_device_name[20][20] = {"Lamp_Device_1","Lamp_Device_2","Lamp_Device_3","Lamp_Device_4","Lamp_Device_5",
                                       "Lamp_Device_6","Lamp_Device_7","Lamp_Device_8","Lamp_Device_9","Lamp_Device_10",
                                       "Lamp_Device_11","Lamp_Device_12","Lamp_Device_13","Lamp_Device_14","Lamp_Device_15",
                                       "Lamp_Device_16","Lamp_Device_17","Lamp_Device_18","Lamp_Device_19","Lamp_Device_20"};
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

typedef struct camera_device_info{
	char device_name[20];  //one camera have a device name  /dev/video0
    char lamp_device_name[20];  //"Lamp_Device_1"
	camera_paras_t camera_paras_info; //one camera have it's parameters
}camera_device_info_t;

 typedef struct camera_list{
     camera_device_info_t camera_device_info[MAXVIDEONUM];
     unsigned int camera_num;
 }camera_list_t;                                                                 

/* mmap to user spaces buffer */
struct VideoBuffer {
    void* start;//mmap start addr
    size_t length;
};
int v4l2_get_camera_device_info();
int v4l2_queryctrl_value(int cam_fd,int V4L2_cid);
int v4l2_get_ctrl_value(int cam_fd,int V4L2_cid);
int v4l2_set_white_balance(int cam_fd,int val_white_balance_temp);
int v4l2_set_exposure(int cam_fd,int val_exposure);
int v4l2_set_ctrl_value(int cam_fd,int V4L2_cid,int V4L2_value);
int v4l2_set_fmtd(int cam_fd,int width,int height);
int v4l2_set_streamparm(int cam_fd,int fps);

int v4l2_init_camera(void);
int v4l2_camera_reqbuff(int cam_fd,int camera_index);
int v4l2_start_camera(FILE* file,int cam_fd,int camera_index);
int v4l2_stop_camera(int cam_fd,int camera_index);
#endif 
