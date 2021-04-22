#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <dirent.h>
#include <sys/stat.h>
#include <fcntl.h>
 #include <unistd.h>

//get all video device from /dev

#define MAXVIDEONUM 20
#define PATHLEN     300
int fds_video[MAXVIDEONUM] = {0};
int main()
{
	char devicename[PATHLEN] = {0};
	char videoname[MAXVIDEONUM][PATHLEN];
	memset(videoname,0,sizeof(videoname));

	
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
			fds_video[i] = fd;	
			close(fd);
		}
	}
						
	return 0;
}
