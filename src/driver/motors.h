#pragma once

#include <stdint.h>
#include <wctype.h>

typedef enum {
	FORWARD,
	BACKWARD
} direction;

typedef struct motor motor;
typedef struct motor_grp motor_grp;

/** Motor functions */

/**
 * @brief Setup motor.
 *
 * @param[in] ctrl1_pin - First control pin (XIN1)
 * @param[in] ctrl2_pin - Second control pin (XIN2)
 * @param[in] pwm_pin - PWM pin (PWMX)
 * @return New motor object.
 */
motor *setup_motor(uint32_t ctrl1_pin, uint32_t ctrl2_pin, uint32_t pwm_pin);

/**
 * @brief Set motor speed value.
 *
 * @param[in] motor - Motor object.
 * @param[in] value - Motor speed.
 * @param[in] dir - Motor direction.
 */
void motor_set(motor *motor, uint8_t value, direction dir);

/**
 * @brief Stop motor.
 *
 * @param[in] motor - Motor object.
 */
void motor_stop(motor *motor);

/**
 * @brief Delete motor object and clean up pins.
 *
 * @param[in] motor - Motor object.
 */
void delete_motor(motor *motor);

/** Motor group functions */

/**
 * @brief Form a new motor group.
 *
 * @param[in] first - First motor.
 * @param[in] second - Second motor.
 * @return New motor group.
 */
motor_grp *group_motors(motor *first, motor *second);

/**
 * @brief Set motor group to speed.
 *
 * @param[in] motors - Motor group object.
 * @param[in] value - Motor speed.
 * @param[in] dir - Motor direction.
 */
void motors_set(motor_grp *motors, uint8_t value, direction dir);

/**
 * @brief Stop motor group.
 *
 * @param[in] motors - Motor group object.
 */
void motors_stop(motor_grp *motors);

/**
 * @brief Disassociate motor group.
 *
 * @param[in] group - Motor group object.
 */
void delete_motor_group(motor_grp *group);
