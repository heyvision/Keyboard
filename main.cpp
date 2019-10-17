#include <iostream>
#include <vector>
#include "ImageProc.h"
#include "RealSense.h"
using namespace std;
using namespace cv;
const int image_width = 1280;
const int image_height = 720;
//const string str = "I Love China";
const string str = "Aelo OOe";
int main(int argc, char const *argv[])
{
	CRealSense RealSense;

	RealSense.init(image_width, image_height);
	rs2::frame rgbFrame;
	RealSense.getRGBFrame(rgbFrame);
	cv::Mat img(Size(image_width, image_height), CV_8UC3,
	      (void *)rgbFrame.get_data(), Mat::AUTO_STEP);

	CImageProc ImageProc;
	vector<cv::Point2f> points;
	ImageProc.GetButton(img, str, points);
	return 0;
}