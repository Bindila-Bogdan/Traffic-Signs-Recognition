#include "stdafx.h"
#include "BoundingBox.h"
#include <math.h>

float BoundingBox::computeOffset(int xa1, int ya1, int xa2, int ya2, int xb1, int yb1, int xb2, int yb2)
{
	float degreeAngleOne = 90.0f, degreeAngleTwo = 90.0f, degreeAngle, m;
	if (abs(xa2 - xa1) != 0)
	{
		m = (ya2 - ya1) / float(xa2 - xa1);
		degreeAngleOne = atan(m) * 180 / PI;
	}
	if (abs(xb2 - xb1) != 0)
	{
		m = (yb2 - yb1) / float(xb2 - xb1);
		degreeAngleTwo = atan(m) * 180 / PI;
	}
	if (degreeAngleOne < 0 && degreeAngleTwo < 0)
	{
		degreeAngle = degreeAngleOne < degreeAngleTwo ? degreeAngleOne : degreeAngleTwo;
	}
	else
	{
		degreeAngle = degreeAngleOne > degreeAngleTwo ? degreeAngleOne : degreeAngleTwo;
	}
	if (degreeAngle < 0)
		return (90.0f + degreeAngle);
	else
		return (-90.0f + degreeAngle);
}

bool BoundingBox::intersection(int min_ax, int min_ay, int max_ax, int max_ay, int min_bx, int min_by, int max_bx, int max_by)
{
	cv::Vec4i smallerBoundingBox = cv::Vec4i(0, 0, 0, 0);
	if (((((min_bx <= min_ax && min_ax <= max_bx) || (min_bx <= max_ax && max_ax <= max_bx)) &&
		  ((min_by <= min_ay && min_ay <= max_by) || (min_by <= max_ay && max_ay <= max_by)))) ||
		((((min_ax <= min_bx && min_bx <= max_ax) || (min_ax <= max_bx && max_bx <= max_ax)) &&
		  ((min_ay <= min_by && min_by <= max_ay) || (min_ay <= max_by && max_by <= max_ay)))))
		return true;
	else
		return false;
}

int *BoundingBox::inside(int min_ax, int min_ay, int max_ax, int max_ay, int min_bx, int min_by, int max_bx, int max_by)
{
	int bigBoundingBox[4];
	if ((min_bx <= min_ax && min_ax <= max_bx) && (min_bx <= max_ax && max_ax <= max_bx) && (min_by <= min_ay && min_ay <= max_by) && (min_by <= max_ay && max_ay <= max_by))
	{
		bigBoundingBox[0] = min_bx;
		bigBoundingBox[1] = min_by;
		bigBoundingBox[2] = max_bx;
		bigBoundingBox[3] = max_by;
		return bigBoundingBox;
	}
	else if ((min_ax <= min_bx && min_bx <= max_ax) && (min_ax <= max_bx && max_bx <= max_ax) && (min_ay <= min_by && min_by <= max_ay) && (min_ay <= max_by && max_by <= max_ay))
	{
		bigBoundingBox[0] = min_ax;
		bigBoundingBox[1] = min_ay;
		bigBoundingBox[2] = max_ax;
		bigBoundingBox[3] = max_ay;
		return bigBoundingBox;
	}
	else
	{
		return bigBoundingBox;
	}
}

