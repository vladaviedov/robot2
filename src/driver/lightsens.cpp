#include "lightsens.hpp"

#include <cstdint>
#include <gpiod.hpp>

#define GPIO_USER "robot2_sensor"

light_sens::light_sens(gpiod::chip &chip, uint32_t input_pin) :
	input(chip.get_line(input_pin)) {
	// Set input pin to input
	input.request({
		.consumer = GPIO_USER,
		.request_type = gpiod::line_request::DIRECTION_INPUT,
		.flags = 0
	});
}

light_sens::~light_sens() {
	input.release();
}

int light_sens::read() const {
	return !input.get_value();
}
