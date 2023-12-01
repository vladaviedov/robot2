/**
 * @file control/control.cpp
 * @brief Robot control logic.
 */
#include "control.hpp"

#include <iostream>
#include <thread>
#include <chrono>
#include <atomic>
#include <gpiod.hpp>

#include "../driver/motors.hpp"
#include "../driver/hcsr04.hpp"
#include "../driver/lightsens.hpp"
#include "robotmap.hpp"
#include "vision.hpp"

// Ultrasonic max distance to detect target
#define US_THRESHOLD 4500

std::atomic_bool active = false;
std::atomic_int edge_timer = 0;

void control::activate() {
	// Get GPIO chip
	gpiod::chip chip("gpiochip0");

	// Initialize motors
	motor motor1(chip, pin::m1in1, pin::m1in2, pin::m1pwm);
	motor motor2(chip, pin::m2in1, pin::m2in2, pin::m2pwm);
	motor motor3(chip, pin::m3in1, pin::m3in2, pin::m3pwm);
	motor motor4(chip, pin::m4in1, pin::m4in2, pin::m4pwm);

	// Our motors couldn't spin in both directions sadly
	/* motor1.invert(); */
	/* motor4.invert(); */

	// Initialize infrareds
	light_sens left_ir(chip, pin::ir1);
	light_sens right_ir(chip, pin::ir2);

	// Initialize ultrasonic
	hc_sr04 us(chip, pin::us_trig, pin::us_echo);

	// Initialize vision
	vision camera;

	// Enemy detection confident level
	uint32_t conf_level = 0;
	// Edge sensor detection side
	uint32_t edge_side = 0;

	// Create thread for getting user input
	std::thread input_thread([](){
		while (true) {
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
		}
	});

	// Create thread for decreasing the edge code timer
	std::thread edge_thread([](){
		while (true) {
			std::this_thread::sleep_for(std::chrono::milliseconds(100));
			if (edge_timer > 0) {
				edge_timer--;
			}
		}
	});

	while (true) {
		std::this_thread::sleep_for(std::chrono::milliseconds(40));

		// Do nothing when inactive
		if (!active) {
			motor1.stop();
			motor2.stop();
			motor3.stop();
			motor4.stop();
			edge_timer = 0;
			edge_side = 0;
			conf_level = 0;
			continue;
		}

		// Read ultrasonic
		uint64_t pulse = us.pulse();
		std::cout << pulse << std::endl;

		// Check for edge detection
		if (left_ir.read() && pulse > 200) {
			edge_timer = 10;
			edge_side = 2;
			std::cout << "EDGE!!" << std::endl;
		}
		if (right_ir.read() && pulse > 200) {
			edge_timer = 10;
			edge_side = 1;
			std::cout << "EDGE!!" << std::endl;
		}

		// Edge avoidance code (sadly cannot reverse)
		if (edge_timer > 0) {
			if (edge_side == 1) {
				motor1.set(0, FORWARD);
				motor2.set(100, FORWARD);
				motor3.set(100, FORWARD);
				motor4.set(0, FORWARD);
			} else {
				motor1.set(100, FORWARD);
				motor2.set(0, FORWARD);
				motor3.set(0, FORWARD);
				motor4.set(100, FORWARD);
			}
			continue;
		}

		// If enemy no longer detected change confidence level
		if (conf_level > 0) {
			if (pulse > US_THRESHOLD) {
				std::cout << "Losing confidence" << std::endl;
				conf_level--;
			}
		}

		// Detecting enemy otherwise
		if (pulse < US_THRESHOLD) {
			std::cout << "Enemy spotted" << std::endl;
			conf_level = 3;
		}

		// If enemy detected, CHARGE!
		if (conf_level > 2) {
			motor1.set(100, FORWARD);
			motor2.set(100, FORWARD);
			motor3.set(100, FORWARD);
			motor4.set(100, FORWARD);

			continue;
		}

		// Check camera input
		camera.process(false);
		int64_t delta = camera.make_guess();

		// Go left or right based on the guess
		if (delta < 0) {
			std::cout << "Enemy detected left side" << std::endl;
			motor1.set(100, FORWARD);
			motor2.set(0, FORWARD);
			motor3.set(0, FORWARD);
			motor4.set(1000, FORWARD);
		} else {
			std::cout << "Enemy detected right side" << std::endl;
			motor1.set(0, FORWARD);
			motor2.set(100, FORWARD);
			motor3.set(100, FORWARD);
			motor4.set(0, FORWARD);
		}
	}
}
