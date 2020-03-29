#include "stdafx.h"
#include "ImagePreprocessing.h"

cv::Mat ImagePreprocessing::autoCanny(cv::Mat& image) {
	cv::Mat imageCopy = cv::Mat(image.rows, image.cols, CV_8UC1);
	int thresholdValue = threshold(image, imageCopy, 0, 255, cv::THRESH_BINARY + cv::THRESH_OTSU);
	cv::Mat cannyImage = cv::Mat(image.rows, image.cols, CV_8UC1);
	Canny(image, cannyImage, int(thresholdValue / 2), thresholdValue);
	return cannyImage;
}

cv::Mat ImagePreprocessing::preprocessImage(cv::Mat& image, bool contestMode) {
	cv::Mat lookUpTable(1, 256, CV_8U);
	uchar* p = lookUpTable.ptr();
	for (int i = 0; i < 256; ++i)
		p[i] = cv::saturate_cast<uchar>(pow(i / 255.0, gamma) * 255.0);
	cv::Mat res = cv::Mat(image.rows, image.cols, CV_8UC3);
	LUT(image, lookUpTable, res);
	cv::Mat resizedImage;
	resize(res, resizedImage, cv::Size(int(res.cols / 2), int(res.rows / 2)));
	if(!contestMode)
		imshow("Enhanced Image", resizedImage);
	cv::Mat grayscale = cv::Mat(res.rows, res.cols, CV_8UC1);
	cvtColor(res, grayscale, cv::COLOR_BGR2GRAY);
	cv::Mat blurred = cv::Mat(image.rows, image.cols, CV_8UC1);
	GaussianBlur(grayscale, blurred, cv::Size(gaussianBlurSize, gaussianBlurSize), 0);
	cv::Mat edges = autoCanny(blurred);
	cv::Mat dilatedImage = cv::Mat(image.rows, image.cols, CV_8UC1);
	//COULD BE CONSTANT STRUCTURING ELEMENT
	cv::Mat structureElem = cv::getStructuringElement(cv::MORPH_RECT, cv::Size(dilationSize, dilationSize));
	dilate(edges, dilatedImage, structureElem);
	return dilatedImage;
}

