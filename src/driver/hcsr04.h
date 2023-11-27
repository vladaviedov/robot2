#pragma once

#include <stdint.h>

typedef struct hc_sr04 hc_sr04;

hc_sr04 *setup_us(uint32_t trig_pin, uint32_t echo_pin);
uint64_t us_pulse(hc_sr04 *us, uint64_t timeout);
void us_pulse_async(hc_sr04 *us, uint64_t timeout, void (*cb)(uint64_t));
void delete_us(hc_sr04 *us);
