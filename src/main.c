#include <stdio.h>
#include <stdint.h>
#include <signal.h>

#include "driver/gpiochip.h"
#include "driver/motors.h"

#define AIN1 2u
#define AIN2 3u
#define PWMA 1u

static motor *test_motor = NULL;

void cleanup();

int main() {
	signal(SIGINT, &cleanup);
	signal(SIGQUIT, &cleanup);

	gpio_start();
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

	return 0;
}

void cleanup() {
	if (test_motor != NULL) {
		delete_motor(test_motor);
	}

	gpio_end();
}
