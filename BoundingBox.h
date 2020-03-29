#ifndef BOUNDING_BOX_H
#define BOUNDING_BOX_H

#include <opencv2/opencv.hpp>
#include <opencv2/core/core.hpp>
#include <opencv2/imgproc/imgproc.hpp>

#define PI 3.14159265

class BoundingBox {
	static const int minArea = 2048;

public:
	static float computeOffset(int xa1, int ya1, int xa2, int ya2, int xb1, int yb1, int xb2, int yb2);
	static int* inside(int min_ax, int min_ay, int max_ax, int max_ay, int min_bx, int min_by, int max_bx, int max_by);
	static std::vector<cv::Vec4i> nonMaximaSupression(std::vector<cv::Vec4i> boxes);
	static cv::Vec4i signCrop(cv::Vec4i line1, cv::Vec4i line2, int width, int height);
	static std::vector<cv::Vec4i> filterBoundingBoxes(int height, int width, std::map<int, std::vector<cv::Vec4i>> groups);
};

#endif