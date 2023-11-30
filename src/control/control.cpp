#include "control.hpp"

#include <atomic>
#include <chrono>
#include <iostream>
#include <gpiod.hpp>
#include <thread>

#include "../driver/motors.hpp"
#include "../driver/hcsr04.hpp"
#include "../driver/lightsens.hpp"
#include "robotmap.hpp"
#include "vision.hpp"

#define US_THRESHOLD 10000

std::atomic_bool active = false;

void control::activate() {
	gpiod::chip chip("gpiochip0");

	// Initialize motors
	motor motor1(chip, pin::m1in1, pin::m1in2, pin::m1pwm);
	motor motor2(chip, pin::m2in1, pin::m2in2, pin::m2pwm);
	motor motor3(chip, pin::m3in1, pin::m3in2, pin::m3pwm);
	motor motor4(chip, pin::m4in1, pin::m4in2, pin::m4pwm);
	motor1.invert();
	motor4.invert();

	// Initialize infrareds
	light_sens left_ir(chip, pin::ir1);
	light_sens right_ir(chip, pin::ir2);

	// Initialize ultrasonic
	hc_sr04 us(chip, pin::us_trig, pin::us_echo);

	// Initialize vision
	vision camera;

	uint32_t conf_level = 0;
	std::thread input_thread([](){
		if (active) {
			char ch;
			std::cout << "Robot is active. Input 's' to stop" << std::endl;
			std::cin >> ch;

			if (ch == 's') {
				active = false;
			}
		} else {
			char ch;
			std::cout << "Robot in inactive. Input 'r' to run" << std::endl;
			std::cin >> ch;

			if (ch == 'r') {
				active = true;
			}
		}
	});

	while (true) {
		std::this_thread::sleep_for(std::chrono::microseconds(20));

		if (!active) {
			continue;
		}

		if (left_ir.read() || right_ir.read()) {
			std::cout << "EDGE!!" << std::endl;
			motor1.set(100, BACKWARD);
			motor2.set(100, BACKWARD);
			motor3.set(100, BACKWARD);
			motor4.set(100, BACKWARD);
			continue;
		}

		if (conf_level > 0) {
			if (us.pulse(0) > US_THRESHOLD) {
				std::cout << "Losing confidence" << std::endl;
				conf_level--;
			} else {
				std::cout << "Enemy spotted" << std::endl;
				conf_level = 3;
			}
		}

		// Enemy found
		if (us.pulse(0) < US_THRESHOLD) {
			std::cout << "Enemy spotted" << std::endl;
			conf_level = 3;
		}

		if (conf_level > 2) {
			motor1.set(100, FORWARD);
			motor2.set(100, FORWARD);
			motor3.set(100, FORWARD);
			motor4.set(100, FORWARD);

			continue;
		}

		camera.process();
		int64_t delta = camera.make_guess();

		if (delta < 0) {
			std::cout << "Enemy detected left side" << std::endl;
			motor1.set(100, FORWARD);
			motor2.set(100, FORWARD);
			motor3.set(100, BACKWARD);
			motor4.set(100, BACKWARD);
		} else {
			std::cout << "Enemy detected right side" << std::endl;
			motor1.set(100, BACKWARD);
			motor2.set(100, BACKWARD);
			motor3.set(100, FORWARD);
			motor4.set(100, FORWARD);
		}
	}
}
