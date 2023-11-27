#pragma once

#include <stdint.h>

typedef struct light_sens light_sens;

/**
 * @brief Setup light sensor.
 *
 * @param[in] pin - Data pin number.
 * @return New light sensor object.
 */
light_sens *setup_ls(uint32_t pin);

/**
 * @brief Read data from a light sensor.
 *
 * @param[in] ls - Sensor object.
 * @return If reflection true, else false.
 */
int ls_read(light_sens *ls);

/**
 * @brief Delete light sensor object and cleanup pins.
 *
 * @param[in] ls - Light sensor object.
 */
void delete_ls(light_sens *ls);
