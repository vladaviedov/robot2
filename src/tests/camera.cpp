#include "camera.hpp"

#include <thread>
#include <chrono>

#include "../control/vision.hpp"

void test::show_masks() {
	vision v;

	while (true) {
		v.capture();
		v.to_screen();
	}
}
