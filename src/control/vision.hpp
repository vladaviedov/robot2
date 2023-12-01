/**
 * @file control/vision.hpp
 * @brief Computer vision header.
 */
#pragma once

#include <opencv4/opencv2/videoio.hpp>

class vision {

public:
	vision();

	~vision();

	/**
	 * @brief Process a new frame.
	 *
	 * @param[in] show_output - Whether to draw output to screen.
	 */
	void process(bool show_output);

	/**
	 * @brief Make a guess on where the robot should go.
	 *
	 * @return Delta x from center to target.
	 */
	int32_t make_guess() const;

private:
	/**
	 * @brief Generate mask from HSL bounds.
	 *
	 * @param[in] frame - Input frame.
	 * @param[in] lower - Lower HSL bound.
	 * @param[in] upper - Upper HSL bound.
	 */
	cv::Mat gen_mask(cv::Mat &frame, cv::Scalar &lower, cv::Scalar &upper) const;

	cv::VideoCapture camera;
	uint64_t best_area;
	cv::Rect best_rect;

};
