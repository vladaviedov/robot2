#include "stats.hpp"

#include <string>
#include <vector>
#include <fstream>
#include <optional>
#include <cstdio>
#include <cmath>
#include <opencv2/opencv.hpp>

#define CAM_WIDTH 1280
#define CAM_HEIGHT 720

struct best_rect {
	uint32_t area;
	int32_t delta_x;
	int32_t delta_y;
};

cv::Mat vision_algo_not_mask(cv::Mat &frame, uint32_t white_sens, uint32_t black_sens);
std::vector<std::vector<cv::Point>> vision_algo_filter(cv::Mat &input, uint32_t kernel_size, uint32_t iterations);
std::optional<std::pair<int32_t, int32_t>> vision_algo_target(const std::vector<std::vector<cv::Point>> &contours, uint32_t area_max, uint32_t size_min);

void sweep(const cv::Mat &frame, std::ofstream &outfile);

void test::var_sweep(std::string &filename, std::string &outfile) {
	// Get frame
	auto frame = cv::imread(filename);

	// Open outfile
	auto file = std::ofstream(outfile);

	// Convert to HSL
	cv::Mat hls_frame;
	cv::cvtColor(frame, hls_frame, cv::COLOR_BGR2HLS);

	sweep(hls_frame, file);
}

void test::save_image() {
	cv::VideoCapture camera(0);
	cv::Mat frame;
	
	uint32_t i = 0;
	while (true) {
		camera.read(frame);
		cv::imshow("Frame", frame);

		if (cv::waitKey(50) == 's') {
			std::string name = "image" + std::to_string(i) + ".png";

			cv::imwrite(name, frame);
			i++;
		}
	}

	camera.release();
}

cv::Mat vision_algo_not_mask(const cv::Mat &frame, uint32_t white_sens, uint32_t black_sens) {
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

std::vector<std::vector<cv::Point>> vision_algo_filter(const cv::Mat &input, uint32_t kernel_size, uint32_t iterations) {
	// Make kernel
	auto kernel = cv::getStructuringElement(cv::MORPH_RECT, cv::Size(kernel_size, kernel_size));

	// Erosion
	cv::Mat eroded;
	cv::morphologyEx(input, eroded, cv::MORPH_ERODE, kernel, cv::Point(-1, -1), iterations);

	// Dilate remainder
	cv::Mat dilated;
	cv::morphologyEx(eroded, dilated, cv::MORPH_DILATE, kernel, cv::Point(-1, -1), iterations);

	// Get contours
	std::vector<std::vector<cv::Point>> contours;
	std::vector<cv::Vec4i> hierarchy;
	cv::findContours(eroded, contours, hierarchy, cv::RETR_TREE, cv::CHAIN_APPROX_SIMPLE);

	return contours;
}

std::optional<std::pair<int32_t, int32_t>> vision_algo_target(const std::vector<std::vector<cv::Point>> &contours, uint32_t area_max, uint32_t size_min) {
	best_rect best = {
		.area = 0,
		.delta_x = INT32_MAX,
		.delta_y = INT32_MAX
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
		int32_t delta_x = (int32_t)center_x - CAM_WIDTH / 2;
		if (area > best.area && abs(delta_x) < abs(best.delta_x)) {
			best = {
				.area = (uint32_t)area,
				.delta_x = delta_x,
				.delta_y = box.y + box.height / 2 - CAM_HEIGHT / 2
			};
		}
	}

	return best.area > 0 ? std::make_optional(std::make_pair(best.delta_x, best.delta_y)) : std::nullopt;
}

void sweep(const cv::Mat &frame, std::ofstream &outfile) {
	for (uint32_t white = 5; white < 25; white++) {
		for (uint32_t black = 5; black < 25; black++) {
			std::cout << "Mask #" << (white - 5) * 25 + black - 4 << "/400";
			std::cout.flush();
			const auto else_mask = vision_algo_not_mask(frame, white * 10, black * 10);

			for (uint32_t kernel = 1; kernel < 7; kernel++) {
				for (uint32_t iter = 1; iter < 7; iter++) {
					const auto contours = vision_algo_filter(else_mask, kernel * 3, iter * 3);

					for (uint32_t max_area = 1; max_area < 20; max_area++) {
						for (uint32_t min_size = 0; min_size < CAM_WIDTH / 10 / 2; min_size++) {
							auto result = vision_algo_target(contours, max_area * 10000, min_size * 10);

							// Record meaningful result
							if (result.has_value()) {
								outfile << white * 10 << ","
									<< black * 10 << ","
									<< kernel * 3 << ","
									<< iter * 3 << ","
									<< max_area * 20000 << ","
									<< min_size * 10 << ","
									<< result.value().first << ","
									<< result.value().second << std::endl;
							}
						}
					}
				}
			}

			printf("\33[2K\r");
		}
	}
}
