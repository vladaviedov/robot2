#pragma once

#include <opencv4/opencv2/videoio.hpp>

class vision {

public:
	vision();
	~vision();

	void process(bool show_output);
	int32_t make_guess() const;

private:
	cv::Mat gen_mask(cv::Mat &frame, cv::Scalar &lower, cv::Scalar &upper) const;

	cv::VideoCapture camera;
	uint64_t best_area;
	cv::Rect best_rect;

};
