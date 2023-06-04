#include "stdafx.h"
#include "LinesProcessing.h"

std::vector<cv::Vec4i> LinesProcessing::sortLines(std::vector<cv::Vec4i> lines)
{
	for (int i = 0; i < lines.size() - 1; i++)
		for (int j = i + 1; j < lines.size(); j++)
		{
			if (lines[i][0] > lines[j][0])
				std::swap(lines[i], lines[j]);
		}
	return lines;
}

bool LinesProcessing::angleInInterval(cv::Vec4i line)
{
	int x1, y1, x2, y2;
	x1 = line[0];
	y1 = line[1];
	x2 = line[2];
	y2 = line[3];
	if (abs(x2 - x1) == 0)
		return true;
	float m = (y2 - y1) / float(x2 - x1);
	float degreeAngle = atan(m) * 180 / PI;
	return (90 - offsetAngle <= abs(degreeAngle)) && (abs(degreeAngle) <= 90 + offsetAngle);
}

std::vector<cv::Vec4i> LinesProcessing::filterByAngle(std::vector<cv::Vec4i> lines)
{
	if (lines.size() == 0)
		return lines;
	else
	{
		std::vector<cv::Vec4i> perpLines;
		for (int i = 0; i < lines.size(); i++)
			if (angleInInterval(lines[i]))
				perpLines.push_back(lines[i]);
		if (perpLines.size() == 0)
			return perpLines;
		std::vector<cv::Vec4i> sortedPerpLines = sortLines(perpLines);
		return sortedPerpLines;
	}
}

cv::Vec4i LinesProcessing::betterLine(cv::Vec4i line1, cv::Vec4i line2)
{
	int minY1, minY2;
	minY1 = std::min(line1[1], line1[3]);
	minY2 = std::min(line2[1], line2[3]);
	if (minY1 < minY2)
		return line1;
	else
		return line2;
}

std::map<int, cv::Vec4i> LinesProcessing::removeNeighbours(std::map<int, cv::Vec4i> lineDict, int currentColor, std::set<int> colors)
{
	cv::Vec4i bestLine = lineDict[currentColor];
	int keepColor = currentColor;
	std::map<int, cv::Vec4i> lineDictCopy = lineDict;
	cv::Vec4i line;
	std::set<int>::iterator itr;
	for (itr = colors.begin(); itr != colors.end(); ++itr)
	{
		line = betterLine(bestLine, lineDictCopy[*itr]);
		if (!(line[0] == bestLine[0] && line[1] == bestLine[1] &&
			  line[2] == bestLine[2] && line[3] == bestLine[3]))
		{
			bestLine = lineDictCopy[*itr];
			keepColor = *itr;
		}
	}
	for (itr = colors.begin(); itr != colors.end(); ++itr)
	{
		if (*itr != keepColor)
			lineDict.erase(*itr);
	}
	return lineDict;
}

std::set<int> LinesProcessing::checkMatrix(int x, int y, int currentColor, cv::Mat &image)
{
	int height = image.rows;
	int width = image.cols;
	std::set<int> colors = {currentColor};
	for (int i = 0; i < matrixSize; i++)
		for (int j = 0; j < matrixSize; j++)
		{
			int pixel1, pixel2, pixel3, pixel4;
			if (y - i < 0 || x - j < 0)
				;
			else
			{
				pixel1 = image.at<uchar>(y - i, x - j);
				if (0 < pixel1 && pixel1 < 255)
					colors.insert(pixel1);
			}
			if (y - i < 0 || x + j > width - 1)
				;
			else
			{
				pixel2 = image.at<uchar>(y - i, x + j);
				if (0 < pixel2 && pixel2 < 255)
					colors.insert(pixel2);
			}
			if (y + i > height - 1 || x - j < 0)
				;
			else
			{
				pixel3 = image.at<uchar>(y + i, x - j);
				if (0 < pixel3 && pixel3 < 255)
					colors.insert(pixel3);
			}
			if (y + i > height - 1 || x + j > width - 1)
				;
			else
			{
				pixel4 = image.at<uchar>(y + i, x + j);
				if (0 < pixel4 && pixel4 < 255)
					colors.insert(pixel4);
			}
		}
	return colors;
}

