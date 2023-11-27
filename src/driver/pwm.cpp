#include "pwm.hpp"

#include <chrono>
#include <thread>

// 500 Hz PWM -> 2000us / period
#define TOTAL_TIME 2000
#define TIME_PER_PERCENT (TOTAL_TIME / 100)
#define GPIO_USER "robot2_pwm"

pwm_worker::pwm_worker(gpiod::chip &chip, uint32_t pin) :
	line(chip.get_line(pin)) {
	// Set pin to output
	line.request({
		.consumer = GPIO_USER,
		.request_type = gpiod::line_request::DIRECTION_OUTPUT,
		.flags = 0
	}, 0);

	// Create worker thread
	auto executor = [&](){
		while (active) {
			if (duty_percent != 0) {
				line.set_value(1);
				std::this_thread::sleep_for(
					std::chrono::microseconds(duty_percent * TIME_PER_PERCENT));
			}
			if (duty_percent != 100) {
				line.set_value(0);
				std::this_thread::sleep_for(
					std::chrono::microseconds((100 - duty_percent) * TIME_PER_PERCENT));
			}
		}
	};
}

pwm_worker::~pwm_worker() {
	// Destroy thread
	active = false;
	std::this_thread::sleep_for(std::chrono::microseconds(TOTAL_TIME));
	pwm_thread->join();

	// Reset gpio line
	line.set_value(0);
	line.release();
}
