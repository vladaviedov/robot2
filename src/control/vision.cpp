#include "vision.hpp"

#include <opencv2/opencv.hpp>

#define WHITE_SENS 100
#define BLACK_SENS 130

static cv::Scalar white_lower(0, 255 - WHITE_SENS, 0);
static cv::Scalar white_upper(255, 255, WHITE_SENS);
static cv::Scalar black_lower(0, 0, 0);
static cv::Scalar black_upper(255, BLACK_SENS, BLACK_SENS);

vision::vision() : camera(0) {}

vision::~vision() {
	camera.release();
}

void vision::capture() {
	// Get camera frame
	cv::Mat frame;
	camera.read(frame);

	// Convert to HSL
	cv::Mat hls_frame;
	cv::cvtColor(frame, hls_frame, cv::COLOR_BGR2HLS);
	cv::imshow("Frame", hls_frame);

	// Generate masks
	cv::Mat white_mask = gen_mask(frame, white_lower, white_upper);
	cv::Mat black_mask = gen_mask(frame, black_lower, black_upper);

	// Generate everything else mask
	cv::Mat not_white_mask;
	cv::Mat not_black_mask;
	cv::Mat else_mask;
	cv::bitwise_not(white_mask, not_white_mask);
	cv::bitwise_not(black_mask, not_black_mask);
	cv::bitwise_and(not_white_mask, not_black_mask, else_mask);

	// Store masks
	stored_white = white_mask;
	stored_else = else_mask;
}

void vision::to_screen() const {
	cv::imshow("White mask", stored_white);
	cv::imshow("Else mask", stored_else);
	cv::waitKey(50);
}

cv::Mat vision::gen_mask(cv::Mat &frame, cv::Scalar &lower, cv::Scalar &upper) const {
	cv::Mat mask;

	// Cutoff
	cv::inRange(frame, lower, upper, mask);

	// TODO: Apply filters
	
	return mask;
}
