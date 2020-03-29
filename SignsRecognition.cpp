#include "stdafx.h"
#include "BoundingBox.h";
#include "AuxFunctions.h";
#include "NeuralNetwork.h";
#include "LinesProcessing.h";
#include "SignsRecognition.h";
#include "ImagePreprocessing.h";

std::vector<cv::Mat> SignsRecognition::getRois(cv::Mat& originalImage, cv::Mat& image, std::string imageNumber) {
	std::vector<cv::Vec4i> linesP;
	std::vector<cv::Mat> images;
	images.push_back(originalImage);
	images.push_back(image);
	HoughLinesP(image, linesP, 1, CV_PI / 180, 50, minLength, maxGap);
	std::vector<cv::Vec4i> filteredLines = LinesProcessing::filterByAngle(linesP);
	if (filteredLines.size() != 0) {
		std::vector<cv::Vec4i> candidateLines = LinesProcessing::supressLines(filteredLines, image);
		std::map<int, std::vector<cv::Vec4i>> groups;
		std::map<int, std::vector<cv::Vec4i>>::iterator itr;
		if (candidateLines.size() != 0) {
			groups = LinesProcessing::groupLines(candidateLines);
		}
		if (groups.size() != 0) {
			cv::Mat finalImage = cv::Mat(image.rows, image.cols, CV_8UC3);
			if (!contestMode) {
				cvtColor(image, finalImage, cv::COLOR_GRAY2BGR);
				int color1, color2, color3;
				for (itr = groups.begin(); itr != groups.end(); ++itr) {
					for (int i = 0; i < 2; i++) {
						color1 = rand() % 256;
						color2 = rand() % 256;
						color3 = rand() % 256;
						// LINE WIDTH WAS 5
						cv::line(finalImage, cv::Point(itr->second[i][0], itr->second[i][1]), cv::Point(itr->second[i][2], itr->second[i][3]), cv::Scalar(color1, color2, color3), 3);
					}
				}
			}
			std::vector<cv::Vec4i> contours = BoundingBox::filterBoundingBoxes(image.rows, image.cols, groups);
			AuxFunctions::classifyAndDraw(originalImage, finalImage, contours, imageNumber, dumpRois, DUMP_FOLDER, contestMode);
			images.clear();
			images.push_back(originalImage);
			images.push_back(finalImage);
			return images;
		}
		return images;
	}
	return images;
}

void SignsRecognition::readFromCamera() {
	
}

void SignsRecognition::readFromFolderWindows() {
	cv::waitKey(1);
	float totalTime = 0.0f, duration;
	cv::Mat image;
	int frameNumber = 0;
	std::string format = ".jpg";
	HANDLE dir;
	WIN32_FIND_DATA fileData;
	CString  fileName, fullFileName;
	if ((dir = FindFirstFile((FOLDER_NAME_WINDOWS + "/*"), &fileData)) == INVALID_HANDLE_VALUE)
	{
		std::cout << "Invalid directory" << std::endl;
		return;
	}
	while (FindNextFile(dir, &fileData)) {
		auto t1 = std::chrono::high_resolution_clock::now();
		fileName = fileData.cFileName;
		fullFileName = FOLDER_NAME_WINDOWS + "\\" + fileName;
		if (strcmp(fileData.cFileName, ".") != 0 && strcmp(fileData.cFileName, "..") != 0)
		{
			std::string fileName = fullFileName.GetString();
			if (fileName.find(format) != -1) {
				frameNumber++;
				if(!contestMode)
					std::cout << fileName << std::endl;
				image = cv::imread(fileName);
				processImage(image, fileName);
				if (!contestMode)
					cv::waitKey(0);
				auto t2 = std::chrono::high_resolution_clock::now();
				duration = std::chrono::duration_cast<std::chrono::microseconds>(t2 - t1).count();
				totalTime += duration;
			}
		}
	}
	float fps = 1.0f / ((totalTime / 1000000.0f) / frameNumber);
	std::cout << "FPS: " << fps << std::endl;

}

void SignsRecognition::readFromFolderLinux() {
	cv::waitKey(1);
	cv::Mat image;
	float totalTime = 0.0f, duration;
	int frameNumber = 0;
	std::string format = ".jpg";
	struct dirent* entry;
	DIR* dir = opendir(FOLDER_NAME_LINUX);
	std::string fileName;
	if (dir == NULL) {
		std::cout << "Invalid directory" << std::endl;
		return;
	}
	while ((entry = readdir(dir)) != NULL) {
		auto t1 = std::chrono::high_resolution_clock::now();
		fileName = FOLDER_NAME_LINUX + "//";
		fileName += entry->d_name;
		if (fileName.find(format) != -1) {
			frameNumber++;
			if (!contestMode)
				std::cout << fileName << std::endl;
			image = cv::imread(fileName);
			processImage(image, fileName);
			if (!contestMode)
				cv::waitKey(0);
			auto t2 = std::chrono::high_resolution_clock::now();
			duration = std::chrono::duration_cast<std::chrono::microseconds>(t2 - t1).count();
			totalTime += duration;
		}
	}
	float fps = 1.0f / ((totalTime / 1000000.0f) / frameNumber);
	std::cout << "FPS: " << fps << std::endl;
	closedir(dir);
}

void SignsRecognition::processImage(cv::Mat& image, std::string file) {
	cv::Mat originalImage = image.clone();
	std::string imageName = file.substr(file.find_last_of("/\\") + 1);
	if (right) {
		cv::Vec4i coords;
		coords[0] = int(image.cols / 2);
		coords[1] = 0;
		coords[2] = image.cols - 1;
		coords[3] = image.rows - 1;
		int width = coords[2] - coords[0];
		int height = coords[3] - coords[1];
		cv::Mat imageRight = originalImage(cv::Rect(coords[0], coords[1], width, height));
		cv::Mat processedImage = ImagePreprocessing::preprocessImage(imageRight,contestMode);
		cv::Mat originalImageRight = originalImage(cv::Rect(coords[0], coords[1], width, height));
		std::vector<cv::Mat> images;
		images = SignsRecognition::getRois(originalImageRight, processedImage, imageName);
		if(!contestMode)
			AuxFunctions::plotImage(images[0], images[1], right);
	}
	else {
		cv::Mat processedImage = ImagePreprocessing::preprocessImage(image, contestMode);
		std::vector<cv::Mat> images;
		images = SignsRecognition::getRois(originalImage, processedImage, imageName);
		if(!contestMode)
			AuxFunctions::plotImage(images[0], images[1], right);
	}
}

void main() {
	SignsRecognition::readFromFolderWindows();
}