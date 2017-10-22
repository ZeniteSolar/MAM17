/**
 * @file conf.h
 *
 * @defgroup CONF Configurations 
 *
 * @brief General configuration of the system.
 *
 */

#ifndef _CONF_H_
#define _CONF_H_

// CONFIGURACOES DE COMPILACAO
//#define DEBUG_ON
//#define VERBOSE_ON

// MODULES ACTIVATION
#define ADC_ON
#define USART_ON
#define PWM_ON
#define CTRL_ON
#define MACHINE_ON                  // this one is absolutly necessary
#define SLEEP_ON
#define WATCHDOG_ON
#define CAN_ON

// INPUT PINS DEFINITIONS
#define     DEAD_MAN_SWITCH         PD4
#define     ON_OFF_SWITCH           PD5
#define     SWITCHES_PORT           PORTD
#define     SWITCHES_PIN            PIND
#define     SWITCHES_DDR            DDRD
#define     FAULT_PORT              PORTD
#define     FAULT_PIN               PIND
#define     FAULT_DDR               DDRD
#define     FAULT                   PD3

// OUTPUT PINS DEFINITIONS
#define     PWM_PORT                PORTB
#define     PWM_PIN                 PINB
#define     PWM_DDR                 DDRB
#define     PWM                     PB1
#define     LED_PORT                PORTC
#define     LED_PIN                 PINC
#define     LED_DDR                 DDRC
#define     LED                     PC5

#define     cpl_led()               cpl_bit(LED_PORT, LED);
#define     set_led()               set_bit(LED_PORT, LED);
#define     clr_led()               clr_bit(LED_PORT, LED);

// PWM DEFINITIONS
#define INITIAL_D                   0   //!< float value from 0 to 1
#define PWM_D_DELTA                 1   //!< amount to increase
#define PWM_D_MAX_DELTA             2   //!< clock divisor
#define PWM_D_MIN                   0   //!< minimum D
#define PWM_D_MAX                   640 //!< maximum D
#define PWM_D_MIN_THRESHHOLD        6   //!< minimum D threshhold
#define PWM_D_MAX_THRESHHOLD        636 //!< maximum D threshhold
#define PWM_D_LIN_MULT              5   //!< this is A for D = (D*A) >> B
#define PWM_D_LIN_DIV               1   //!< this is B for D = (D*A) >> B

// tempo em que o potenciometro deve ficar zerado para que seja permitido ligar 
#define MIN_ZERO_WIDTH_TIMES        10     
// number of checks before reset the pwm fault counter.
#define CHECKS_BEFORE_RESET_FAULT_COUNTER 100
// maximum of consecutive faults before state an error
#define FAULT_COUNT_LIMIT           50

// CANBUS DEFINITIONS
#define SUPPORT_EXTENDED_CANID      0   // 11bits ID
#define SUPPORT_TIMESTAMPS          0   // timestamp not supported in atmega328p

#endif /* ifndef _CONF_H_ */
