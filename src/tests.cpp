#include "tests.hpp"

#include <iostream>
#include <gpiod.hpp>
#include <signal.h>

#include "driver/pinmap.hpp"
#include "driver/pwm.hpp"
#include "driver/motors.hpp"

#define GPIO_USER "robot2_test"

gpiod::chip chip("gpiochip0");
bool running = true;

void stop_test(int code) {
	running = false;
}

void test::gpio_out() {
	signal(SIGINT, stop_test);
	signal(SIGQUIT, stop_test);

	gpiod::line test_line = chip.get_line(RASPI_37);
	test_line.request({
		.consumer = GPIO_USER,
		.request_type = gpiod::line_request::DIRECTION_OUTPUT,
		.flags = 0
	}, 0);

	while (running) {
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

}

void test::pwm() {
	signal(SIGINT, stop_test);
	signal(SIGQUIT, stop_test);

	pwm_worker test_pwm(chip, RASPI_37);	

	while (running) {
		int input;
		std::cout << "Input duty: ";
		std::cin >> input;

		if (input >= 0 && input <= 100) {
			test_pwm.set_duty(input);
		}
	}
}

void test::one_motor() {
	signal(SIGINT, stop_test);
	signal(SIGQUIT, stop_test);

	motor motor(chip, RASPI_11, RASPI_7, RASPI_5);

	while (running) {
		int input;
		std::cout << "Input speed: ";
		std::cin >> input;

		if (input > 0 || input <= 100) {
			motor.set(input, direction::FORWARD);
		} else if (input < 0 || input >= -100) {
			motor.set(input, direction::BACKWARD);
		} else {
			motor.stop();
		}
	}
}

void test::two_motor() {

}

void test::four_motor() {

}

void test::distance() {

}
