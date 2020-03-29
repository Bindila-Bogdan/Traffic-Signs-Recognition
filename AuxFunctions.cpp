#include "stdafx.h"
#include "AuxFunctions.h";
#include "NeuralNetwork.h";

void AuxFunctions::plotImage(cv::Mat& image, cv::Mat& originalImage, bool right) {
	int waitKeyValue = 1, width = image.cols, height = image.rows;
	if (right) {
		cv::imshow("Image", image);
		cv::imshow("Original", originalImage);
	}
	else {
		cv::Mat resizedImage, resizedOriginalImage;
		resize(image, resizedImage, cv::Size(int(width / 2), int(height / 2)));
		cv::imshow("Image", resizedImage);
		resize(originalImage, resizedOriginalImage, cv::Size(int(width / 2), int(height / 2)));
		cv::imshow("Original", resizedOriginalImage);
	}
}

void AuxFunctions::classifyAndDraw(cv::Mat& originalImage, cv::Mat& finalImage, std::vector<cv::Vec4i> contours, std::string imageNumber, bool dumpRois, std::string dumpFolder, bool contestMode) {
	cv::Mat copyOfOriginalImage = originalImage.clone();
	cv::Mat grayscaleCopy;
	cv::Mat currentRoi;
	cvtColor(copyOfOriginalImage, grayscaleCopy, cv::COLOR_BGR2GRAY);
	int contourNumber = 0, minX, minY, maxX, maxY;
	std::string prediction;
	std::string contourName;
	cv::Point p1, p2, p3;
	for (int i = 0; i < contours.size(); i++) {
		minX = contours[i][0];
		minY = contours[i][1];
		maxX = contours[i][2];
		maxY = contours[i][3];
		int width = maxX - minX;
		int height = maxY - minY;
		currentRoi = grayscaleCopy(cv::Rect(minX, minY, width, height));
		prediction = NeuralNetwork::getPredictions(currentRoi, contestMode);
		if (prediction != "Other") {
			std::cout << prediction << std::endl;
			if (!contestMode) {
				contourNumber++;
				if (dumpRois) {
					contourName = imageNumber + "_" + std::to_string(contourNumber);
					imwrite(dumpFolder + contourName + ".jpg", currentRoi);
				}
				rectangle(finalImage, cv::Point(minX, minY), cv::Point(maxX, maxY), cv::Scalar(89, 182, 91), 5);
				// FONT SIZE WAS 4
				putText(finalImage, prediction, cv::Point(minX, maxY + 28), cv::FONT_HERSHEY_SIMPLEX, 1, cv::Scalar(0, 165, 255), 2);
				rectangle(originalImage, cv::Point(minX, minY), cv::Point(maxX, maxY), cv::Scalar(89, 182, 91), 5);
				putText(originalImage, prediction, cv::Point(minX, maxY + 28), cv::FONT_HERSHEY_SIMPLEX, 1, cv::Scalar(0, 165, 255), 2);
			}
		}
	}
}
