#pragma once

#include <opencv4/opencv2/videoio.hpp>

class vision {

public:
	vision();
	~vision();

	void capture();
	void to_screen() const;

private:
	cv::Mat gen_mask(cv::Mat &frame, cv::Scalar &lower, cv::Scalar &upper) const;

	cv::VideoCapture camera;
	cv::Mat stored_white;
	cv::Mat stored_else;

};
