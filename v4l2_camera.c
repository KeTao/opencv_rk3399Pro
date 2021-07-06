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
#include <dirent.h>

int cam_FdList[MAXVIDEONUM] = {0};//all camera device fd
int number_frame[MAXVIDEONUM] = {0};//all the video frame 
struct VideoBuffer* buffers[MAXVIDEONUM]; //user space frame buffer for 
camera_list_t cameraList;

/* get camera device info for client */
int v4l2_get_camera_device_info()
{
    char devicename[PATHLEN] = {0};
    char videoname[MAXVIDEONUM][PATHLEN];
    memset(videoname,0,sizeof(videoname));
    cameraList.camera_num = 0;
    /* open /dev dir;then read the file in this dir */
    DIR* dir = opendir("/dev");
    struct dirent *ptr;

    if(dir == NULL) {
        printf("open /dev failed\n");
        return -1;
    }

    int count = 0;
    while((ptr = readdir(dir)) != NULL)
    {
        if(ptr->d_type == _DT_CHR)
        {
            for(int i = 0; i < MAXVIDEONUM; i++) {
                sprintf(devicename,"video%d",i);
                if(!strcmp(ptr->d_name,devicename)) {
                    sprintf(devicename,"/dev/%s",ptr->d_name);
                    strcpy(videoname[count],devicename);
                    count++;
                }
            }
        }
    }

    struct v4l2_format fmt;
     //set default video capture parameters設置默認的攝像頭捕獲參數
    fmt.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    fmt.fmt.pix.pixelformat = V4L2_PIX_FMT_YUYV;
    fmt.fmt.pix.height = 720;
    fmt.fmt.pix.width  = 1280;
    fmt.fmt.pix.field = V4L2_FIELD_INTERLACED;

    for(int i = 0;i< count;i++)
    {
        printf("video name = %s\n",videoname[i]);
        int fd = open(videoname[i],O_RDONLY);
        if(fd < 0) {
            printf("open %s failed\n",videoname[i]);
            close(fd);
        }
        else {
            printf("open %s success\n",videoname[i]);
            if(ioctl(fd,VIDIOC_S_FMT,&fmt) != -1) {
                strcpy(cameraList.camera_device_info[cameraList.camera_num].device_name,videoname[i]);
				strcpy(cameraList.camera_device_info[cameraList.camera_num].lamp_device_name,lamp_device_name[i]);
                cameraList.camera_num++;
            }
            close(fd);
        }
    }

    printf("in this client ,having %d devices\n",cameraList.camera_num);
    for(int i =0;i< cameraList.camera_num;i++) {
        printf("in this client,device [%d] is [%s]\n",i,cameraList.camera_device_info[i].device_name);
    }

    return 0;
}

/* query camera control parameters fot tets */
int v4l2_queryctrl_value(int cam_fd,int V4L2_cid)
{
	struct v4l2_queryctrl queryctrl;
	queryctrl.id = V4L2_cid;
	int ret = ioctl(cam_fd, VIDIOC_QUERYCTRL, &queryctrl);
	if(ret < 0) {
		perror("ioctl VIDIOC_QUERYCTRL");
		return -1;
	}

	printf("v4l2_queryctrl.mini = %d v4l2_queryctrl.max = %d v4l2_queryctrl.step = %d\n",queryctrl.minimum,queryctrl.maximum,queryctrl.step);
	return 0;
}

/* get camera control parameters value by v4l2_cid;for example brightness contrast ...*/
int v4l2_get_ctrl_value(int cam_fd,int V4L2_cid)
{
	struct v4l2_control ctrl;
	ctrl.id = V4L2_cid;
	int ret = ioctl(cam_fd, VIDIOC_G_CTRL, &ctrl);
	if(ret < 0) {
		perror("ioctl VIDIOC_G_CTRL");
		return -1;
	}

	printf("VALUE IS %d\n",ctrl.value);
	
	return ctrl.value;
}

/* set white balance temperature cvalue 2800 ~ 6500，default = 4600 */
int v4l2_set_white_balance(int cam_fd,int val_white_balance_temp)
{
	struct v4l2_control ctrl;
    ctrl.id = V4L2_CID_AUTO_WHITE_BALANCE;
    ctrl.value = V4L2_WHITE_BALANCE_MANUAL ;
    if(ioctl(cam_fd,VIDIOC_S_CTRL,&ctrl)==-1)
    {
        perror("ioctl VIDIOC_G_CTRL for V4L2_CID_AUTO_WHITE_BALANCE");
        return V4L2_UTILS_SET_WHITE_BALANCE_AUTO_TYPE_ERR;
    }

    ctrl.id = V4L2_CID_WHITE_BALANCE_TEMPERATURE;
    ctrl.value = val_white_balance_temp;
     if(ioctl(cam_fd,VIDIOC_S_CTRL,&ctrl)==-1)
    {
        perror("ioctl VIDIOC_S_CTRL for V4L2_CID_WHITE_BALANCE_TEMPERATURE");
        return V4L2_UTILS_SET_WHITE_BALANCE_ERR;
    }

	 printf("set white balance ok\n");
	return 0;
}

