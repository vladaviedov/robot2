#pragma once

#include <cstdint>
#include <atomic>
#include <memory>
#include <thread>
#include <gpiod.hpp>

class pwm_worker {

public:
	pwm_worker(gpiod::chip &chip, uint32_t pin);
	~pwm_worker();
	
	inline void set_duty(uint32_t percent) {
		duty_percent = percent;
	}

private:
	uint32_t duty_percent = 0;
	std::unique_ptr<std::thread> pwm_thread;
	std::atomic_bool active = true;
	const gpiod::line line;

};
