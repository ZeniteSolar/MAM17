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

typedef enum state_machine{
    STATE_INITIALIZING,
    STATE_IDLE,
    STATE_RUNNING,
    STATE_ERROR,
} state_machine_t;

typedef union system_flags{
    struct{
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

// checks
void check_switches(void);
void check_idle_zero_pot(void);
void check_idle_current(void);
void check_idle_voltage(void);
void check_idle_temperature(void);
void check_running_current(void);
void check_running_voltage(void);
void check_running_temperature(void);

// tasks
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

extern uint8_t led_div;

// externs
extern uint8_t mean;
extern uint16_t D;
extern uint8_t fault_count;
extern void calc_d(uint8_t out);
//extern uint8_t total_errors;
extern uint8_t zero_width(uint16_t duty_cycle);

#endif /* ifndef _MACHINE_H_ */
