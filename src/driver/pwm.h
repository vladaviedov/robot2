#pragma once

#include <stdint.h>
#include <gpiod.h>

typedef struct pwm_handle pwm_handle;

pwm_handle *pwm_attach(struct gpiod_line *pin);
struct gpiod_line *pwm_detach(pwm_handle *handle);
void pwm_set(pwm_handle *handle, uint32_t speed);
