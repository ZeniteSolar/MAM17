/**
 * @file pwm.h
 *
 * @defgroup PWM Module
 *
 * @brief This module implements a simple PWM
 *
 */

#ifndef PWM_H_
#define PWM_H_

#include "avr/io.h"
#include "avr/interrupt.h"
#include "conf.h"
#include "machine.h"

// // pwm macros
// #define set_pwm_duty_cycle(d)       OCR1A = d      //!< apply duty cycle 'd'
// #define set_pwm_off()               set_pwm_duty_cycle(0)      //!< d = 0

// pwm functions
void pwm_init(void);
// void pwm_reset(void);
// void pwm_compute(void);
// void pwm_treat_fault(void);
// uint8_t pwm_zero_width(uint16_t duty_cycle);

// // pwm variables
// uint8_t pwm_d_clk_div;

// extern state_machine_t state_machine;
// extern system_flags_t system_flags;
// extern error_flags_t error_flags;

#endif /* ifndef PWM_H_ */