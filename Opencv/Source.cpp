#include "Source.h"

GreenScreen::GreenScreen() {
	cap = cv::VideoCapture(0);

	if (initWindow() == -1) {
		exit(-1);
	}
	image = cv::imread("C:/Users/Stefan/Downloads/image.jpg", 1);
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
	cv::Mat binaryImage;
	cv::Mat inputFrameCopy;
	

	inputFrame.copyTo(inputFrameCopy);

	cv::medianBlur(inputFrameCopy, inputFrameCopy, 3);
	cv::cvtColor(inputFrameCopy, hsvImage, cv::COLOR_BGR2HSV);
	cv::inRange(hsvImage, cv::Scalar(86, 100, 60), cv::Scalar(120, 255, 255), binaryImage);


	vector<vector<cv::Point>> points;
	cv::findContours(binaryImage, points, CV_RETR_EXTERNAL, CV_CHAIN_APPROX_SIMPLE);

	double area = 0, tmpArea = 0;
	int index = 0;
	for (int i = 0; i < points.size(); i++) {
		tmpArea = cv::contourArea(points.at(i));

		if (tmpArea > area) {
			area = tmpArea;
			index = i;
		}
	}

	cv::Mat drawing = cv::Mat::zeros(binaryImage.size(), CV_8UC3);

	vector<cv::Point> approx, hull;

	if (points.size() > 0 && points.at(index).size() > 0) {
		cv::approxPolyDP(cv::Mat(points.at(index)), approx, 20, true);

		cv::convexHull(approx, hull, true);
		cv::polylines(drawing, hull, true, cv::Scalar(0, 0, 255));

		vector<cv::Point> untransformedPoints;
		untransformedPoints.push_back(cv::Point());
		untransformedPoints.push_back(cv::Point(1372, 0));
		untransformedPoints.push_back(cv::Point(1372, 911));
		untransformedPoints.push_back(cv::Point(0, 911));

		cv::Point2f inputQuad[4];
		cv::Point2f outputQuad[4];

		inputQuad[0] = cv::Point2f(0, 0);
		inputQuad[1] = cv::Point2f(1372, 0);
		inputQuad[2] = cv::Point2f(1372, 911);
		inputQuad[3] = cv::Point2f(0, 911);

		if (hull.size() == 4) {
			vector<vector<cv::Point>> ps;
			ps.push_back(hull);

			cv::fillPoly(inputFrame, ps, cv::Scalar(0, 0, 0));

			for (int i = 0; i < hull.size(); i++) {
				outputQuad[i] = cv::Point2f(hull.at(i).x, hull.at(i).y);
			}

			cv::Mat transform(2, 4, CV_32FC1);
			
			transform = cv::Mat::zeros(image.rows, image.cols, image.type());

			transform = cv::getPerspectiveTransform(inputQuad, outputQuad);
			cv::warpPerspective(image, drawing, transform, drawing.size());
		}
	}
	

	//cv::Canny(binaryImage, grayImage, 100, 200, 3);














	/*vector<cv::Vec4i> lines;
	cv::HoughLinesP(grayImage, lines, 1, CV_PI / 180, 10, 30, 10);

	//vector<vector<cv::Point>> contours;
	//vector<cv::Vec4i> hierarchy;
	//cv::findContours(grayImage, contours, hierarchy, CV_RETR_TREE, CV_CHAIN_APPROX_SIMPLE, cv::Point(0, 0));

	cv::Mat drawing = cv::Mat::zeros(binaryImage.size(), CV_8UC3);

	const vector<Function> functions = createFunctions(lines);
	const vector<Intersection> intersections = calculateIntersections(functions);
	vector<int> sortedIndices;
	cout << "Punkte: " << intersections.size() << " Linien: " << lines.size() << endl;

	//if (intersections.size() >= 4) {
		//sortedIndices = sortPointsClockwise(intersections);
	//}





	for (int i = 0; i < intersections.size(); i++) {
		cv::circle(drawing, cv::Point(intersections.at(i).point.x, intersections.at(i).point.y), 5, cv::Scalar(0, 0, 255), 3);
	}

	for (int i = 0; i < lines.size(); i++) {
		cv::Vec4i line = lines.at(i);
		cv::line(drawing, cv::Point(line[0], line[1]), cv::Point(line[2], line[3]), cv::Scalar(255, 255, 255), 3, CV_AA);
	}*/

	/*int max = 0;
	int tmpIndex = 0;
	vector<int> indices;

	for (int k = 0; k < 3; k++) {
		for (int i = 0; i < contours.size(); i++) {
			if (contours.at(i).size() > max) {
				if (contains(indices, i))
					continue;

				max = contours.at(i).size();
				tmpIndex = i;
			}
		}

		indices.push_back(tmpIndex);
		max = 0;
	}*/


	/*vector<cv::Point> approx;
	double epsilon = 0.1 * cv::arcLength(contours.at(index), true);
	cv::approxPolyDP(contours.at(index), approx, epsilon, true);*/

	/*vector<cv::Point> points;
	vector<cv::Point> convexHullPoints;
	if (contours.size() > 0) {
		for (int j = 0; j < indices.size(); j++) {
			for (int i = 0; i < contours.at(indices.at(j)).size(); i++) {
				points.push_back(contours.at(indices.at(j)).at(i));
			}
		}

		cv::convexHull(points, convexHullPoints);
		cv::polylines(drawing, convexHullPoints, true, cv::Scalar(255, 255, 255), 2);
	}*/


	//outputFrame = inputFrame + drawing;
	cv::addWeighted(inputFrame, 1, drawing, 1, 1, outputFrame);
	//outputFrame = drawing;

	return 0;
}

