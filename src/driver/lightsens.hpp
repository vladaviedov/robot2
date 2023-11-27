#pragma once

#include <cstdint>
#include <gpiod.hpp>

class light_sens {

public:
	light_sens(gpiod::chip &chip, uint32_t input_pin);
	~light_sens();

	int read() const;

private:
	const gpiod::line input;

};
