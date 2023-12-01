/**
 * @file tests/camera.hpp
 * @brief Functions for testing camera.
 */
#include "camera.hpp"

#include <iostream>
#include <thread>
#include <chrono>

#include "../control/vision.hpp"

void test::show_camera() {
	vision v;

	while (true) {
		// Show boxes
		v.process(true);

		// Print out delta x of guess
		std::cout << v.make_guess() << std::endl;
	}
}
