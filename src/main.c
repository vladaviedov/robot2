#include <stdio.h>
#include <stdint.h>
#include <signal.h>

#include <gpiod.h>

#include "driver/gpiochip.h"
#include "driver/pwm.h"
#include "driver/motors.h"

#define AIN1 2u
#define AIN2 3u
#define PWMA 1u

static struct gpiod_line *test_gpio = NULL;
static pwm_handle *test_pwm = NULL;
static motor *test_motor = NULL;

void cleanup_gpio();
void cleanup_pwm();
void cleanup_motor();

void gpio_test_routine(void);
void pwm_test_routine(void);
void motor_test_routine(void);

int main() {
	gpio_start();
	gpio_test_routine();
	return 0;
}

void cleanup_gpio() {
	gpiod_line_release(test_gpio);
	gpio_end();
}

void cleanup_pwm() {
	gpiod_line_release(pwm_detach(test_pwm));
	gpio_end();
}

void cleanup_motor() {
	delete_motor(test_motor);
	gpio_end();
}

void gpio_test_routine(void) {
	signal(SIGINT, &cleanup_gpio);
	signal(SIGQUIT, &cleanup_gpio);

	test_gpio = gpiod_chip_get_line(gpio_chip, AIN1);
	gpiod_line_request_output(test_gpio, "TEST", 0);

	while (1) {
		int32_t input;

		printf("Input value: ");
		scanf("%d\n", &input);

		if (input == 0) {
			gpiod_line_set_value(test_gpio, 0);
		}
		if (input == 1) {
			gpiod_line_set_value(test_gpio, 1);
		}
	}
}

void pwm_test_routine(void) {
	signal(SIGINT, &cleanup_pwm);
	signal(SIGQUIT, &cleanup_pwm);

	struct gpiod_line *gpio_line = gpiod_chip_get_line(gpio_chip, PWMA);
	gpiod_line_request_output(gpio_line, "TEST", 0);
	test_pwm = pwm_attach(gpio_line);

	while (1) {
		int32_t input;

		printf("Duty cycle: ");
		scanf("%d\n", &input);

		pwm_set(test_pwm, input);
	}
}

void motor_test_routine(void) {
	signal(SIGINT, &cleanup_motor);
	signal(SIGQUIT, &cleanup_motor);

	test_motor = setup_motor(AIN1, AIN2, PWMA);

	while (1) {
		int32_t input;

		printf("Input speed: ");
		scanf("%d\n", &input);

		if (input > 0) {
			motor_set(test_motor, (uint8_t)input, FORWARD);
		} else if (input < 0) {
			motor_set(test_motor, (uint8_t)input, BACKWARD);
		} else {
			motor_stop(test_motor);
		}
	}
}
