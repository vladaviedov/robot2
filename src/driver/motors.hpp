#pragma once

#include <cstdint>
#include <gpiod.hpp>

#include "pwm.hpp"

enum direction {
	FORWARD,
	BACKWARD
};

class motor {

public:
	motor(gpiod::chip &chip, uint32_t in1_pin, uint32_t in2_pin, uint32_t pwm_pin);
	~motor();

	void stop();
	void set(uint32_t speed, direction dir);

	inline void invert() {
		inverted = !inverted;
	}

private:
	const gpiod::line ain1;
	const gpiod::line ain2;
	pwm_worker pwm;
	bool inverted;

};
