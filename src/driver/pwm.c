#include "pwm.h"

#include <stdatomic.h>
#include <pthread.h>
#include <unistd.h>
#include <gpiod.h>

// 500 Hz PWM -> 2000us / period
#define TOTAL_TIME 2000
#define TIME_PER_PERCENT (TOTAL_TIME / 100)

typedef struct pwm_handle {
	struct gpiod_line *line;
	atomic_int active;
	atomic_int duty_percent;
	pthread_t thread;
} pwm_handle;

void *pwm_executor(void *arg);

pwm_handle *pwm_attach(struct gpiod_line *pin) {
	pwm_handle *new_handle = malloc(sizeof(pwm_handle));

	new_handle->line = pin;
	new_handle->active = 1;
	new_handle->duty_percent = 0;
	pthread_create(&new_handle->thread, NULL, &pwm_executor, new_handle);

	return new_handle;
}

struct gpiod_line *pwm_detach(pwm_handle *handle) {
	// Kill thread
	handle->active = 0;
	usleep(TOTAL_TIME);

	// Destroy object
	struct gpiod_line *freed_line = handle->line;
	free(handle);

	return freed_line;
}

void pwm_set(pwm_handle *handle, uint32_t duty) {
	handle->duty_percent = duty;
}

/** Internal */

void *pwm_executor(void *arg) {
	pwm_handle *handle = (pwm_handle *)arg;

	while (handle->active) {
		if (handle->duty_percent != 0) {
			gpiod_line_set_value(handle->line, 1);
			usleep(TIME_PER_PERCENT * handle->duty_percent);
		}
		if (handle->duty_percent != 100) {
			gpiod_line_set_value(handle->line, 0);
			usleep(TIME_PER_PERCENT * (100 - handle->duty_percent));
		}
	}

	return NULL;
}
