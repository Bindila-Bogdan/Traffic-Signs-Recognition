#include "stdafx.h"
#include "ImagePreprocessing.h"

cv::Mat ImagePreprocessing::imageEnhancement(cv::Mat& image, bool contestMode) {
	cv::Mat lookUpTable(1, 256, CV_8U);
	uchar* p = lookUpTable.ptr();
	for (int i = 0; i < 256; ++i)
		p[i] = cv::saturate_cast<uchar>(pow(i / 255.0, gamma) * 255.0);
	cv::Mat res = cv::Mat(image.rows, image.cols, CV_8UC3);
	LUT(image, lookUpTable, res);
	cv::Mat resizedImage;
	resize(res, resizedImage, cv::Size(int(res.cols / 2), int(res.rows / 2)));
	return res;
}

cv::Mat ImagePreprocessing::autoCanny(cv::Mat& image) {
	cv::Mat imageCopy = cv::Mat(image.rows, image.cols, CV_8UC1);
	int thresholdValue = threshold(image, imageCopy, 0, 255, cv::THRESH_BINARY + cv::THRESH_OTSU);
	cv::Mat cannyImage = cv::Mat(image.rows, image.cols, CV_8UC1);
	Canny(image, cannyImage, int(thresholdValue / 2), thresholdValue);
	return cannyImage;
}

cv::Mat ImagePreprocessing::preprocessImage(cv::Mat& image, bool contestMode) {
	cv::Mat grayscale = cv::Mat(image.rows, image.cols, CV_8UC1);
	cvtColor(image, grayscale, cv::COLOR_BGR2GRAY);
	cv::Mat blurred = cv::Mat(image.rows, image.cols, CV_8UC1);
	GaussianBlur(grayscale, blurred, cv::Size(gaussianBlurSize, gaussianBlurSize), 0);
	cv::Mat edges = autoCanny(blurred);
	cv::Mat dilatedImage = cv::Mat(image.rows, image.cols, CV_8UC1);
	cv::Mat structureElem = cv::getStructuringElement(cv::MORPH_RECT, cv::Size(dilationSize, dilationSize));
	dilate(edges, dilatedImage, structureElem);
	return dilatedImage;
}

std::vector<cv::Mat> ImagePreprocessing::generateColorMasks(cv::Mat& image) {
	std::vector<cv::Mat> masks;
	cv::Mat hsv = cv::Mat(image.rows, image.cols, CV_8UC3);
	cv::cvtColor(image, hsv, cv::COLOR_BGR2HSV);
	cv::Mat structureElem = cv::getStructuringElement(cv::MORPH_RECT, cv::Size(dilationSizeMask, dilationSizeMask));

	cv::Mat maskForRedLower = cv::Mat(image.rows, image.cols, CV_8UC1);
	cv::inRange(hsv, cv::Scalar(0, 100, 100), cv::Scalar(10, 255, 255), maskForRedLower);
	dilate(maskForRedLower, maskForRedLower, structureElem);
	erode(maskForRedLower, maskForRedLower, structureElem);

	cv::Mat maskForRedUpper = cv::Mat(image.rows, image.cols, CV_8UC1);
	cv::inRange(hsv, cv::Scalar(161, 100, 100), cv::Scalar(180, 255, 255), maskForRedUpper);
	dilate(maskForRedUpper, maskForRedUpper, structureElem);
	erode(maskForRedUpper, maskForRedUpper, structureElem);

	cv::Mat maskForBlue = cv::Mat(image.rows, image.cols, CV_8UC1);
	cv::inRange(hsv, cv::Scalar(101, 39, 64), cv::Scalar(140, 255, 255), maskForBlue);
	dilate(maskForBlue, maskForBlue, structureElem);
	erode(maskForBlue, maskForBlue, structureElem);

	cv::Mat maskForYellow = cv::Mat(image.rows, image.cols, CV_8UC1);
	cv::inRange(hsv, cv::Scalar(21, 125, 150), cv::Scalar(33, 255, 255), maskForYellow);
	dilate(maskForYellow, maskForYellow, structureElem);
	erode(maskForYellow, maskForYellow, structureElem);

	masks.push_back(maskForRedLower);
	masks.push_back(maskForRedUpper);
	masks.push_back(maskForBlue);
	masks.push_back(maskForYellow);

	return masks;
}

