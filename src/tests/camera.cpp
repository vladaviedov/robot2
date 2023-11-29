#include "camera.hpp"

#include <iostream>
#include <thread>
#include <chrono>

#include "../control/vision.hpp"

void test::show_masks() {
	vision v;

	while (true) {
		v.process();
		switch (v.make_guess()) {
			case LEFT:
				std::cout << "I think we should go left" << std::endl;
				break;
			case STRAIGHT:
				std::cout << "I think we should go forward" << std::endl;
				break;
			case RIGHT:
				std::cout << "I think we should go right" << std::endl;
				break;
		}
	}
}
