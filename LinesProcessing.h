#ifndef LINES_PROCESSING_H
#define INES_PROCESSING_H

#include <opencv2/opencv.hpp>
#include <opencv2/core/core.hpp>
#include <opencv2/imgproc/imgproc.hpp>

#define PI 3.14159265
#define MIN_INTERSECTION 0.5

class LinesProcessing {
	static const int offsetAngle = 15;
	static const int matrixSize = 5;
	static const int maxDistance = 50;

public:
	static std::vector<cv::Vec4i> sortLines(std::vector<cv::Vec4i> lines);
	static bool angleInInterval(cv::Vec4i line);
	static std::vector<cv::Vec4i> filterByAngle(std::vector<cv::Vec4i> lines);
	static cv::Vec4i betterLine(cv::Vec4i line1, cv::Vec4i line2);
	static std::map<int, cv::Vec4i> removeNeighbours(std::map<int, cv::Vec4i> lineDict, int currentColor, std::set<int> colors);
	static std::set<int> checkMatrix(int x, int y, int currentColor, cv::Mat& image);
	static std::vector<cv::Vec4i> supressLines(std::vector<cv::Vec4i> lines, cv::Mat& image);
	static bool sameLevel(cv::Vec4i line1, cv::Vec4i line2);
	static std::map<int, std::vector<cv::Vec4i>> groupLines(std::vector<cv::Vec4i> lines);
};

#endif 