#pragma once

#include <stdint.h>
#include <gpiod.h>

extern struct gpiod_chip *gpio_chip;

void gpio_start(void);
void gpio_end(void);
