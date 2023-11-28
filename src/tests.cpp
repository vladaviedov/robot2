#include "tests.hpp"

#include <iostream>
#include <gpiod.hpp>

#include "driver/pinmap.hpp"
#include "driver/pwm.hpp"

#define GPIO_USER "robot2_test"

gpiod::chip chip("gpiochip0");

void test::gpio_out() {
	gpiod::line test_line = chip.get_line(RASPI_37);
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

}

void test::pwm() {
	pwm_worker test_pwm(chip, RASPI_37);	

	while (true) {
		int input;
		std::cout << "Input duty: ";
		std::cin >> input;

		if (input >= 0 && input <= 100) {
			test_pwm.set_duty(input);
		}
	}
}

void test::motor() {

}

void test::two_motor() {

}

void test::four_motor() {

}

void test::distance() {

}