std::vector<cv::Vec4i> LinesProcessing::supressLines(std::vector<cv::Vec4i> lines, cv::Mat &image)
{
	cv::Mat imageClone = image.clone();
	std::map<int, cv::Vec4i> lineDict;
	std::vector<cv::Vec4i> linesToReturn;
	int lineNumber = 1;
	if (lines.size() == 0)
		return linesToReturn;
	cv::Vec4i line;
	int x1, y1, x2, y2;
	for (int i = 0; i < lines.size(); i++)
	{
		line = lines[i];
		if (line[1] < line[3])
		{
			x1 = line[0];
			y1 = line[1];
			x2 = line[2];
			y2 = line[3];
			line = cv::Vec4i(line[2], line[3], line[0], line[1]);
		}
		cv::line(imageClone, cv::Point(line[0], line[1]), cv::Point(line[2], line[3]), lineNumber, 1);
		lineDict[lineNumber] = line;
		lineNumber++;
	}
	std::map<int, cv::Vec4i> lineDictCopy = lineDict;
	std::map<int, cv::Vec4i>::iterator itr0, itr1, itr2;
	std::set<int> colors;
	std::set<int>::iterator itrColor;
	bool inside, inside1;
	for (itr1 = lineDict.begin(); itr1 != lineDict.end(); ++itr1)
	{
		inside = false;
		for (itr2 = lineDictCopy.begin(); itr2 != lineDictCopy.end(); ++itr2)
			if (itr1->first == itr2->first)
				inside = true;
		if (inside)
		{
			std::set<int> currentColors;
			colors = checkMatrix(lineDict[itr1->first][2], lineDict[itr1->first][3], itr1->first, imageClone);
			for (itrColor = colors.begin(); itrColor != colors.end(); ++itrColor)
			{
				inside1 = false;
				for (itr0 = lineDictCopy.begin(); itr0 != lineDictCopy.end(); ++itr0)
				{
					if (*itrColor == itr0->first)
						inside1 = true;
				}
				if (inside1)
					currentColors.insert(*itrColor);
			}
			if (currentColors.size() >= 2)
				lineDictCopy = removeNeighbours(lineDictCopy, itr1->first, currentColors);
		}
	}
	for (itr0 = lineDictCopy.begin(); itr0 != lineDictCopy.end(); ++itr0)
		linesToReturn.push_back(itr0->second);
	return linesToReturn;
}

bool LinesProcessing::sameLevel(cv::Vec4i line1, cv::Vec4i line2)
{
	int maxY1, minY1, maxY2, minY2;
	maxY1 = std::max(line1[1], line1[3]);
	minY1 = std::min(line1[1], line1[3]);

	maxY2 = std::max(line2[1], line2[3]);
	minY2 = std::min(line2[1], line2[3]);

	bool oyIntersection = (minY2 <= maxY1 && maxY1 <= maxY2) || (minY2 <= minY1 && minY1 <= maxY2) ||
						  (minY1 <= maxY2 && maxY2 <= maxY1) || (minY1 <= minY2 && minY2 <= maxY1);
	int intersectionLength = 0;
	if (minY2 <= maxY1 && maxY1 <= maxY2)
		intersectionLength = maxY1 - minY2;
	else if (minY2 <= minY1 && minY1 <= maxY2)
		intersectionLength = maxY2 - minY1;
	else if (minY1 <= maxY2 && maxY2 <= maxY1)
		intersectionLength = maxY2 - minY1;
	else if (minY1 <= minY2 && minY2 <= maxY1)
		intersectionLength = maxY1 - minY2;
	int l1, l2, lMin;
	l1 = sqrt(pow((line1[0] - line1[2]), 2) + pow((line1[1] - line1[3]), 2));
	l2 = sqrt(pow((line2[0] - line2[2]), 2) + pow((line2[1] - line2[3]), 2));
	lMin = std::min(l1, l2);
	if (intersectionLength == 0 || !oyIntersection)
		return false;
	else
		return (lMin / float(intersectionLength) > MIN_INTERSECTION);
}

std::map<int, std::vector<cv::Vec4i>> LinesProcessing::groupLines(std::vector<cv::Vec4i> lines)
{
	std::vector<cv::Vec4i> sortedLines = sortLines(lines);
	int groupNumber = 1;
	std::map<int, std::vector<cv::Vec4i>>::iterator itr;
	std::map<int, std::vector<cv::Vec4i>> groups;
	bool notInside = true;
	for (int i = 0; i < lines.size(); i++)
		for (int j = 0; j < lines.size(); j++)
		{
			if ((i + 1 == j) && sameLevel(sortedLines[i], sortedLines[j]) &&
				(abs(sortedLines[i][2] - sortedLines[j][2]) < maxDistance) &&
				(abs(sortedLines[i][3] - sortedLines[j][3]) < maxDistance))
			{
				if (groups.size() == 0)
				{
					std::vector<cv::Vec4i> first;
					first.push_back(sortedLines[i]);
					first.push_back(sortedLines[j]);
					groups.insert(std::pair<int, std::vector<cv::Vec4i>>(groupNumber, first));
					groupNumber++;
				}
				else
				{
					for (itr = groups.begin(); itr != groups.end(); ++itr)
						if (itr->second[0] == sortedLines[i] || itr->second[0] == sortedLines[j])
							notInside = false;
					if (notInside)
					{
						std::vector<cv::Vec4i> vec;
						vec.push_back(sortedLines[i]);
						vec.push_back(sortedLines[j]);
						groups.insert(std::pair<int, std::vector<cv::Vec4i>>(groupNumber, vec));
						groupNumber++;
					}
				}
			}
		}
	return groups;
}