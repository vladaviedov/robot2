/**
 * @file tests/driver.hpp
 * @brief Functions for testing hardware and driver code.
 */
#pragma once

namespace test {
	/**
	 * @brief Must call this before any other driver test.
	 */
	void driver_prepare();

	/**
	 * @brief Control GPIO output on pin 37.
	 */
	void gpio_out();

	/**
	 * @brief Control GPIO input on pin 37.
	 */
	void gpio_in();

	/**
	 * @brief Control PWM output on pin 37.
	 */
	void pwm();

	/**
	 * @brief Control one motor with M1 pinout.
	 */
	void one_motor();

	/**
	 * @brief Control two motors with M1, M2 pinouts.
	 */
	void two_motor();

	/**
	 * @brief Control four motors with M1-4 pinouts.
	 */
	void four_motor();

	/**
	 * @brief Continuously read in ultrasonic distance readings.
	 */
	void distance();
}
