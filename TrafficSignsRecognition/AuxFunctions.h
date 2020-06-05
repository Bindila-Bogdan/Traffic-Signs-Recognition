#ifndef AUX_FUNCTIONS_H
#define AUX_FUNCTIONS_H

#include <opencv2/opencv.hpp>
#include <opencv2/core/core.hpp>
#include <opencv2/imgproc/imgproc.hpp>

class AuxFunctions{
public:
	static void classifyAndDraw(cv::Mat& originalImage, cv::Mat& finalImage, std::map<int, cv::Vec4i> contours, std::string imageNumber, bool dumpRois, std::string dumpFolder, bool constestMode);
	static void plotImage(cv::Mat& image, cv::Mat& orignalImage, bool right);
	static std::map<int, cv::Vec4i> determineBestBoxes(std::vector<cv::Vec4i> category, int indexOfCategory, std::map<int, cv::Vec4i> outputBoxes);
};

#endif