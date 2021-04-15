#include <iostream>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/opencv.hpp>
#include <opencv2/videoio.hpp>
#include <fstream>

using namespace std;
using namespace cv;

const int width = 640;
const int height = 480;
const int framesize = width * height * 2;   //一副图所含的像素个数
#define FrameCount 1

int main()
{
	FILE* fileIn = fopen("img_yuv.yuv", "rb+");
	unsigned char* pYuvBuf = new unsigned char[framesize]; //一帧数据大小
	fread(pYuvBuf, framesize*sizeof(unsigned char), 1, fileIn);
	printf("length = %ld\n",strlen((char*)pYuvBuf));
	fclose(fileIn);

	cv::Mat yuvImg;
	cv::Mat rgbImg(height, width, CV_8UC3);
	yuvImg.create(height, width, CV_8UC2);
	memcpy(yuvImg.data, pYuvBuf, framesize);
	cv::cvtColor(yuvImg, rgbImg, CV_YUV2BGR_YUYV);
	
	imshow("aaa",rgbImg);
	waitKey(0);


		
	return 0;
}