std::vector<cv::Vec4i> BoundingBox::nonMaximaSupression(std::vector<cv::Vec4i> boxes)
{
	int minAx, minAy, maxAx, maxAy, minBx, minBy, maxBx, maxBy, minX, minY, maxX, maxY;
	int *coordinates;
	for (int i = 0; i < boxes.size(); i++)
		for (int j = 0; j < boxes.size(); j++)
		{
			if (i < j)
			{
				minAx = boxes[i][0];
				minAy = boxes[i][1];
				maxAx = boxes[i][2];
				maxAy = boxes[i][3];

				minBx = boxes[j][0];
				minBy = boxes[j][1];
				maxBx = boxes[j][2];
				maxBy = boxes[j][3];

				coordinates = inside(minAx, minAy, maxAx, maxAy, minBx, minBy, maxBx, maxBy);
				if (coordinates[0] != 0)
				{
					for (int pos = 0; pos < boxes.size(); pos++)
					{
						if (boxes[pos] == boxes[i] || boxes[pos] == boxes[j])
						{
							boxes[pos] = cv::Vec4i(coordinates[0], coordinates[1], coordinates[2], coordinates[3]);
						}
					}
					boxes[i] = cv::Vec4i(coordinates[0], coordinates[1], coordinates[2], coordinates[3]);
					boxes[j] = cv::Vec4i(coordinates[0], coordinates[1], coordinates[2], coordinates[3]);
				}
			}
		}
	std::vector<cv::Vec4i> finalBoxes;
	for (int i = 0; i < boxes.size(); i++)
	{
		bool insideFinalBoxes = false;
		for (int j = 0; j < finalBoxes.size(); j++)
		{
			if (boxes[i] == finalBoxes[j])
				insideFinalBoxes = true;
		}
		if (!insideFinalBoxes)
		{
			finalBoxes.push_back(boxes[i]);
		}
	}
	return finalBoxes;
}

cv::Vec4i BoundingBox::signCrop(cv::Vec4i line1, cv::Vec4i line2, int width, int height)
{
	int xa1, ya1, xa2, ya2;
	xa1 = line1[0];
	ya1 = line1[1];
	xa2 = line1[2];
	ya2 = line1[3];
	if (ya1 < ya2)
	{
		std::swap(xa1, xa2);
		std::swap(ya1, ya2);
	}

	int xb1, yb1, xb2, yb2;
	xb1 = line2[0];
	yb1 = line2[1];
	xb2 = line2[2];
	yb2 = line2[3];
	if (yb1 < yb2)
	{
		std::swap(xb1, xb2);
		std::swap(yb1, yb2);
	}
	int distance = abs(xa2 - xb2);
	float offset = computeOffset(xa1, ya1, xa2, ya2, xb1, yb1, xb2, yb2);
	int minX, minY, maxX, maxY;
	minX = xa2 - int(3 * distance) + int(offset);
	maxX = xb2 + int(3 * distance) + int(offset);
	minY = std::min(ya2, yb2) - int(6 * distance);
	maxY = std::max(ya2, yb2) + (1 * distance);
	cv::Vec4i crop;
	if (minY >= 0 && minX >= 0 && maxX < width && maxY < height)
	{
		crop = {minX, minY, maxX, maxY};
		return crop;
	}
	else if (minY < 0 && minX >= 0 && maxX < width && maxY < height)
	{
		crop = {minX, 0, maxX, maxY};
		return crop;
	}
	else if (minY >= 0 && minX >= 0 && maxX >= width && maxY < height)
	{
		crop = {minX, minY, width - 1, maxY};
		return crop;
	}
	else if (minY < 0 && minX >= 0 && maxX >= width && maxY < height)
	{
		crop = {minX, 0, width - 1, maxY};
		return crop;
	}
	else
	{
		crop = {0, 0, 0, 0};
		return crop;
	}
}

std::map<int, cv::Vec4i> BoundingBox::filterBoundingBoxes(int height, int width, std::map<int, std::vector<cv::Vec4i>> groups)
{
	int area, w, h;
	float ratio1, ratio2;
	std::map<int, std::vector<cv::Vec4i>>::iterator itr;
	std::map<int, cv::Vec4i> boxes;
	cv::Vec4i crop;
	int index = -2;
	int minX, minY, maxX, maxY;
	for (itr = groups.begin(); itr != groups.end(); ++itr)
	{
		if ((itr->second)[0][2] < (itr->second)[1][2])
			crop = signCrop((itr->second)[0], (itr->second)[1], width, height);
		else
			crop = signCrop((itr->second)[1], (itr->second)[0], width, height);
		minX = crop[0];
		minY = crop[1];
		maxX = crop[2];
		maxY = crop[3];
		area = (maxX - minX) * (maxY - minY);
		w = maxX - minX;
		h = maxY - minY;
		if (((maxY - minY) > 0) && ((maxX - minX) > 0) && (area > minArea))
		{
			ratio1 = h / float(w);
			ratio2 = w / float(h);
			if (ratio1 < 1.5f && ratio2 < 1.5f)
			{
				boxes.insert(std::pair<int, cv::Vec4i>(index--, cv::Vec4i(minX, minY, maxX, maxY)));
			}
		}
	}
	// std::vector<cv::Vec4i> finalBoxes = nonMaximaSupression(boxes);
	// return finalBoxes;
	return boxes;
}

