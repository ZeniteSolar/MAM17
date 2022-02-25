/**
 * @file machine.h
 *
 * @defgroup MACHINE State Machine Module
 *
 * @brief Implements the main state machine of the system.
 *
 */

#ifndef MACHINE_H
#define MACHINE_H 

#include <avr/io.h>
#include <avr/wdt.h>

#include "conf.h"
#include "adc.h"
#include "usart.h"
#include "dbg_vrb.h"
#include "pwm.h"
#ifdef CAN_ON
#include "can.h"
#include "can_app.h"
#endif

typedef enum state_machine{
    STATE_INITIALIZING,
    STATE_CONTACTOR,
    STATE_IDLE,
    STATE_RUNNING,
    STATE_ERROR,
} state_machine_t;

typedef union system_flags{
    struct{
        uint8_t     motor_on        :1;
        uint8_t     dms             :1;
        uint8_t     pot_zero_width  :1;
        uint8_t     reverse         :1;
    };
    uint8_t   all__;
} system_flags_t;

typedef union error_flags{
    struct{
        uint8_t     overcurrent :1;
        uint8_t     overvoltage :1;
        uint8_t     overheat    :1;
        uint8_t     fault       :1;
        uint8_t     no_canbus   :1;
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
    uint8_t     fault;          // counts the faults from ir2127

}control_t;

typedef struct contactor{
    uint8_t message_sent;
    uint8_t message_received;
    uint8_t acknowledged;
    uint16_t timeout_clk_div;
    uint16_t motor_stop_clk_div;
}contactor_t;

typedef enum state_contactor{
    STATE_CONTACTOR_WAITING_MOTOR,
    STATE_CONTACTOR_SEND_REQUEST,
    STATE_CONTACTOR_WAITING_RESPONSE,
}state_contactor_t;

typedef enum contactor_request{
    CONTACTOR_REQUEST_TURN_OFF,
    CONTACTOR_REQUEST_SET_FORWARD,
    CONTACTOR_REQUEST_SET_REVERSE,
    CONTACTOR_REQUEST_UNKNOWN = 0xFF,
}contactor_request_t;

// machine checks
void check_idle_zero_pot(void);
void check_idle_current(void);
void check_idle_voltage(void);
void check_idle_temperature(void);
void check_running_current(void);
void check_running_voltage(void);
void check_running_temperature(void);
//void check_can(void);         // transfered to can_app.h
void check_pwm_fault(void);
void check_reverse(void);

// debug functions
void print_system_flags(void);
void print_error_flags(void);
void print_control(void);

// machine tasks
void task_initializing(void);
void task_change_contactor(void);
void task_idle(void);
void task_running(void);
void task_error(void);

// the machine itself
void machine_init(void);
void machine_run(void);
void set_state_error(void);
void set_state_initializing(void);
void set_state_contactor(void);
void set_state_idle(void);
void set_state_running(void);
void set_initial_state(void);

// machine variables
extern state_machine_t state_machine;
extern system_flags_t system_flags;
extern error_flags_t error_flags;
extern volatile uint8_t machine_clk;
extern uint8_t total_errors; // Contagem de ERROS

// pwm variables
extern uint8_t pwm_fault_count;
extern uint8_t check_pwm_fault_times;

// other variables
extern uint8_t led_clk_div;
extern control_t control;
extern state_contactor_t state_contactor;
extern contactor_t contactor;

#endif /* ifndef MACHINE_H */
