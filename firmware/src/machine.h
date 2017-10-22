/**
 * @file machine.h
 *
 * @defgroup MACHINE State Machine Module
 *
 * @brief Implements the main state machine of the system.
 *
 */

#ifndef _MACHINE_H_
#define _MACHINE_H_ 

#include <avr/io.h>
#include <avr/wdt.h>

#include "conf.h"
#include "adc.h"
#include "usart.h"
#include "dbg_vrb.h"
#include "can.h"
#include "can_app.h"

typedef enum state_machine{
    STATE_INITIALIZING,
    STATE_IDLE,
    STATE_RUNNING,
    STATE_ERROR,
} state_machine_t;

typedef union system_flags{
    struct{
        uint8_t     can_enabled     :1;
        uint8_t     on_off_switch   :1;
        uint8_t     dms_switch      :1;
        uint8_t     pot_zero_width  :1;
    };
    uint8_t   all;
} system_flags_t;

typedef union error_flags{
    struct{
        uint8_t     overcurrent :1;
        uint8_t     overvoltage :1;
        uint8_t     overheat    :1;
        uint8_t     fault       :1;
    };
    uint8_t   all;
}error_flags_t;

typedef struct control{
    uint8_t     D_raw;          // value from 0 to 255
    uint8_t     D_raw_target;   // value for target pwm, from 0 to 255
    uint16_t    D;              // value converted from 0 to TOP
    uint8_t     I_raw;          // value from 0 to 255
    uint8_t     I_raw_target;   // value for target pwm, from 0 to 255
    uint8_t     I;              // value of current in AMPS
    uint8_t     V;              // value of voltage in VOLTS
    uint8_t     R;              // value of angular velocity in RPMS
    uint8_t     T;              // value of temperature in CELCIUS DEGREES
}control_t;

control_t control;

// machine checks
void check_switches(void);
void check_idle_zero_pot(void);
void check_idle_current(void);
void check_idle_voltage(void);
void check_idle_temperature(void);
void check_running_current(void);
void check_running_voltage(void);
void check_running_temperature(void);
//void check_can(void);         // transfered to can_app.h
void check_pwm_fault(void);

// machine tasks
void task_initializing(void);
void task_idle(void);
void task_running(void);
void task_error(void);

// the machine itself
void machine_run(void);
void set_state_error(void);
void set_state_initializing(void);
void set_state_idle(void);
void set_state_running(void);

// machine variables
state_machine_t state_machine;
system_flags_t system_flags;
error_flags_t error_flags;
uint8_t total_errors;   // Contagem de ERROS

// pwm macros
#define set_pwm_duty_cycle(d)       OCR1A = d      //!< apply duty cycle 'd'
#define set_pwm_off()               set_pwm_duty_cycle(0)      //!< d = 0

// pwm functions
void pwm_reset(void);
void pwm_compute(void);
void pwm_treat_fault(void);
uint8_t pwm_zero_width(uint16_t duty_cycle);

// pwm variables
uint8_t pwm_d_clk_div;
uint8_t pwm_fault_count;
uint8_t check_pwm_fault_times;

// other variables
uint8_t led_clk_div;

// externs
//extern uint8_t total_errors;

#endif /* ifndef _MACHINE_H_ */
