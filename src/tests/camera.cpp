#include "camera.hpp"

#include <iostream>
#include <thread>
#include <chrono>

#include "../control/vision.hpp"

void test::show_masks() {
	vision v;

	while (true) {
		v.process();
		v.make_guess();
	}
}
