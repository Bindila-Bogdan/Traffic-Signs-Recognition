#ifndef NEURAL_NETWORK_H
#define NEURAL_NETWORK_H

#include <fdeep/fdeep.hpp>
#include <opencv2/opencv.hpp>
#include <opencv2/core/core.hpp>
#include <opencv2/imgproc/imgproc.hpp>

#define CLIP_LIMIT 0.1
#define MIN_PROBABILITY 0.99
const std::string signs[5] = {"Other", "Parking", "Pedestrian", "Priority", "Stop"};

class NeuralNetwork
{
	static const int imgSize = 64;
	static const int patchSize = 8;

public:
	static cv::Mat prepareImage(cv::Mat &image, bool contestMode);
	static std::string getPredictions(cv::Mat &image, bool contestMode);
};

#endif