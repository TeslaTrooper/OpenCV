#ifndef SOURCE_HEADER
#define SOURCE_HEADER

#include <iostream>
#include <opencv2\opencv.hpp>

using namespace std;

struct Point {
	float x, y;

	Point() {
		x = 0;
		y = 0;
	}

	Point(float x, float y) {
		this->x = x;
		this->y = y;
	}
};

struct Function {
	float m, b;

	Function() {
		m = 0;
		b = 0;
	}

	Function(float m, float b) {
		this->m = m;
		this->b = b;
	}

	float get(float x) const {
		return m*x + b;
	}

	bool equals(const Function& func) const {
		float mDiff = abs(this->m - func.m);
		float bDiff = abs(this->b - func.b);

		return bDiff < 1 || mDiff < 2;
	}

	bool check(const Point& p) const {
		return get(p.x) == p.y;
	}
};

struct Intersection {
	Point point;
	Function f1, f2;

	Intersection() {}

	Intersection(Point point, Function f1, Function f2) {
		this->point = point;
		this->f1 = f1;
		this->f2 = f2;
	}
};

enum Orientation {
	Y_ORIENTATION, X_ORIENTATION
};

class GreenScreen {
	const int IMAGE_WIDTH = 640;
	const int IMAGE_HEIGHT = 480;
	const int WAIT_IN_MS = 20;

	cv::Mat image; 

	cv::VideoCapture cap;

	int initWindow();
	int processFrame();
	int greenScreen(const cv::Mat& inputFrame, cv::Mat& outputFrame);
	bool contains(const vector<Function>& vec, const Function& element);
	bool containsIndex(const vector<int>& vec, const int element);

	int compX(cv::Point& p1, cv::Point& p2);
	int compY(cv::Point& p1, cv::Point& p2);

	const vector<int> getPointIndicesWithOrientation(const vector<Intersection>& intersections, const Orientation orientation);

	const vector<Function> createFunctions(const vector<cv::Vec4i>& lines);
	const vector<Intersection> calculateIntersections(const vector<Function>& functions);
public:
	GreenScreen();

	int mainLoop();
	const Function& makeFunction(const cv::Point& p1, const cv::Point& p2);
	float intersect(const Function& func1, const Function& func2);
	const vector<int> sortPointsClockwise(const vector<Intersection>& intersections);
};

#endif SOURCE_HEADER