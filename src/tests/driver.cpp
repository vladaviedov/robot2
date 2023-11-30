#include "driver.hpp"

#include <chrono>
#include <iostream>
#include <cmath>
#include <gpiod.hpp>
#include <signal.h>
#include <thread>

#include "../driver/pinmap.hpp"
#include "../driver/pwm.hpp"
#include "../driver/motors.hpp"
#include "../driver/hcsr04.hpp"
#include "../control/robotmap.hpp"

#define GPIO_USER "robot2_test"

std::unique_ptr<gpiod::chip> chip;

void test::driver_prepare() {
	chip = std::make_unique<gpiod::chip>("gpiochip0");
}

void test::gpio_out() {
	gpiod::line test_line = chip->get_line(RASPI_37);
	test_line.request({
		.consumer = GPIO_USER,
		.request_type = gpiod::line_request::DIRECTION_OUTPUT,
		.flags = 0
	}, 0);

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
	gpiod::line test_line = chip->get_line(RASPI_37);
	test_line.request({
		.consumer = GPIO_USER,
		.request_type = gpiod::line_request::DIRECTION_INPUT,
		.flags = 0
	});

	while (true) {
		int value = test_line.get_value();
		std::cout << value << std::endl;
		std::this_thread::sleep_for(std::chrono::milliseconds(50));
	}
}

void test::pwm() {
	pwm_worker test_pwm(*chip, RASPI_37);	

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
	motor motor(*chip, RASPI_11, RASPI_12, RASPI_10);

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
	motor motor1(*chip, RASPI_11, RASPI_12, RASPI_10);
	motor motor2(*chip, RASPI_13, RASPI_15, RASPI_19);

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
	motor motor1(*chip, RASPI_11, RASPI_12, RASPI_10);
	motor motor2(*chip, RASPI_13, RASPI_15, RASPI_19);
	motor motor3(*chip, RASPI_29, RASPI_23, RASPI_21);
	motor motor4(*chip, RASPI_31, RASPI_33, RASPI_35);

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

void test::event() {
	gpiod::line test_line = chip->get_line(RASPI_37);
	test_line.request({
		.consumer = GPIO_USER,
		.request_type = gpiod::line_request::EVENT_FALLING_EDGE,
		.flags = 0
	});

	while (true) {
		int res = test_line.event_wait(std::chrono::milliseconds(100));
		std::cout << "DONENE " << res << std::endl;
	}
}

void test::distance() {
	hc_sr04 sensor(*chip, pin::us_trig, pin::us_echo);

	while (true) {
		uint64_t value = sensor.pulse(0);
		std::cout << value << std::endl;
		std::this_thread::sleep_for(std::chrono::milliseconds(50));
	}
}