/* set exposure 300us~9900us default = auto exposure mode */
int v4l2_set_exposure(int cam_fd,int val_exposure)
{
	int ret;
	struct v4l2_control ctrl;

	ctrl.id = V4L2_CID_EXPOSURE_AUTO;
	ctrl.value = V4L2_EXPOSURE_MANUAL;//手动曝光模式
	ret = ioctl(cam_fd, VIDIOC_S_CTRL, &ctrl);
	if (ret < 0)
	{
		printf("Get exposure auto Type failed\n");
		return V4L2_UTILS_SET_EXPSURE_AUTO_TYPE_ERR;
	}

	//设置曝光绝对值
	ctrl.id = V4L2_CID_EXPOSURE_ABSOLUTE;
	ctrl.value = val_exposure;  //单位100us
	ret = ioctl(cam_fd, VIDIOC_S_CTRL, &ctrl);
	if (ret < 0)
	{
		printf("Set exposure failed (%d)\n", ret);
		return V4L2_UTILS_SET_EXPSURE_ERR;
	}
	
	printf("set exposure ok\n");
	return 0;
}

/* set camera control parameters value */
int v4l2_set_ctrl_value(int cam_fd,int V4L2_cid,int V4L2_value)
{
	struct v4l2_control ctrl;
	 ctrl.id= V4L2_cid;
	 ctrl.value = V4L2_value;
	 if(ioctl(cam_fd,VIDIOC_S_CTRL,&ctrl)==-1)
     {
        perror("ioctl VIDIOC_S_CTRL error");
        return V4L2_UTILS_SET_BRIGHTNESS_ERR;
     }

	printf("set VIDIOC_S_CTRL ok\n");
	return 0;
}

/* set v4l2_format(resolution for width * height) */
int v4l2_set_fmtd(int cam_fd,int width,int height)
{
	struct v4l2_format fmt;
	//set default video capture parameters設置默認的攝像頭捕獲參數
	fmt.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
	fmt.fmt.pix.pixelformat = V4L2_PIX_FMT_MJPEG;
	fmt.fmt.pix.height = height;
	fmt.fmt.pix.width = width;
	fmt.fmt.pix.field = V4L2_FIELD_INTERLACED;

	if(ioctl(cam_fd,VIDIOC_S_FMT,&fmt) == -1)
	{
		printf("Unable to set format\n");
		return VIDIOC_S_FMT_ERROR;
	}

	//check having set video capture parameters
	if(ioctl(cam_fd,VIDIOC_G_FMT,&fmt) == -1 ) {
		printf("Unable to get format\n");
		return VIDIOC_G_FMT_ERROR;
	}
	
	printf("set v4l2_format ok\n");
	printf("frame h:%d frame w:%d\n",fmt.fmt.pix.height,fmt.fmt.pix.width);
	return 0;
}

/* set v4l2_streamparm for fps */
int v4l2_set_streamparm(int cam_fd,int fps)
{
	int ret;
	//set fps
	struct v4l2_streamparm stream_parm;
	stream_parm.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
	stream_parm.parm.capture.timeperframe.numerator = 1;
	stream_parm.parm.capture.timeperframe.denominator = fps;
	ret = ioctl(cam_fd,VIDIOC_S_PARM,&stream_parm);
	if(ret < 0) {
		printf("Unable to set stream_parm\n");
		return VIDIOC_S_PARM_ERROR;
	}
	
	//check fps
	if(ioctl(cam_fd,VIDIOC_G_PARM,&stream_parm) == -1 ) {
		printf("Unable to get stream_parm\n");
		return VIDIOC_G_PARM_ERROR;
	}

	printf("set v4l2_streamparm ok\n");
	
	return 0;
} 

