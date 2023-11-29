#include "vision.hpp"

#include <vector>
#include <opencv2/opencv.hpp>

#include "filter.hpp"

#define WHITE_SENS 170
#define BLACK_SENS 130
#define CAMERA_WIDTH 600
#define CAMERA_HEIGHT 400

static cv::Scalar white_lower(0, 255 - WHITE_SENS, 0);
static cv::Scalar white_upper(255, 255, WHITE_SENS);
static cv::Scalar black_lower(0, 0, 0);
static cv::Scalar black_upper(255, BLACK_SENS, BLACK_SENS);

vision::vision() : camera(0) {}

vision::~vision() {
	cv::destroyAllWindows();
	camera.release();
}

void vision::process() {
	// Get camera frame
	cv::Mat frame;
	camera.read(frame);

	// Convert to HSL
	cv::Mat hls_frame;
	cv::cvtColor(frame, hls_frame, cv::COLOR_BGR2HLS);

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

	// Display masks
	cv::imshow("White mask", white_mask);
	cv::imshow("Else mask", else_mask);

	// Get contours
	std::vector<std::vector<cv::Point>> contours;
	std::vector<cv::Vec4i> hierarchy;
	cv::findContours(else_mask, contours, hierarchy, cv::RETR_TREE, cv::CHAIN_APPROX_SIMPLE);

	best_area = 0;

	// Display bounding boxes
	for (auto &c : contours) {
		auto area = cv::contourArea(c);
		auto rect = cv::boundingRect(c);

		// Filter small things
		if (rect.height < 100 || rect.width < 100) {
			continue;
		}

		// Filter huge things
		if (area > (CAMERA_WIDTH * CAMERA_HEIGHT) / 2.0) {
			continue;
		}

		if (area > best_area) {
			best_rect = rect;
			best_area = area;
		}

		cv::rectangle(frame, cv::Point(rect.x, rect.y), cv::Point(rect.x + rect.width, rect.y + rect.height),
			cv::Scalar(0, 0, 255), 2);
	}
	cv::imshow("Boxes", frame);

	// Delay
	cv::waitKey(50);
}

guess_dir vision::make_guess() const {
	uint64_t center_x = best_rect.x + best_rect.width / 2;

	if (center_x < CAMERA_WIDTH / 3) {
		return guess_dir::LEFT;
	} else if (center_x < CAMERA_WIDTH * 2 / 3) {
		return guess_dir::STRAIGHT;
	} else {
		return guess_dir::RIGHT;
	}
}

cv::Mat vision::gen_mask(cv::Mat &frame, cv::Scalar &lower, cv::Scalar &upper) const {
	cv::Mat mask;

	// Cutoff
	cv::inRange(frame, lower, upper, mask);

	// TODO: Apply filters
	mask = filter::morph(mask);
	
	return mask;
}
