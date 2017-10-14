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

// ATIVACAO DOS MODULOS
#define ADC_ON
#define USART_ON
#define PWM_ON
#define CTRL_ON
#define MACHINE_ON
#define SLEEP_ON
#define WATCHDOG_ON
#define CAN_ON

// DEFINICOES DOS PINOS DE ENTRADA
#define     DEAD_MAN_SWITCH         PD4
#define     ON_OFF_SWITCH           PD5
#define     SWITCHES_PORT           PORTD
#define     SWITCHES_PIN            PIND
#define     SWITCHES_DDR            DDRD
#define     FAULT_PORT              PORTD
#define     FAULT_PIN               PIND
#define     FAULT_DDR               DDRD
#define     FAULT                   PD3

// DEFINICOES DOS PINOS DE SAIDA
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

// DEFINICOES DOS ADCS
//#define     raw_temperature         raw_adc[ADC0]
//#define     raw_pot                 raw_adc[ADC1]
//#define     raw_motor_current       raw_adc[ADC2]
//#define     raw_motor_voltage       raw_adc[ADC3]
#define     avg_pot                 avg_adc0

// DEFINICOES DO PWM
#define INITIAL_D 0     // float value from 0 to 1

// tempo em que o potenciometro deve ficar zerado para que seja permitido ligar 
#define MIN_ZERO_WIDTH_TIMES        10     

// maximo de faltas
#define FAULT_COUNT_LIMIT           50

// -----------------------------------------------------------------------------
/* Global settings for building the can-lib and application program.
 *
 * The following two #defines must be set identically for the can-lib and
 * your application program. They control the underlying CAN struct. If the
 * settings disagree, the underlying CAN struct will be broken, with
 * unpredictable results.
 * If can.h detects that any of the #defines is not defined, it will set them
 * to the default values shown here, so it is in your own interest to have a
 * consistent setting. Ommiting the #defines in both can-lib and application
 * program will apply the defaults in a consistent way too.
 *
 * Select if you want to use 29 bit identifiers.
 */
#define SUPPORT_EXTENDED_CANID  1

/* Select if you want to use timestamps.
 * Timestamps are sourced from a register internal to the AT90CAN.
 * Selecting them on any other controller will have no effect, they will
 * be 0 all the time.
 */
#define SUPPORT_TIMESTAMPS      0



#endif /* ifndef _CONF_H_ */
