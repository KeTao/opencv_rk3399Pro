#include <iostream>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/opencv.hpp>
#include <opencv2/videoio.hpp>
#include <fstream>

#include "v4l2_camera.h"
using namespace std;
using namespace cv;
extern VideoBuffer* buffers;
extern int vidDevFd; 
const int framesize = IMAGEWIDTH * IMAGEHEIGHT * 2;   //一副图所含的像素个数
int main()
{
	int ret= v4l2_init_camera();
	if(ret == FALSE) {
		printf("init vdl2 failed\n");
		return 0;
	}


	ret = v4l2_camera_reqbuff();
	if(ret == FALSE) {
		printf("request buffer error\n");
		return 0;
	}


	while(1) {
		unsigned char* pYuvBuf = new unsigned char[framesize]; //一帧数据大小	
		FILE* file_yuv = fopen(YUY2IMG,"wb");
		if(!file_yuv) {
			printf("open YUY2IMG failed\n");
			exit(-1);
		}
		else {
			printf("create yuyv file success!\n");
		}

		int ret = v4l2_start_camera(file_yuv);
		if(ret == FALSE) {
			printf("cap image failed\n");
			return 0;
		}
		fclose(file_yuv);
		
		FILE* fileIn = fopen(YUY2IMG, "rb+");
		if(!fileIn) {
			printf("open YUY2IMG failed\n");
			exit(-1);
		}
		else {
			printf("open yuyv file success!\n");
		}
		fread(pYuvBuf, framesize*sizeof(unsigned char), 1, fileIn);
		fclose(fileIn);
		
		printf("length = %ld\n",strlen((char*)pYuvBuf));
		cv::Mat yuvImg;
		cv::Mat rgbImg(IMAGEHEIGHT, IMAGEWIDTH, CV_8UC3);
		yuvImg.create(IMAGEHEIGHT, IMAGEWIDTH, CV_8UC2);
		memcpy(yuvImg.data, pYuvBuf, framesize);
		cv::cvtColor(yuvImg, rgbImg, CV_YUV2BGR_YUYV);
		Mat imgResize;
		resize(rgbImg,imgResize,Size(),0.5,0.5);
		imshow("rgbImg",rgbImg);
		imshow("imgResize",imgResize);

		vector<int> compression_params;
		compression_params.push_back(CV_IMWRITE_JPEG_QUALITY);
		compression_params.push_back(100);

		imwrite("1.jpg",imgResize,compression_params);
		
		free(pYuvBuf);	
		printf("-----------------------------------------------------------------------------------------------------------------\n");
		waitKey(10);
	}
	return 0;
}
