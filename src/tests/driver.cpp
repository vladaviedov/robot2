/**
 * @file tests/driver.cpp
 * @brief Functions for testing hardware and driver code.
 */
#include "driver.hpp"

#include <cmath>
#include <iostream>
#include <thread>
#include <chrono>
#include <gpiod.hpp>

#include "../driver/pinmap.hpp"
#include "../driver/pwm.hpp"
#include "../driver/motors.hpp"
#include "../driver/hcsr04.hpp"
#include "../control/robotmap.hpp"

#define GPIO_USER "robot2_test"

// GPIO Chip object for libgpiod
std::unique_ptr<gpiod::chip> chip;

void test::driver_prepare() {
	chip = std::make_unique<gpiod::chip>("gpiochip0");
}

void test::gpio_out() {
	// Setup test 37 line
	gpiod::line test_line = chip->get_line(RASPI_37);
	test_line.request({
		.consumer = GPIO_USER,
		.request_type = gpiod::line_request::DIRECTION_OUTPUT,
		.flags = 0
	}, 0);

	// Control from console
	while (true) {
		int input;
		std::cout << "Input value: ";
		std::cin >> input;

		if (input == 0) {
			test_line.set_value(0);
		}
		if (input == 1) {
			test_line.set_value(1);
		}
	}
}

void test::gpio_in() {
	// Setup test 37 line
	gpiod::line test_line = chip->get_line(RASPI_37);
	test_line.request({
		.consumer = GPIO_USER,
		.request_type = gpiod::line_request::DIRECTION_INPUT,
		.flags = 0
	});

	// Control from console
	while (true) {
		int value = test_line.get_value();
		std::cout << value << std::endl;
		std::this_thread::sleep_for(std::chrono::milliseconds(50));
	}
}

void test::pwm() {
	// Setup test 37 line
	pwm_worker test_pwm(*chip, RASPI_37);	

	// Control from console
	while (true) {
		int input;
		std::cout << "Input duty: ";
		std::cin >> input;

		if (input >= 0 && input <= 100) {
			test_pwm.set_duty(input);
		}
	}
}

void test::one_motor() {
	motor motor(*chip, pin::m1in1, pin::m1in2, pin::m1pwm);

	// Control from console
	while (true) {
		int input;
		std::cout << "Input speed: ";
		std::cin >> input;

		if (input > 0 && input <= 100) {
			motor.set(abs(input), direction::FORWARD);
		} else if (input < 0 && input >= -100) {
			motor.set(abs(input), direction::BACKWARD);
		} else {
			motor.stop();
		}
	}
}

void test::two_motor() {
	motor motor1(*chip, pin::m1in1, pin::m1in2, pin::m1pwm);
	motor motor2(*chip, pin::m2in1, pin::m2in2, pin::m2pwm);

	// Control from console
	while (true) {
		int input1, input2;
		std::cout << "Input speed 1: ";
		std::cin >> input1;
		std::cout << "Input speed 2: ";
		std::cin >> input2;

		if (input1 > 0 && input1 <= 100) {
			motor1.set(abs(input1), direction::FORWARD);
		} else if (input1 < 0 && input1 >= -100) {
			motor1.set(abs(input1), direction::BACKWARD);
		} else {
			motor1.stop();
		}

		if (input2 > 0 && input2 <= 100) {
			motor2.set(abs(input2), direction::FORWARD);
		} else if (input2 < 0 && input2 >= -100) {
			motor2.set(abs(input2), direction::BACKWARD);
		} else {
			motor2.stop();
		}
	}
}

void test::four_motor() {
	motor motor1(*chip, pin::m1in1, pin::m1in2, pin::m1pwm);
	motor motor2(*chip, pin::m2in1, pin::m2in2, pin::m2pwm);
	motor motor3(*chip, pin::m3in1, pin::m3in2, pin::m3pwm);
	motor motor4(*chip, pin::m4in1, pin::m4in2, pin::m4pwm);

	// Control from console
	while (true) {
		int input1, input2, input3, input4;
		std::cout << "Input speed 1: ";
		std::cin >> input1;
		std::cout << "Input speed 2: ";
		std::cin >> input2;
		std::cout << "Input speed 3: ";
		std::cin >> input3;
		std::cout << "Input speed 4: ";
		std::cin >> input4;

		if (input1 > 0 && input1 <= 100) {
			motor1.set(abs(input1), direction::FORWARD);
		} else if (input1 < 0 && input1 >= -100) {
			motor1.set(abs(input1), direction::BACKWARD);
		} else {
			motor1.stop();
		}

		if (input2 > 0 && input2 <= 100) {
			motor2.set(abs(input2), direction::FORWARD);
		} else if (input2 < 0 && input2 >= -100) {
			motor2.set(abs(input2), direction::BACKWARD);
		} else {
			motor2.stop();
		}

		if (input3 > 0 && input3 <= 100) {
			motor3.set(abs(input3), direction::FORWARD);
		} else if (input3 < 0 && input3 >= -100) {
			motor3.set(abs(input3), direction::BACKWARD);
		} else {
			motor3.stop();
		}

		if (input4 > 0 && input4 <= 100) {
			motor4.set(abs(input4), direction::FORWARD);
		} else if (input4 < 0 && input4 >= -100) {
			motor4.set(abs(input4), direction::BACKWARD);
		} else {
			motor4.stop();
		}
	}
}

void test::distance() {
	hc_sr04 sensor(*chip, pin::us_trig, pin::us_echo);

	// Write readings to console
	while (true) {
		uint64_t value = sensor.pulse();
		std::cout << value << std::endl;
		std::this_thread::sleep_for(std::chrono::milliseconds(50));
	}
}
