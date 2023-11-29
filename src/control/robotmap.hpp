#pragma once

#include <cstdint>

#include "../driver/pinmap.hpp"

// Should be exactly like the schematic
namespace pin {
	// Motor 1 pins
	const uint32_t m1in1 = RASPI_11;
	const uint32_t m1in2 = RASPI_12;
	const uint32_t m1pwm = RASPI_10;

	// Motor 2 pins
	const uint32_t m2in1 = RASPI_13;
	const uint32_t m2in2 = RASPI_15;
	const uint32_t m2pwm = RASPI_19;

	// Motor 3 pins
	const uint32_t m3in1 = RASPI_29;
	const uint32_t m3in2 = RASPI_23;
	const uint32_t m3pwm = RASPI_21;

	// Motor 4 pins
	const uint32_t m4in1 = RASPI_31;
	const uint32_t m4in2 = RASPI_33;
	const uint32_t m4pwm = RASPI_35;

	// Ultrasonic pins
	const uint32_t us_trig = RASPI_38;
	const uint32_t us_echo = RASPI_40;

	// IR sensor pins
	const uint32_t ir1 = RASPI_22;
	const uint32_t ir2 = RASPI_24;
}
