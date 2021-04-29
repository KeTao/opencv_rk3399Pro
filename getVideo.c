#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <dirent.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <linux/videodev2.h>
#include <linux/v4l2-controls.h>
#include <sys/ioctl.h>
#include "getVideo.h"
//get all video device from /dev

#define MAXVIDEONUM 20
#define PATHLEN     300

camera_list_t cameraList;

int main()
{
	int fds_video[MAXVIDEONUM] = {0};
	char devicename[PATHLEN] = {0};
	char videoname[MAXVIDEONUM][PATHLEN];
	memset(videoname,0,sizeof(videoname));
	cameraList.camera_num = 0;
	/* open /dev dir;then read the file in this dir */
	DIR* dir = opendir("/dev");
	struct dirent *ptr;

	if(dir == NULL) {
		printf("open /dev failed\n ");
		return -1;
	}

	int count = 0;
	while((ptr = readdir(dir)) != NULL)
	{
		if(ptr->d_type == DT_CHR)
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
				fds_video[i] = fd;	
				strcpy(cameraList.camera_device_info[cameraList.camera_num].device_name,videoname[i]);
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