int v4l2_init_camera(void)
{
	struct v4l2_capability cap;
	struct v4l2_fmtdesc fmtdesc;//视频流相关
	int ret;

	//get all camera device info
	ret = v4l2_get_camera_device_info();
	if(ret < 0) {
		printf("get all camera device error\n ");
		return CAMERA_GET_DEVICE_INFO_ERROE;
	}


	for(int i = 0; i < cameraList.camera_num; i++) {
		//1.open all video device
		cam_FdList[i] = open(cameraList.camera_device_info[i].device_name,O_RDWR);
		if(cam_FdList[i] == -1) {
			printf("open %s failed;err info = %s\n",cameraList.camera_device_info[i].device_name,strerror(errno));
			return CAMERA_OPEN_ERROR;
		}	
		printf("open %s success and device file descriptor is %d\n",cameraList.camera_device_info[i].device_name,cam_FdList[i]);

		//2.get the video info

		ret = ioctl(cam_FdList[i],VIDIOC_QUERYCAP,&cap);
		if(ret == -1) {
			printf("VIDIOC_QUERYCAP failed\n");
			return VIDIOC_QUERYCAP_ERROR;
		}

		//print video capabilities
		printf("driver:\t\t%s\n",cap.driver);
		printf("card:\t\t%s\n",cap.card);
		printf("bus_info:\t\t%s\n",cap.bus_info);
		printf("version:\t\t%d\n",cap.version);
		printf("capabilities:\t\t%x\n",cap.capabilities);
		if((cap.capabilities & V4L2_CAP_VIDEO_CAPTURE) == V4L2_CAP_VIDEO_CAPTURE) {
			printf("Device %s : supports capture.\n",cameraList.camera_device_info[i].device_name);
		}

		if((cap.capabilities & V4L2_CAP_STREAMING) == V4L2_CAP_STREAMING) {
			printf("Device %s:supports streaming.\n",cameraList.camera_device_info[i].device_name);
		}

		//3.get vide0 pixel format
		fmtdesc.index = 0;
		fmtdesc.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
		printf("Supports format:\n");
		while(ioctl(cam_FdList[i],VIDIOC_ENUM_FMT,&fmtdesc) != -1) {
			printf("\t%d.%s\n",fmtdesc.index + 1,fmtdesc.description);
			fmtdesc.index++;
		}

		//set resolution
		ret = v4l2_set_fmtd(cam_FdList[i],IMAGEWIDTH,IMAGEHEIGHT);
		if(ret < 0) {
			printf("set v4l2_format failed\n");
			return ret;
		}
		cameraList.camera_device_info[i].camera_paras_info.width = IMAGEWIDTH;
		cameraList.camera_device_info[i].camera_paras_info.height = IMAGEHEIGHT;

		//set fps
		ret = v4l2_set_streamparm(cam_FdList[i],FPS);
		if(ret < 0) {
			printf("set v4l2_streamparm failed\n");
			return ret;
		}
		cameraList.camera_device_info[i].camera_paras_info.fps = FPS;

		//set exposure 
		v4l2_set_exposure(cam_FdList[i],EXPOSURE_DEFULT);
		cameraList.camera_device_info[i].camera_paras_info.camera_ctrl_paras.val_exposure = EXPOSURE_DEFULT;
		//get video contrl info
		cameraList.camera_device_info[i].camera_paras_info.camera_ctrl_paras.val_white_balance_temp = v4l2_get_ctrl_value(cam_FdList[i],V4L2_CID_WHITE_BALANCE_TEMPERATURE);
		cameraList.camera_device_info[i].camera_paras_info.camera_ctrl_paras.val_brightness = v4l2_get_ctrl_value(cam_FdList[i],V4L2_CID_BRIGHTNESS);
		cameraList.camera_device_info[i].camera_paras_info.camera_ctrl_paras.val_contrast = v4l2_get_ctrl_value(cam_FdList[i],V4L2_CID_CONTRAST);
		cameraList.camera_device_info[i].camera_paras_info.camera_ctrl_paras.val_saturation = v4l2_get_ctrl_value(cam_FdList[i],V4L2_CID_SATURATION);
		cameraList.camera_device_info[i].camera_paras_info.camera_ctrl_paras.val_sharpness = v4l2_get_ctrl_value(cam_FdList[i],V4L2_CID_SHARPNESS);
	}//end for
	return 0;
}

