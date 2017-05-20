#ifndef SOURCE_HEADER
#define SOURCE_HEADER

#include <iostream>
#include <opencv2\opencv.hpp>

class GreenScreen {
private:
	const int IMAGE_WIDTH = 640;
	const int IMAGE_HEIGHT = 480;
	const int WAIT_IN_MS = 20;

	cv::VideoCapture cap;

	int initWindow();
	int processFrame();
	int greenScreen(const cv::Mat& inputFrame, cv::Mat& outputFrame);
public:
	GreenScreen();

	int mainLoop();
};

#endif SOURCE_HEADER