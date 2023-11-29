#include "tests/camera.hpp"
#include "tests/driver.hpp"
#include "tests/stats.hpp"

#include <iostream>

int main() {
	// For all images
	for (int index = 2; index < 21; index++) {
		std::string input_frame = "images/image" + std::to_string(index) + ".png";
		std::string data_file = "data/image" + std::to_string(index) + ".csv";

		std::cout << "Starting analysis for image " << index << std::endl;
		test::var_sweep(input_frame, data_file);
		std::cout << "Image " << index << " analysis complete" << std::endl;
	}

	return 0;
}
