#include "hcsr04.hpp"

#include <chrono>
#include <mutex>
#include <stdexcept>
#include <thread>
#include <cstdint>
#include <gpiod.hpp>

#define GPIO_USER "robot2_sensor"

hc_sr04::hc_sr04(gpiod::chip &chip, uint32_t trig_pin, uint32_t echo_pin) :
	trig(chip.get_line(trig_pin)),
	echo(chip.get_line(echo_pin)) {
	// Set trig pin to output
	trig.request({
		.consumer = GPIO_USER,
		.request_type = gpiod::line_request::DIRECTION_OUTPUT,
		.flags = 0
	}, 0);
	
	// Set echo pin to rising edge input
	echo.request({
		.consumer = GPIO_USER,
		.request_type = gpiod::line_request::DIRECTION_INPUT,
		.flags = 0
	});
}

hc_sr04::~hc_sr04() {
	trig.release();
	echo.release();
}

#include <iostream>
uint64_t hc_sr04::pulse(uint64_t timeout_ns) {
	std::lock_guard<std::mutex> guard(busy);

	// Send pulse
	trig.set_value(1);
	std::this_thread::sleep_for(std::chrono::microseconds(10));
	trig.set_value(0);

	// Wait for pulse
	while (!echo.get_value()) {
		std::this_thread::sleep_for(std::chrono::microseconds(10));
	}

	// Get initial time
	auto initial = std::chrono::steady_clock::now();

	// Wait for pulse
	while (echo.get_value()) {
		std::this_thread::sleep_for(std::chrono::microseconds(10));
	}

	// Get final time
	auto final = std::chrono::steady_clock::now();

	// Compute & return delta
	return std::chrono::duration_cast<std::chrono::microseconds>(final - initial).count();
}

void hc_sr04::pulse_async(uint64_t timeout_ns, std::function<void(uint64_t)> &callback) {
	auto executor = [&](uint64_t timeout_ns, std::function<void(uint64_t)> callback) {
		auto result = pulse(timeout_ns);
		callback(result);
	};

	std::thread async(executor, timeout_ns, callback);
	async.detach();
}
