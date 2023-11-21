#include "gpiochip.h"

#include <stdint.h>
#include <gpiod.h>

#define CHIPNAME "gpiochip0"

struct gpiod_chip *gpio_chip = NULL;

void gpio_start(void) {
	gpio_chip = gpiod_chip_open_by_name(CHIPNAME);
}

void gpio_end(void) {
	if (gpio_chip != NULL) {
		gpiod_chip_close(gpio_chip);
	}
}
