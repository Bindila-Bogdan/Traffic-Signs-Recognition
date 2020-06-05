#include "stdafx.h"
#include "NeuralNetwork.h"

const auto model = fdeep::load_model("signs_recognition_model.json");

cv::Mat NeuralNetwork::prepareImage(cv::Mat& image, bool contestMode) {
    int pixelValue;
    float scaledPixelValue;
    cv::Mat resizedImage;
    resize(image, resizedImage, cv::Size(imgSize, imgSize));
    cv::Mat equalizedResizedImage = resizedImage.clone();
    cv::Ptr<cv::CLAHE> clahe = cv::createCLAHE(CLIP_LIMIT, cv::Size(patchSize, patchSize));
    clahe->apply(resizedImage, equalizedResizedImage);
    return equalizedResizedImage;
}

std::string NeuralNetwork::getPredictions(cv::Mat& image, bool contestMode) {
    std::string prediction;
    float probability;
    prediction = "Other";
    cv::Mat preparedImg = prepareImage(image, contestMode);
    if (!preparedImg.isContinuous()) {
        return prediction;
    }
    else {
        const auto input = fdeep::tensor_from_bytes(preparedImg.ptr(),
            static_cast<std::size_t>(preparedImg.rows),
            static_cast<std::size_t>(preparedImg.cols),
            static_cast<std::size_t>(preparedImg.channels()), 0.0f, 1.0f);
        const auto result = model.predict_class_with_confidence({ input });
        if (result.second > MIN_PROBABILITY) {
            if (result.first != 0)
            {
                prediction = signs[result.first];
                probability = result.second;
                if (!contestMode)
                    cv::imshow("Example of processed ROI", preparedImg);
                return prediction;
            }
            else
                return prediction;
        }
        else
            return prediction;
    }
}