//reqest frame buffer ;mmp into user space;put into output queue for a camera device
int v4l2_camera_reqbuff(int cam_fd,int camera_index)
{
	struct v4l2_requestbuffers reqBuffer;//request frame buffer to kernel
	struct v4l2_buffer buf[COUNT];//use to userspace;a struct v4l2_buffer meomery is the same to kernel buffer

	//1.request kernel buffers(4 frames)
	reqBuffer.count = COUNT;
	reqBuffer.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
	reqBuffer.memory = V4L2_MEMORY_MMAP;
	if(ioctl(cam_fd,VIDIOC_REQBUFS,&reqBuffer) == -1) {
		printf("request for buffers failed\n");
		return VIDIOC_REQBUFS_ERROR;
	}
	printf("request buffers frame success!!\n");

	//2.get memory kernel address and length;then mmap into user memory space 
	buffers[camera_index] =(struct VideoBuffer*) malloc(reqBuffer.count * sizeof(struct VideoBuffer));
	if(!buffers[camera_index]) {
		printf("malloc memory error\n");
		return MALLOC_ERROR;
	}

	for(int i = 0;i < reqBuffer.count;i++) {
		buf[i].type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
		buf[i].memory = V4L2_MEMORY_MMAP;
		buf[i].index = i;

		//mmp frame buffers into user space;read kernel frame buffers 
		if(ioctl(cam_fd,VIDIOC_QUERYBUF,&buf[i]) == -1) {
			printf("query buffer error\n");
			return VIDIOC_QUERYBUF_ERROR;
		}
		//get buffers length;change to address;buf[COUNT]'s memory space is map to buffers[i]
		buffers[camera_index][i].length = buf[i].length;
		buffers[camera_index][i].start = mmap(NULL,buf[i].length,PROT_READ | PROT_WRITE,MAP_SHARED,cam_fd,buf[i].m.offset);
		
		if(buffers[camera_index][i].start == MAP_FAILED) {
			printf("buffer map error\n");
			return MMP_ERROR;
		}
		printf("Frame buffer %d   address = %p ,length = %ld \n",reqBuffer.count,buffers[camera_index][i].start,buffers[camera_index][i].length);

		//put into buffer queue
		if(ioctl(cam_fd,VIDIOC_QBUF,&buf[i]) == -1) {
			printf("VIDIOC_QBUF put frame buffers into buffer queue\n");
			return VIDIOC_QBUF_ERROR;
		};
	}

	printf("request buffer ok for cemera %d\n",camera_index);
	return 0;
}

int v4l2_start_camera(FILE* file,int cam_fd,int camera_index) 
{
	// start cap video stream
	enum v4l2_buf_type type;
	type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
	if(ioctl(cam_fd,VIDIOC_STREAMON,&type) == -1) {
		printf("start cap video stream failed\n");
		return VIDIOC_STREAMON_ERROR;
		return VIDIOC_STREAMON_ERROR;
	}
//	printf("cap vidieo stream success!\n");
  
	struct v4l2_buffer v4lbuf;
    memset(&v4lbuf,0, sizeof(v4lbuf));
    v4lbuf.type  = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    v4lbuf.memory= V4L2_MEMORY_MMAP;
	v4lbuf.index = number_frame[camera_index] % COUNT;
//	printf("index = %d\n",v4lbuf.index);
	//get out frame buffer data from queue
	if(ioctl(cam_fd,VIDIOC_DQBUF,&v4lbuf) == -1) {
		printf("VIDIOC_DQBUF get out frame buffer data from queue failed\n");

		return VIDIOC_DQBUF_ERROR;
	};
//	printf("GRAB YUYV ok\n");
	//save yuyv image file
	fwrite(buffers[camera_index][v4lbuf.index].start,1,buffers[camera_index][v4lbuf.index].length,file);
//	printf("save yuyv ok\n");

	if (ioctl(cam_fd, VIDIOC_QBUF, &v4lbuf) == -1) {
		printf("VIDIOC_QBUF error\n");
		return VIDIOC_QBUF_ERROR;
	}
  	number_frame[camera_index] ++;
  	printf("number_frame[%d] = %d\n",camera_index,number_frame[camera_index]);
  	return 0;
}


int v4l2_stop_camera(int cam_fd,int camera_index)
{
	//unmap
	for (int i = 0; i < COUNT; i++)
	{
		if (NULL != buffers[camera_index][i].start)
		{
			if (-1 == munmap(buffers[camera_index][i].start, buffers[camera_index][i].length))
			{
				printf("unmap failed\n");
				return UMMMP_ERROR;
			}
			buffers[camera_index][i].start = NULL;
		}
	}

	//stop camera and close fd
	enum v4l2_buf_type type;
	type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
	ioctl(cam_fd,VIDIOC_STREAMOFF,&type);
	if(cam_fd != -1) {
		close(cam_fd);
		return 0;
	}
	else
		return VIDIOC_STREAMOFF_ERROR;
}
