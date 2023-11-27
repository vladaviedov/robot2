#include "lightsens.h"

#include <stdint.h>
#include <gpiod.h>

#include "gpiochip.h"

#define GPIO_USER "robot2_sensor"

typedef struct light_sens {
	struct gpiod_line *pin;
} light_sens;

light_sens *setup_ls(uint32_t pin) {
	light_sens *new_ls = malloc(sizeof(light_sens));
	
	new_ls->pin = gpiod_chip_get_line(gpio_chip, pin);
	gpiod_line_request_input(new_ls->pin, GPIO_USER);

	return new_ls;
}

int ls_read(light_sens *ls) {
	if (ls == NULL) {
		return -1;
	}

	int result = gpiod_line_get_value(ls->pin);
	if (result == -1) {
		return -1;
	}

	return !result;
}

void delete_ls(light_sens *ls) {
	if (ls == NULL) {
		return;
	}

	gpiod_line_release(ls->pin);
	free(ls);
}
