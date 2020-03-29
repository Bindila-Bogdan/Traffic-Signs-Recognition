#ifndef SIGNS_RECOGNITION_H
#define SIGNS_RECOGNITION_H

#include <chrono>
#include <opencv2/opencv.hpp>
#include <opencv2/core/core.hpp>
#include <opencv2/imgproc/imgproc.hpp>

//POSIX SYSTEM
#include <dirent.h>
#include <sys/types.h>
     
//WINDOWS
#include <atlstr.h>
#include <windows.h>


const std::string DUMP_FOLDER = "//home//pi//Documents//RoadSignsRecognition//DumpFolder";
const CString FOLDER_NAME_WINDOWS = "..\\SignsRecognitionC++\\Images";
//const CString FOLDER_NAME_WINDOWS = "..\\SignsRecognitionC++\\Benchmark";
const CString FOLDER_NAME_LINUX = "//home//pi//Documents//RoadSignsRecognition//BenchmarkFolder";

class SignsRecognition{
	//BEFORE MAX_GAP VALUE WAS 10
	static const int minLength = 75;
	static const int maxGap = 3;
	static const bool right = false, dumpRois = false, dumpImages = false, contestMode = false;

public:
	static std::vector<cv::Mat> getRois(cv::Mat& originalImage, cv::Mat& image, std::string imageNumber);
	static void readFromCamera();
	static void readFromFolderWindows();
	static void readFromFolderLinux();
	static void processImage(cv::Mat& image, std::string file);
};

#endif