#include "motors.h"

#include <stdint.h>
#include <stdlib.h>
#include <gpiod.h>

#include "gpiochip.h"

#define GPIO_USER "robot2_motors"

typedef struct motor {
	struct gpiod_line *ctrl1;
	struct gpiod_line *ctrl2;
	struct gpiod_line *pwm;
} motor;

typedef struct motor_grp {
	motor *first;
	motor *second;
} motor_grp;

motor *setup_motor(uint32_t ctrl1_pin, uint32_t ctrl2_pin, uint32_t pwm_pin) {
	motor *new_motor = malloc(sizeof(motor));

	new_motor->ctrl1 = gpiod_chip_get_line(gpio_chip, ctrl1_pin);
	new_motor->ctrl2 = gpiod_chip_get_line(gpio_chip, ctrl2_pin);
	new_motor->pwm = gpiod_chip_get_line(gpio_chip, pwm_pin);

	gpiod_line_request_output(new_motor->ctrl1, GPIO_USER, 0);
	gpiod_line_request_output(new_motor->ctrl2, GPIO_USER, 0);
	gpiod_line_request_output(new_motor->pwm, GPIO_USER, 0);

	return new_motor;
}

void motor_set(motor *motor, uint8_t value, direction dir) {
	if (dir) {
		gpiod_line_set_value(motor->ctrl1, 1);
		gpiod_line_set_value(motor->ctrl2, 0);
	} else {
		gpiod_line_set_value(motor->ctrl1, 0);
		gpiod_line_set_value(motor->ctrl2, 1);
	}

	// TODO: set pwm
}

void motor_stop(motor *motor) {
	gpiod_line_set_value(motor->ctrl1, 0);
	gpiod_line_set_value(motor->ctrl2, 0);
	// TODO: pwm
}

void delete_motor(motor *motor) {
	gpiod_line_release(motor->ctrl1);
	gpiod_line_release(motor->ctrl2);
	gpiod_line_release(motor->pwm);

	free(motor);
}

void motors_set(motor_grp *motors, uint8_t value, direction dir) {
	motor_set(motors->first, value, dir);
	motor_set(motors->second, value, dir);
}

void motors_stop(motor_grp *motors) {
	motor_stop(motors->first);
	motor_stop(motors->second);
}
