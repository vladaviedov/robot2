#include "stats.hpp"
#include "opencv2/imgproc.hpp"

#include <string>
#include <vector>
#include <fstream>
#include <optional>
#include <opencv2/opencv.hpp>

#define CAM_WIDTH 1920
#define CAM_HEIGHT 1080
#define OUTFILE "data.csv"

struct best_rect {
	cv::Rect rect;
	uint32_t area;
	uint32_t delta_x;
};

cv::Mat vision_algo_not_mask(cv::Mat &frame, uint32_t white_sens, uint32_t black_sens);
cv::Mat vision_algo_filter(cv::Mat &input, uint32_t kernel_size, uint32_t iterations);
std::optional<uint32_t> vision_algo_target(const cv::Mat &final, uint32_t area_max, uint32_t size_min);

void sweep(const cv::Mat &frame, std::ofstream &outfile);

void test::var_sweep() {
	// Open camera
	cv::VideoCapture camera;

	// Open outfile
	auto file = std::ofstream(OUTFILE);

	// Get camera frame
	cv::Mat frame;
	camera.read(frame);

	sweep(frame, file);

	// Release camera
	camera.release();
}

cv::Mat vision_algo_not_mask(const cv::Mat &frame, uint32_t white_sens, uint32_t black_sens) {
	// Convert to HSL
	cv::Mat hls_frame;
	cv::cvtColor(frame, hls_frame, cv::COLOR_BGR2HLS);

	// Generate bounds
	cv::Scalar white_lower(0, 255 - white_sens, 0);
	cv::Scalar white_upper(255, 255, white_sens);
	cv::Scalar black_lower(0, 0, 0);
	cv::Scalar black_upper(255, black_sens, black_sens);

	// Generate masks
	cv::Mat white_mask;
	cv::Mat black_mask;
	cv::inRange(frame, white_lower, white_upper, white_mask);
	cv::inRange(frame, black_lower, black_upper, black_mask);

	// Generate inverses
	cv::Mat not_white_mask;
	cv::Mat not_black_mask;
	cv::bitwise_not(white_mask, not_white_mask);
	cv::bitwise_not(black_mask, not_black_mask);

	// Generate else_mask
	cv::Mat else_mask;
	cv::bitwise_and(not_white_mask, not_black_mask, else_mask);

	return else_mask;
}

cv::Mat vision_algo_filter(const cv::Mat &input, uint32_t kernel_size, uint32_t iterations) {
	// Make kernel
	auto kernel = cv::getStructuringElement(cv::MORPH_RECT, cv::Size(kernel_size, kernel_size));

	// Erosion
	cv::Mat eroded;
	cv::morphologyEx(input, eroded, cv::MORPH_ERODE, kernel, cv::Point(-1, -1), iterations);

	// Dilate remainder
	cv::Mat dilated;
	cv::morphologyEx(eroded, dilated, cv::MORPH_DILATE, kernel, cv::Point(-1, -1), iterations);

	return dilated;
}

std::optional<uint32_t> vision_algo_target(const cv::Mat &final, uint32_t area_max, uint32_t size_min) {
	// Get contours
	std::vector<std::vector<cv::Point>> contours;
	std::vector<cv::Vec4i> hierarchy;
	cv::findContours(final, contours, hierarchy, cv::RETR_TREE, cv::CHAIN_APPROX_SIMPLE);

	best_rect best = {
		.area = 0,
		.delta_x = 0
	};

	// Find best rectangle
	for (auto &c : contours) {
		auto area = cv::contourArea(c);
		auto box = cv::boundingRect(c);

		// Area filter
		if (area > area_max) {
			continue;
		}

		// Side length filter
		if ((uint32_t)box.height < size_min || (uint32_t)box.width < size_min) {
			continue;
		}

		// Check if better than best
		uint32_t center_x = box.x + box.width / 2;
		uint32_t delta_x = CAM_WIDTH / 2 - center_x;
		if (area > best.area && delta_x > best.delta_x) {
			best = {
				.area = (uint32_t)area,
				.delta_x = delta_x
			};
		}
	}

	return best.area > 0 ? std::make_optional(best.delta_x) : std::nullopt;
}

void sweep(const cv::Mat &frame, std::ofstream &outfile) {
	for (uint32_t white = 0; white < 255; white++) {
		for (uint32_t black = 0; black < 255; black++) {
			const cv::Mat else_mask = vision_algo_not_mask(frame, white, black);

			for (uint32_t kernel = 1; kernel < 20; kernel++) {
				for (uint32_t iter = 1; iter < 20; iter++) {
					const cv::Mat filtered = vision_algo_filter(else_mask, kernel, iter);

					for (uint32_t max_area = 1; max_area < 2000; max_area++) {
						for (uint32_t min_size = 0; min_size < CAM_WIDTH; min_size++) {
							auto result = vision_algo_target(filtered, max_area * 100, min_size);

							outfile << white << ","
								<< black << ","
								<< kernel << ","
								<< iter << ","
								<< max_area << ","
								<< min_size << ",";

							result.has_value()
								? outfile << result.value()
								: outfile << "FAILED";
							
							outfile << "\n";
						}
					}
				}
			}
		}
	}
}
