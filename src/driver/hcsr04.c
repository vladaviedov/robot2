#include "hcsr04.h"

#include <stdint.h>
#include <pthread.h>
#include <time.h>
#include <unistd.h>
#include <gpiod.h>

#include "gpiochip.h"

#define GPIO_USER "robot2_sensor"

typedef struct hc_sr04 {
	struct gpiod_line *trig;
	struct gpiod_line *echo;
} hc_sr04;

typedef struct {
	hc_sr04 *sensor;
	uint64_t timeout;
	void (*cb)(uint64_t);
	pthread_t thread;
} async_op;

void *async_executor(void *arg);

hc_sr04 *setup_us(uint32_t trig_pin, uint32_t echo_pin) {
	hc_sr04 *new_us = malloc(sizeof(hc_sr04));

	new_us->trig = gpiod_chip_get_line(gpio_chip, trig_pin);
	new_us->echo = gpiod_chip_get_line(gpio_chip, echo_pin);

	gpiod_line_request_output(new_us->trig, GPIO_USER, 0);
	gpiod_line_request_rising_edge_events(new_us->echo, GPIO_USER);

	return new_us;
}

uint64_t us_pulse(hc_sr04 *us, uint64_t timeout) {
	// Trigger pulse
	gpiod_line_set_value(us->trig, 1);
	usleep(10);
	gpiod_line_set_value(us->trig, 0);

	struct timespec initial;
	clock_gettime(CLOCK_MONOTONIC_RAW, &initial);

	// Get pulse
	struct timespec delay = {
		.tv_sec = 0,
		.tv_nsec = timeout * 1000
	};
	gpiod_line_event_wait(us->echo, &delay);

	struct timespec final;
	clock_gettime(CLOCK_MONOTONIC_RAW, &final);

	// Calculate delta
	if (final.tv_sec > initial.tv_sec) {
		final.tv_nsec += 1e9;
	}
	return (final.tv_nsec - initial.tv_nsec) / 1000;
}

void us_pulse_async(hc_sr04 *us, uint64_t timeout, void (*cb)(uint64_t)) {
	async_op *op = malloc(sizeof(async_op));

	op->sensor = us;
	op->timeout = timeout;
	op->cb = cb;
	pthread_create(&op->thread, NULL, &async_executor, op);
}

void delete_us(hc_sr04 *us) {
	if (us == NULL) {
		return;
	}

	gpiod_line_release(us->trig);
	gpiod_line_release(us->echo);
	free(us);
}

/** Internal */

void *async_executor(void *arg) {
	async_op *op = (async_op *)arg;

	uint64_t pulse = us_pulse(op->sensor, op->timeout);
	op->cb(pulse);

	return NULL;
}
