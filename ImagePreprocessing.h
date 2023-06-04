#ifndef IMAGE_PREPROCESSING_H
#define IMAGE_PREPROCESSING_H

#include <opencv2/opencv.hpp>
#include <opencv2/core/core.hpp>
#include <opencv2/imgproc/imgproc.hpp>

#define gamma 0.67

class ImagePreprocessing
{
	static const int dilationSize = 2;
	static const int dilationSizeMask = 3;
	static const int gaussianBlurSize = 5;

public:
	static cv::Mat imageEnhancement(cv::Mat &image, bool contestMode);
	static cv::Mat autoCanny(cv::Mat &image);
	static cv::Mat preprocessImage(cv::Mat &image, bool contestMode);
	static std::vector<cv::Mat> generateColorMasks(cv::Mat &image);
};

#endif