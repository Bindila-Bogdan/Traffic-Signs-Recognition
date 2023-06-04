#ifndef SIGNS_RECOGNITION_H
#define SIGNS_RECOGNITION_H

#include <chrono>
#include <atlstr.h>
#include <windows.h>
#include <opencv2/opencv.hpp>
#include <opencv2/core/core.hpp>
#include <opencv2/imgproc/imgproc.hpp>

const std::string DUMP_FOLDER = ".//data//dumps//";
const CString FOLDER_NAME_WINDOWS_IMAGES = ".//data//images";
const std::string FOLDER_NAME_WINDOWS_VIDEO = ".//data//videos//track_3.avi";

class SignsRecognition
{
	static const int minLength = 50;
	static const int maxGap = 5;
	static const bool right = false, dumpRois = false, dumpImages = false, contestMode = false, measureFPS = false;

public:
	static std::vector<cv::Mat> getRois(cv::Mat &originalImage, cv::Mat &image, std::vector<cv::Mat> masks, std::string imageNumber, bool right);
	static void readVideoFromFolderWindows();
	static void readFromFolderWindows();
	static void processImage(cv::Mat &image, std::string file);
};

#endif