#include "stdafx.h"
#include "BoundingBox.h";
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
		cv::imshow("Image", image);
		resize(originalImage, resizedOriginalImage, cv::Size(int(width / 2), int(height / 2)));
		cv::imshow("Original", originalImage);
	}
}

void AuxFunctions::classifyAndDraw(cv::Mat& originalImage, cv::Mat& finalImage, std::map<int, cv::Vec4i> contours, std::string imageNumber, bool dumpRois, std::string dumpFolder, bool contestMode) {
	cv::Mat copyOfOriginalImage = originalImage.clone();
	cv::Mat grayscaleCopy;
	cv::Mat currentRoi;
	cv::Mat currentRoiColored;
	cvtColor(copyOfOriginalImage, grayscaleCopy, cv::COLOR_BGR2GRAY);
	int contourNumber = 0, minX, minY, maxX, maxY;
	std::string prediction;
	std::string contourName;
	std::map<int, cv::Vec4i>::iterator i;
	int noOfEachClassified[4] = { 0,0,0,0 };
	std::vector<cv::Vec4i> stop, parking, pedestrian, priority;
	cv::Point p1, p2, p3;
	for (i = contours.begin(); i != contours.end(); ++i) {
		minX = i->second[0];
		minY = i->second[1];
		maxX = i->second[2];
		maxY = i->second[3];
		int width = maxX - minX;
		int height = maxY - minY;
		if(!contestMode)
			currentRoiColored = copyOfOriginalImage(cv::Rect(minX, minY, width, height));
		currentRoi = grayscaleCopy(cv::Rect(minX, minY, width, height));
		prediction = NeuralNetwork::getPredictions(currentRoi, contestMode);
		rectangle(finalImage, cv::Point(minX, minY), cv::Point(maxX, maxY), cv::Scalar(255, 182, 91), 2);
		putText(finalImage, prediction, cv::Point(minX, maxY + 28), cv::FONT_HERSHEY_SIMPLEX, 1, cv::Scalar(0, 165, 255), 2);
		contourNumber++;
		if (dumpRois && !contestMode) {
			contourName = imageNumber + "_" + prediction + "_" + std::to_string(contourNumber);
			imwrite(dumpFolder + contourName + ".jpg", currentRoiColored);
		}
		if (prediction != "Other") {
			if (prediction == "Stop" && (i->first % 10 == 1 || i->first <= -1)) {
				printf("Stop\n");
				stop.push_back(i->second);
				noOfEachClassified[0] ++;
			}
			else if (prediction == "Parking" && (i->first % 10 == 2 || i->first <= -1)) {
				printf("Parking\n");
				parking.push_back(i->second);
				noOfEachClassified[1] ++;
			}
			else if (prediction == "Pedestrian" && (i->first % 10 == 2 || i->first <= -1)) {
				printf("Pedestrian\n");
				pedestrian.push_back(i->second);
				noOfEachClassified[2] ++;
			}
			else if (prediction == "Priority" && (i->first % 10 == 3 || i->first <= -1)) {
				printf("Priority\n");
				priority.push_back(i->second);
				noOfEachClassified[3] ++;
			}
		}
	}
	std::map<int, cv::Vec4i> outputBoxes;
	std::map<int, cv::Vec4i>::iterator j;

	outputBoxes = determineBestBoxes(stop, 0, outputBoxes);
	outputBoxes = determineBestBoxes(parking, 1, outputBoxes);
	outputBoxes = determineBestBoxes(pedestrian, 2, outputBoxes);
	outputBoxes = determineBestBoxes(priority, 3, outputBoxes);

	for (j = outputBoxes.begin(); j != outputBoxes.end(); ++j) {
			minX = j->second[0];
			minY = j->second[1];
			maxX = j->second[2];
			maxY = j->second[3];
			if (j->first % 10 == 0)
				prediction = "Stop";
			else if (j->first % 10 == 1)
				prediction = "Parking";
			else if (j->first  % 10  == 2)
				prediction = "Pedestrian";
			else if (j->first % 10 == 3)
				prediction = "Priority";
			if (!contestMode) {
			rectangle(finalImage, cv::Point(minX, minY), cv::Point(maxX, maxY), cv::Scalar(212, 255, 127), 2);
			putText(finalImage, prediction, cv::Point(minX, maxY + 28), cv::FONT_HERSHEY_SIMPLEX, 1, cv::Scalar(128, 128, 0), 2);
			rectangle(originalImage, cv::Point(minX, minY), cv::Point(maxX, maxY), cv::Scalar(212, 255, 127), 2);
			putText(originalImage, prediction, cv::Point(minX, maxY + 28), cv::FONT_HERSHEY_SIMPLEX, 1, cv::Scalar(128, 128, 0), 2);
		}
	}
}


std::map<int, cv::Vec4i> AuxFunctions::determineBestBoxes(std::vector<cv::Vec4i> category, int indexOfCategory, std::map<int, cv::Vec4i> outputBoxes) {
	int index = indexOfCategory;
	if (category.size() == 1)
		outputBoxes.insert(std::pair<int, cv::Vec4i>(indexOfCategory, category[0]));
	else {
		bool intersect, insertContour;
		int currentContourArea;
		std::vector<cv::Vec4i> intersectedContours;
		std::map<int, cv::Vec4i>::iterator k;
		for (int i = 0; i < category.size(); i++) {
			intersectedContours.clear();
			intersectedContours.push_back(category[i]);
			for (int j = 0; j < category.size(); j++) {
				if (i != j) {
					intersect = BoundingBox::intersection(category[i][0], category[i][1], category[i][2], category[i][3], category[j][0], category[j][1], category[j][2], category[j][3]);
					if (intersect)
						intersectedContours.push_back(category[j]);
				}
			}
			if (intersectedContours.size() == 1) {
				outputBoxes.insert(std::pair<int, cv::Vec4i>(index, category[i]));
				index += 10;
			}
			else {
				insertContour = true;
				currentContourArea = (category[i][3] - category[i][1]) * (category[i][2] - category[i][0]);
				for (int k = 1; k < intersectedContours.size(); k++) {
					if (currentContourArea > (intersectedContours[k][3] - intersectedContours[k][1])* (intersectedContours[k][2] - intersectedContours[k][0])) {
						insertContour = false;
						break;
					}
				}
			}
			if (insertContour) {
				outputBoxes.insert(std::pair<int, cv::Vec4i>(index, category[i]));
				index += 10;
			}
		}
	}
	return outputBoxes;
}