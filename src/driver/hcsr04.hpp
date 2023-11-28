#pragma once

#include <functional>
#include <mutex>
#include <thread>
#include <cstdint>
#include <gpiod.hpp>

class hc_sr04 {

public:
	hc_sr04(gpiod::chip &chip, uint32_t trig_pin, uint32_t echo_pin);
	~hc_sr04();

	uint64_t pulse(uint64_t timeout_us);
	void pulse_async(uint64_t timeout_us, std::function<void(uint64_t)> &callback);

private:
	std::mutex busy; 
	const gpiod::line trig;
	const gpiod::line echo;
};
