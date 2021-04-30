#include <iostream>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/opencv.hpp>
#include <opencv2/videoio.hpp>
#include <fstream>
#include <unistd.h>


#include "v4l2_camera.h"
using namespace std;
using namespace cv;
extern struct VideoBuffer* buffers[MAXVIDEONUM];
extern int cam_FdList[MAXVIDEONUM]; 
extern camera_list_t cameraList;
const int framesize = IMAGEWIDTH * IMAGEHEIGHT * 2;   //一副图所含的像素个数

void img_resize_show(char* path)
{
	//resize show
	Mat img = imread(path);
	Mat imgResize;
	resize(img,imgResize,Size(),0.5,0.5);
	imshow("imgResize",imgResize);

}

int main()
{
	int ret= v4l2_init_camera();
	if(ret < 0) {
		printf("init vdl2 failed\n");
		return ret;
	}

	for(int i=0;i<(int)cameraList.camera_num;i++) {
		ret = v4l2_camera_reqbuff(cam_FdList[i],i);
		if(ret < 0) {
			printf("request buffer error\n");
			return ret;
		}
	}
	
	sleep(10);
	while(1) {
		for(int i = 0;i < (int)cameraList.camera_num;i++) {
			unsigned char* pYuvBuf = new unsigned char[framesize]; //一帧数据大小
			//create a image file of yuv format,no data
			char* path = new char[100];
			sprintf(path,"./image/camera%d_img_yuv.yuv",i+1);
			FILE* file_yuv = fopen(path,"wb");
			if(!file_yuv) {
				printf("open YUY2IMG failed\n");
				exit(-1);
			}
			else {
				printf("create yuyv file success!\n");
			}

			//start camera and wrrite data to the yuv image,one frame
			int ret = v4l2_start_camera(file_yuv,cam_FdList[i],i);
			if(ret < 0) {
				printf("cap image failed\n");
				return ret;
			}
			fclose(file_yuv);
			
			//open the yuv image ;read data to pYuvBuf
			FILE* fileIn = fopen(path, "rb+");
			if(!fileIn) {
				printf("open YUY2IMG failed\n");
				exit(-1);
			}
			else {
				printf("open yuyv file success!\n");
			}
			fread(pYuvBuf, framesize*sizeof(unsigned char), 1, fileIn);
			fclose(fileIn);

			//yuv to rgb
			printf("length = %ld\n",strlen((char*)pYuvBuf));
			cv::Mat yuvImg;
			cv::Mat rgbImg(IMAGEHEIGHT, IMAGEWIDTH, CV_8UC3);
			yuvImg.create(IMAGEHEIGHT, IMAGEWIDTH, CV_8UC2);
			memcpy(yuvImg.data, pYuvBuf, framesize);
			cv::cvtColor(yuvImg, rgbImg, CV_YUV2BGR_YUYV);
			
			/*save a image of jpg format */
			vector<int> compression_params;
			compression_params.push_back(CV_IMWRITE_JPEG_QUALITY);
			compression_params.push_back(100);
			char* img_name = new char[100];
			sprintf(img_name,"./image/camera%d_img_jpg.jpg",i+1);
			imwrite(img_name,rgbImg,compression_params);

			//show
			img_resize_show(img_name);

			//free
			free(pYuvBuf);
			free(path);
			free(img_name);
			yuvImg.release();
			rgbImg.release();
			printf("-----------------------------------------------------------------------------------------------------------------\n");
			waitKey(10);
		}
	}
	return 0;
}
