#include "filter.hpp"
#include "opencv2/imgproc.hpp"

#include <opencv2/opencv.hpp>

// TODO: adjust settings
#define KERNEL_SIZE 3
#define ITER 3

cv::Mat filter::morph(cv::Mat &input) {
	auto kernel = cv::getStructuringElement(cv::MORPH_RECT, cv::Size(KERNEL_SIZE, KERNEL_SIZE));

	// Erosion
	cv::Mat eroded;
	cv::morphologyEx(input, eroded, cv::MORPH_ERODE, kernel, cv::Point(-1, -1), ITER);

	// Dilate remainder
	cv::Mat dilated;
	cv::morphologyEx(eroded, dilated, cv::MORPH_DILATE, kernel, cv::Point(-1, -1), ITER);

	return dilated;
}
