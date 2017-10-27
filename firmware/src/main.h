/**
 * @file main.h
 *
 * @defgroup MAIN Main Module
 *
 * @brief Headers for main.c
 *
 */
#ifndef MAIN_H_
#define MAIN_H_ 

#ifndef F_CPU
#define F_CPU 16000000UL    ///< Frequency of CPU
#endif /* ifndef F_CPU */

#include <avr/io.h>
#include <avr/pgmspace.h>

// MODULOS DO SISTEMA
#include "conf.h"

//#if defined(DEBUG_ON) || defined(VERBOSE_ON)
#include "dbg_vrb.h"
//#pragma message("VERBOSE OR DEBUG: ON!")
//#endif  /*ifdef ADC_ON*/

#ifdef USART_ON
#include "usart.h"
#else
#pragma message("USART: OFF!")
#endif /*ifdef USART_ON*/

#ifdef CAN_ON
#include "can.h"

const uint8_t can_filter[] PROGMEM = 
{
	// Group 0
	MCP2515_FILTER(0),				// Filter 0
	MCP2515_FILTER(0),				// Filter 1
	
	// Group 1
	MCP2515_FILTER(0),		// Filter 2
	MCP2515_FILTER(0),		// Filter 3
	MCP2515_FILTER(0),		// Filter 4
	MCP2515_FILTER(0),		// Filter 5
	
	MCP2515_FILTER(0),				// Mask 0 (for group 0)
	MCP2515_FILTER(0),		// Mask 1 (for group 1)
};

#include "can_app.h"

#else
#pragma message("CAN: OFF!")
#endif /*ifdef CAN_ON*/

#ifdef ADC_ON 
#include "adc.h"
#else
#pragma message("ADC: OFF!")
#endif  /*ifdef ADC_ON*/

#ifdef MACHINE_ON
#include "machine.h"
#else
#pragma message("MACHINE: OFF!")
#endif /*ifdef MACHINE_ON*/

#ifdef PWM_ON
#include "pwm.h"
#else
#pragma message("PWM: OFF!")
#endif /*ifdef PWM*/

#ifdef WATCHDOG_ON
#include "watchdog.h"
#else
#pragma message("WATCHDOG: OFF!")
#endif /*ifdef WATCHDOG_ON*/

#ifdef SLEEP_ON
#include "sleep.h"
#else
#pragma message("SLEEP: OFF!")
#endif /*ifdef SLEEP_ON*/

void can_app_print_msg(can_t *msg);

#endif /* ifndef MAIN_H_ */