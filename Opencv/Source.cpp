#include "Source.h"

using namespace std;

GreenScreen::GreenScreen() {
	cap = cv::VideoCapture(0);

	if (initWindow() == -1) {
		exit(-1);
	}
}

int GreenScreen::initWindow() {
	if (!cap.isOpened()) {
		cout << "Cannot open the video cam" << endl;
		return -1;
	}

	// Set cameras to 15fps (if wanted!!!)
	cap.set(CV_CAP_PROP_FPS, 15);

	double dWidth = cap.get(CV_CAP_PROP_FRAME_WIDTH);
	double dHeight = cap.get(CV_CAP_PROP_FRAME_HEIGHT);

	// Set image size
	cap.set(CV_CAP_PROP_FRAME_WIDTH, IMAGE_WIDTH);
	cap.set(CV_CAP_PROP_FRAME_HEIGHT, IMAGE_HEIGHT);

	// display the frame size that OpenCV has picked in order to check 
	cout << "cam Frame size: " << dWidth << " x " << dHeight << endl;
	cv::namedWindow("cam", CV_WINDOW_AUTOSIZE);

	return 0;
}

int GreenScreen::greenScreen(const cv::Mat& inputFrame, cv::Mat& outputFrame) {
	cv::Mat hsvImage;
	cv::Mat grayImage;
	cv::Mat threshHoldImage;
	cv::Mat inputFrameCopy;

	inputFrame.copyTo(inputFrameCopy);

	cv::medianBlur(inputFrameCopy, inputFrameCopy, 3);
	cv::cvtColor(inputFrameCopy, hsvImage, cv::COLOR_BGR2HSV);
	cv::inRange(hsvImage, cv::Scalar(60, 130, 60), cv::Scalar(100, 255, 255), threshHoldImage);

	cv::Canny(threshHoldImage, grayImage, 200, 250, 3);

	vector<vector<cv::Point>> contours;
	vector<cv::Vec4i> hierarchy;
	cv::findContours(grayImage, contours, hierarchy, CV_RETR_TREE, CV_CHAIN_APPROX_SIMPLE, cv::Point(0, 0));

	cv::Mat drawing = cv::Mat::zeros(grayImage.size(), CV_8UC3);

	int max = 0;
	int index;
	for (int i = 0; i < contours.size(); i++) {
		if (contours.at(i).size() > max) {
			max = contours.at(i).size();
			index = i;
		}
	}

	vector<cv::Point> points;
	vector<cv::Point> convexHullPoints;
	if (contours.size() > 0) {
		for (int j = 0; j < contours.at(index).size(); j++) {
			points.push_back(contours.at(index).at(j));
		}

		cv::convexHull(points, convexHullPoints);
		cv::polylines(drawing, convexHullPoints, true, cv::Scalar(255, 255, 255), 2);
	}

	cv::addWeighted(inputFrame, 1, drawing, 1, 1, outputFrame);

	return 0;
}

int GreenScreen::processFrame() {
	cv::Mat inputFrame, outputFrame;
	bool bSuccess = cap.read(inputFrame);

	if (!bSuccess) {
		cout << "Cannot read a frame from video stream" << endl;
		return -1;
	}

	int result = greenScreen(inputFrame, outputFrame) != 0;

	if (result != 0) {
		return result;
	}

	imshow("cam", outputFrame);

	if (cv::waitKey(WAIT_IN_MS) == 27) {
		cout << "ESC key is pressed by user" << endl;
		return -5;
	}

	return 0;
}

int GreenScreen::mainLoop() {
	while (true) {
		int status = processFrame();

		if (status == 0) {
			continue;
		}

		return status;
	}

	return 0;
}

int main() {
	cout << CV_MAJOR_VERSION << endl;

	return GreenScreen().mainLoop();
}