const vector<Function> GreenScreen::createFunctions(const vector<cv::Vec4i>& lines) {
	vector<Function> functions;

	for (int i = 0; i < lines.size(); i++) {
		cv::Point p1 = cv::Point(lines.at(i)[0], lines.at(i)[1]);
		cv::Point p2 = cv::Point(lines.at(i)[2], lines.at(i)[3]);
		Function fi = makeFunction(p1, p2);

		if (!contains(functions, fi))
			functions.push_back(fi);
	}

	return functions;
}

const vector<Intersection> GreenScreen::calculateIntersections(const vector<Function>& functions) {
	vector<Intersection> intersections;

	for (int i = 0; i < functions.size(); i++) {
		for (int j = i; j < functions.size(); j++) {
			if (i == j)
				continue;

			float arg = intersect(functions.at(i), functions.at(j));

			if (arg > IMAGE_WIDTH || arg < 0 || functions.at(i).get(arg) > IMAGE_HEIGHT || functions.at(i).get(arg) < 0)
				continue;

			intersections.push_back(Intersection(Point(arg, functions.at(i).get(arg)), functions.at(i), functions.at(j)));
		}
	}

	return intersections;
}

const vector<int> GreenScreen::sortPointsClockwise(const vector<Intersection>& intersections) {
	vector<int> clockwiseSortedIndices;

	const vector<int> indexMin = getPointIndicesWithOrientation(intersections, Orientation::X_ORIENTATION);
	const Intersection leftTop = intersections.at(indexMin.at(0));
	
	int rightTopIndex, rightBottomIndex, leftBottomIndex;

	clockwiseSortedIndices.push_back(indexMin.at(0));

	vector<int> foundIndices;
	foundIndices.push_back(indexMin.at(0));

	for (int i = 0; i < intersections.size(); i++) {
		if (i == indexMin.at(0)) continue;

		if (leftTop.f1.check(intersections.at(i).point)) {
			if (leftTop.f1.m < leftTop.f2.m) {
				rightTopIndex = i;
			} else {
				leftBottomIndex = i;
			}
			foundIndices.push_back(i);
		}
		if (leftTop.f2.check(intersections.at(i).point)) {
			if (leftTop.f1.m < leftTop.f2.m) {
				leftBottomIndex = i;
			} else {
				rightTopIndex = i;
			}
			foundIndices.push_back(i);
		}
	}

	clockwiseSortedIndices.push_back(rightTopIndex);

	for (int i = 0; i < intersections.size(); i++) {
		if (i == leftBottomIndex || i == indexMin.at(0) || i == rightTopIndex) continue;

		clockwiseSortedIndices.push_back(i);
	}

	clockwiseSortedIndices.push_back(leftBottomIndex);
	// Verbleibender Punkt ist immer rightBottom
	return clockwiseSortedIndices;
}

const vector<int> GreenScreen::getPointIndicesWithOrientation(const vector<Intersection>& intersections, const Orientation orientation) {
	int min = IMAGE_WIDTH;
	int tmpIndex = 0;
	vector<int> indices;

	for (int k = 0; k < 1; k++) {
		for (int i = 0; i < intersections.size(); i++) {
			int tmpValue = 0;
			if (orientation == Orientation::X_ORIENTATION)
				tmpValue = intersections.at(i).point.x;
			else
				tmpValue = intersections.at(i).point.y;

			if (tmpValue < min) {
				if (containsIndex(indices, i))
					continue;

				min = intersections.at(i).point.x;
				tmpIndex = i;
			}
		}

		indices.push_back(tmpIndex);
		min = IMAGE_WIDTH;
	}

	return indices;
}

int GreenScreen::compX(cv::Point& p1, cv::Point& p2) {
	int diff = p1.x - p2.x;

	if (diff == 0) {
		return 0;
	}

	return diff < 0 ? -1 : 1;
}

int GreenScreen::compY(cv::Point& p1, cv::Point& p2) {
	int diff = p1.y - p2.y;

	if (diff == 0) {
		return 0;
	}

	return diff < 0 ? -1 : 1;
}

const Function& GreenScreen::makeFunction(const cv::Point& p1, const cv::Point& p2) {
	float nenner = 0;
	if (p1.x - p2.x == 0)
		nenner = numeric_limits<float>::infinity();
	else
		nenner = p1.x - p2.x;

	float m = (float) (p1.y - p2.y) / nenner;
	float b = (float) (p1.y) - (float) (m * p1.x);

	return Function(m, b);
}

float GreenScreen::intersect(const Function& func1, const Function& func2) {
	if (abs(func1.m - func2.m) < 1)
		return nanf("");

	return (func2.b - func1.b) / (func1.m - func2.m);
}

bool GreenScreen::contains(const vector<Function>& vec, const Function& element) {
	for each (Function func in vec) {
		if (func.equals(element))
			return true;
	}

	return false;
}

bool GreenScreen::containsIndex(const vector<int>& vec, const int element) {
	for each (int index in vec) {
		if (index == element)
			return true;
	}

	return false;
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