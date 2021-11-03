#include "stdafx.h"
#include "BoundingBox.h";
#include "AuxFunctions.h";
#include "NeuralNetwork.h";
#include "LinesProcessing.h";
#include "SignsRecognition.h";
#include "ImagePreprocessing.h";

std::vector<cv::Mat> SignsRecognition::getRois(cv::Mat& originalImage, cv::Mat& image, std::vector<cv::Mat> masks, std::string imageNumber, bool right) {
	std::map<int, cv::Vec4i> colorBasedContours = BoundingBox::determineColorBasedBoxes(masks, originalImage, right);
	cv::Mat finalImage = cv::Mat(image.rows, image.cols, CV_8UC3);
	cvtColor(image, finalImage, cv::COLOR_GRAY2BGR);
	std::vector<cv::Vec4i> linesP;
	std::vector<cv::Mat> images;
	cv::Mat groupedLinesImage;
	if (!contestMode)
		groupedLinesImage = finalImage.clone();
	images.push_back(originalImage);
	images.push_back(finalImage);
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
			if (!contestMode) {
				int color1, color2, color3;
				for (itr = groups.begin(); itr != groups.end(); ++itr) {
					color1 = rand() % 256;
					color2 = rand() % 256;
					color3 = rand() % 256;
					for (int i = 0; i < 2; i++) {
						cv::line(finalImage, cv::Point(itr->second[i][0], itr->second[i][1]), cv::Point(itr->second[i][2], itr->second[i][3]), cv::Scalar(color1, color2, color3), 2);
						cv::line(groupedLinesImage, cv::Point(itr->second[i][0], itr->second[i][1]), cv::Point(itr->second[i][2], itr->second[i][3]), cv::Scalar(color1, color2, color3), 2);
					}
				}
			}
			if (!contestMode)
				cv::imshow("Grouped lines", groupedLinesImage);
			std::map<int, cv::Vec4i> contours = BoundingBox::filterBoundingBoxes(image.rows, image.cols, groups);
			colorBasedContours.insert(contours.begin(), contours.end());
			AuxFunctions::classifyAndDraw(originalImage, finalImage, colorBasedContours, imageNumber, dumpRois, DUMP_FOLDER, contestMode);
			images.clear();
			images.push_back(originalImage);
			images.push_back(finalImage);
			return images;
		}
		AuxFunctions::classifyAndDraw(originalImage, finalImage, colorBasedContours, imageNumber, dumpRois, DUMP_FOLDER, contestMode);
		return images;
	}
	AuxFunctions::classifyAndDraw(originalImage, finalImage, colorBasedContours, imageNumber, dumpRois, DUMP_FOLDER, contestMode);
	return images;
}

void SignsRecognition::readVideoFromFolderWindows() {
	float totalTime = 0.0f, duration;
	cv::Mat image;
	std::string format = ".jpg";
	int frameNumber = 0;
	HANDLE dir;
	WIN32_FIND_DATA fileData;
	std::string filename;
	std::cout << FOLDER_NAME_WINDOWS_VIDEO << std::endl;
	cv::VideoCapture cap(FOLDER_NAME_WINDOWS_VIDEO);
	if (!cap.isOpened()) {
		std::cout << "Error opening video stream or file" << std::endl;
		return;
	}
	while (true) {
		cap >> image;
		if (image.empty())
			break;
		auto t1 = std::chrono::high_resolution_clock::now();
		frameNumber++;
		filename = std::to_string(frameNumber) + format;
		if (!contestMode)
			std::cout << filename << std::endl;
		processImage(image, filename);
		if (!measureFPS)
			cv::waitKey(0);
		else
			cv::waitKey(1);
		if (contestMode) {
			auto t2 = std::chrono::high_resolution_clock::now();
			duration = std::chrono::duration_cast<std::chrono::microseconds>(t2 - t1).count();
			totalTime += duration;
		}

	}
	cap.release();	
	if (contestMode && measureFPS) {
		float fps = 1.0f / ((totalTime / 1000000.0f) / frameNumber);
		std::cout << "FPS: " << fps << std::endl;
	}
}

void SignsRecognition::readFromFolderWindows() {
	float totalTime = 0.0f, duration;
	cv::Mat image;
	int frameNumber = 0;
	std::string format = ".jpg";
	HANDLE dir;
	WIN32_FIND_DATA fileData;
	CString  fileName, fullFileName;
	if ((dir = FindFirstFile((FOLDER_NAME_WINDOWS_IMAGES + "/*"), &fileData)) == INVALID_HANDLE_VALUE)
	{
		std::cout << "Invalid directory" << std::endl;
		return;
	}
	while (FindNextFile(dir, &fileData)) {
		auto t1 = std::chrono::high_resolution_clock::now();
		fileName = fileData.cFileName;
		fullFileName = FOLDER_NAME_WINDOWS_IMAGES + "//" + fileName;
		if (strcmp(fileData.cFileName, ".") != 0 && strcmp(fileData.cFileName, "..") != 0)
		{
			std::string fileName = fullFileName.GetString();
			if (fileName.find(format) != -1) {
				frameNumber++;
				if(!contestMode)
					std::cout << fileName << std::endl;
				image = cv::imread(fileName);
				processImage(image, fileName);
				if (!measureFPS)
					cv::waitKey(0);
				else
					cv::waitKey(1);
				auto t2 = std::chrono::high_resolution_clock::now();
				duration = std::chrono::duration_cast<std::chrono::microseconds>(t2 - t1).count();
				totalTime += duration;
			}
		}
	}
	if (contestMode && measureFPS) {
		float fps = 1.0f / ((totalTime / 1000000.0f) / frameNumber);
		std::cout << "FPS: " << fps << std::endl;
	}
}

void SignsRecognition::processImage(cv::Mat& image, std::string file) {
	cv::Mat resizedImage;
	cv::resize(image, resizedImage, cv::Size(640, 480));
	cv::Mat originalImage = resizedImage.clone();
	std::string imageName = file.substr(file.find_last_of("//") + 1);
	imageName = imageName.substr(0, imageName.size() - 4);
	if (right) {
		cv::Vec4i coords;
		coords[0] = int(originalImage.cols / 2);
		coords[1] = 0;
		coords[2] = originalImage.cols - 1;
		coords[3] = originalImage.rows - 1;
		int width = coords[2] - coords[0];
		int height = coords[3] - coords[1];
		cv::Mat imageRight = originalImage(cv::Rect(coords[0], coords[1], width, height));
		cv::Mat enhancedImageRight = ImagePreprocessing::imageEnhancement(imageRight, contestMode);
		std::vector<cv::Mat> masks = ImagePreprocessing::generateColorMasks(enhancedImageRight);
		cv::Mat processedImage = ImagePreprocessing::preprocessImage(enhancedImageRight, contestMode);
		std::vector<cv::Mat> images;
		images = SignsRecognition::getRois(enhancedImageRight, processedImage, masks, imageName, right);
		AuxFunctions::plotImage(images[1], images[0], right);
	}
	else {
		cv::Mat enhancedImage = ImagePreprocessing::imageEnhancement(originalImage, contestMode);
		std::vector<cv::Mat> masks = ImagePreprocessing::generateColorMasks(enhancedImage);
		cv::Mat processedImage = ImagePreprocessing::preprocessImage(enhancedImage, contestMode);
		std::vector<cv::Mat> images;
		images = SignsRecognition::getRois(enhancedImage, processedImage, masks,imageName, right);
		AuxFunctions::plotImage(images[1], images[0], right);
	}
}

void main() {
	SignsRecognition::readFromFolderWindows();
	// SignsRecognition::readVideoFromFolderWindows();
}