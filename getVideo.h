#ifndef __GETVIDEO_H
#define __GETVIDEO_H

#define MAXVIDEONUM 20
#define PATHLEN     300

typedef struct camera_device_info{
    char device_name[20];
}camera_device_info_t;

typedef struct camera_list{
	camera_device_info_t camera_device_info[MAXVIDEONUM];
	unsigned int camera_num;
}camera_list_t;

#endif