std::map<int, cv::Vec4i> BoundingBox::determineColorBasedBoxes(std::vector<cv::Mat> masks, cv::Mat &originalImage, bool right)
{
	std::map<int, cv::Vec4i> boxes;
	int xMin, yMin, xMax, yMax, imageWidth = masks[0].cols, imageHeight = masks[0].rows, maxArea;
	int marginOffsetWidth, marginOffsetHeight;
	int redIndex = 1, blueIndex = 2, yellowIndex = 3;
	if (right)
		maxArea = masks[0].rows * masks[1].cols * 2;
	else
		maxArea = masks[0].rows * masks[1].cols;
	for (int index = 0; index < masks.size(); index++)
	{
		std::vector<std::vector<cv::Point>> currentContours;
		cv::findContours(masks[index], currentContours, cv::RETR_EXTERNAL, cv::CHAIN_APPROX_SIMPLE);
		for (int t = 0; t < currentContours.size(); t++)
		{
			cv::Rect rect = cv::boundingRect(currentContours[t]);
			if (rect.width / rect.height < 1.5 && rect.height / rect.width < 1.5 && ((rect.width * rect.height) > minArea) && ((rect.width * rect.height) < maxArea / 5.0f))
			{
				xMin = rect.x;
				yMin = rect.y;
				xMax = rect.x + rect.width;
				yMax = rect.y + rect.height;
				if (index == 3)
				{
					marginOffsetWidth = rect.width / 2.5;
					marginOffsetHeight = rect.height / 2.5;
				}
				else
				{
					marginOffsetWidth = 0;
					marginOffsetHeight = 0;
				}
				if (index == 1 || index == 2)
				{
					int width = xMax - xMin;
					int height = yMax - yMin;
					int diff = width - height;
					if (diff > 0)
					{
						if (yMin - diff > 0)
							yMin -= diff;
						else
							yMin = 0;
					}
				}
				if (xMin - int(rect.width / 20) - marginOffsetWidth > 0)
					xMin -= (int(rect.width / 20) + marginOffsetWidth);
				else
					xMin = 0;
				if (yMin - int(rect.height / 20) - marginOffsetHeight > 0)
					yMin -= (int(rect.height / 20) + marginOffsetHeight);
				else
					yMin = 0;
				if (xMax + int(rect.width / 20) + marginOffsetWidth < imageWidth - 1)
					xMax += (int(rect.width / 20) + marginOffsetWidth);
				else
					xMax = imageWidth - 1;
				if (yMax + int(rect.height / 20) + marginOffsetHeight < imageHeight - 1)
					yMax += (int(rect.height / 20) + marginOffsetHeight);
				else
					yMax = imageHeight - 1;
				cv::Vec4i box = {xMin, yMin, xMax, yMax};
				if (index == 0 || index == 1)
				{
					boxes.insert(std::pair<int, cv::Vec4i>(redIndex, box));
					redIndex += 10;
				}
				else if (index == 2)
				{
					boxes.insert(std::pair<int, cv::Vec4i>(blueIndex, box));
					blueIndex += 10;
				}
				else if (index == 3)
				{
					boxes.insert(std::pair<int, cv::Vec4i>(yellowIndex, box));
					yellowIndex += 10;
				}
			}
		}
	}
	return boxes;
}
