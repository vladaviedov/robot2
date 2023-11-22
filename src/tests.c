#include "tests.h"

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <signal.h>
#include <gpiod.h>

#include "driver/gpiochip.h"
#include "driver/pwm.h"
#include "driver/motors.h"

#define AIN1 17u
#define AIN2 27u
#define PWMA 18u

#define BIN1 22u
#define BIN2 23u
#define PWMB 24u

static struct gpiod_line *test_gpio1 = NULL;
static struct gpiod_line *test_gpio2 = NULL;
static struct gpiod_line *test_gpio3 = NULL;
static pwm_handle *test_pwm1 = NULL;
static pwm_handle *test_pwm2 = NULL;
static pwm_handle *test_pwm3 = NULL;
static motor *test_motor1 = NULL;
static motor *test_motor2 = NULL;

void cleanup_gpio();
void cleanup_pwm();
void cleanup_motor();
void cleanup_two_motor();

void gpio_test_routine(void) {
	gpio_start();
	signal(SIGINT, &cleanup_gpio);
	signal(SIGQUIT, &cleanup_gpio);

	test_gpio1 = gpiod_chip_get_line(gpio_chip, AIN1);
	gpiod_line_request_output(test_gpio1, "TEST", 0);
	test_gpio2 = gpiod_chip_get_line(gpio_chip, AIN2);
	gpiod_line_request_output(test_gpio2, "TEST", 0);
	test_gpio3 = gpiod_chip_get_line(gpio_chip, PWMA);
	gpiod_line_request_output(test_gpio3, "TEST", 0);

	while (1) {
		int32_t input;

		printf("Input value: ");
		scanf("%d", &input);

		if (input == 0) {
			gpiod_line_set_value(test_gpio1, 0);
			gpiod_line_set_value(test_gpio2, 0);
			gpiod_line_set_value(test_gpio3, 0);
		}
		if (input == 1) {
			gpiod_line_set_value(test_gpio1, 1);
			gpiod_line_set_value(test_gpio2, 1);
			gpiod_line_set_value(test_gpio3, 1);
		}
	}
}

void pwm_test_routine(void) {
	gpio_start();
	signal(SIGINT, &cleanup_pwm);
	signal(SIGQUIT, &cleanup_pwm);

	struct gpiod_line *gpio_line = gpiod_chip_get_line(gpio_chip, AIN1);
	gpiod_line_request_output(gpio_line, "TEST", 0);
	test_pwm1 = pwm_attach(gpio_line);

	gpio_line = gpiod_chip_get_line(gpio_chip, AIN2);
	gpiod_line_request_output(gpio_line, "TEST", 0);
	test_pwm2 = pwm_attach(gpio_line);

	gpio_line = gpiod_chip_get_line(gpio_chip, PWMA);
	gpiod_line_request_output(gpio_line, "TEST", 0);
	test_pwm3 = pwm_attach(gpio_line);

	while (1) {
		int32_t input;

		printf("Duty cycle: ");
		scanf("%d", &input);

		pwm_set(test_pwm1, input);
		pwm_set(test_pwm2, input);
		pwm_set(test_pwm3, input);
	}
}

void motor_test_routine(void) {
	gpio_start();
	signal(SIGINT, &cleanup_motor);
	signal(SIGQUIT, &cleanup_motor);

	test_motor1 = setup_motor(AIN1, AIN2, PWMA);

	while (1) {
		int32_t input;

		printf("Input speed: ");
		scanf("%d", &input);

		if (input > 0) {
			motor_set(test_motor1, abs(input), FORWARD);
		} else if (input < 0) {
			motor_set(test_motor1, abs(input), BACKWARD);
		} else {
			motor_stop(test_motor1);
		}

	}
}

void two_motor_test_routine(void) {
	gpio_start();
	signal(SIGINT, &cleanup_two_motor);
	signal(SIGQUIT, &cleanup_two_motor);

	test_motor1 = setup_motor(AIN1, AIN2, PWMA);
	test_motor2 = setup_motor(BIN1, BIN2, PWMB);

	while (1) {
		int32_t input1, input2;

		printf("Input speeds: ");
		scanf("%d %d", &input1, &input2);

		if (input1 > 0) {
			motor_set(test_motor1, abs(input1), FORWARD);
		} else if (input1 < 0) {
			motor_set(test_motor1, abs(input1), BACKWARD);
		} else {
			motor_stop(test_motor1);
		}

		if (input2 > 0) {
			motor_set(test_motor2, abs(input2), FORWARD);
		} else if (input2 < 0) {
			motor_set(test_motor2, abs(input2), BACKWARD);
		} else {
			motor_stop(test_motor2);
		}
	}
}

void cleanup_gpio() {
	gpiod_line_release(test_gpio1);
	gpiod_line_release(test_gpio2);
	gpiod_line_release(test_gpio3);
	gpio_end();
	exit(2);
}

void cleanup_pwm() {
	gpiod_line_release(pwm_detach(test_pwm1));
	gpiod_line_release(pwm_detach(test_pwm2));
	gpiod_line_release(pwm_detach(test_pwm3));
	gpio_end();
	exit(2);
}

void cleanup_motor() {
	delete_motor(test_motor1);
	gpio_end();
	exit(2);
}

void cleanup_two_motor() {
	delete_motor(test_motor1);
	delete_motor(test_motor2);
	gpio_end();
	exit(2);
}
