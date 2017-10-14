/**
 * @file main.h
 *
 * @defgroup MAIN Main Module
 *
 * @brief Headers for main.c
 *
 */

#ifndef _MAIN_H_
#define _MAIN_H_ 

#ifndef F_CPU
#define F_CPU 16000000UL    ///< Frequency of CPU
#endif /* ifndef F_CPU */

// BIBLIOTECAS AVR
#include <avr/io.h> 	    //definições do componente especificado
#include <util/delay.h>		//biblioteca para o uso das rotinas de _delay_ms e _delay_us()
#include <avr/interrupt.h>
#include <avr/eeprom.h>
#include <avr/pgmspace.h>   //para o uso do PROGMEM, gravação de dados na memória flash
#include <avr/sleep.h>      //para o uso do modo sleep
#include <avr/wdt.h>        //para o uso do Watchdog
#include <avr/power.h>

// BIBLIOTECAS
//#include "../lib/cbuf.h"
//#include "../lib/log2.h"
#include "../lib/bit_utils.h"

// CONFIGURACOES DO SISTEMA
#include "conf.h"

#include "dbg_vrb.h"        //para o uso das predefinicoes de debug e verbose

// MODULOS DO SISTEMA
#ifdef ADC_ON 
#include "adc.h"
#else
#pragma message("ADC: OFF!")
#endif  /*ifdef ADC_ON*/

#ifdef USART_ON
#include "usart.h"
#else
#pragma message("USART: OFF!")
#endif /*ifdef USART_ON*/

#ifdef MACHINE_ON
#include "machine.h"
#else
#pragma message("MACHINE: OFF!")
#endif /*ifdef MACHINE_ON*/

#ifdef WATCHDOG_ON
#include "watchdog.h"
#else
#pragma message("WATCHDOG: OFF!")
#endif /*ifdef WATCHDOG_ON*/

#ifdef CAN_ON
#include "can.h"
#else
#pragma message("CAN: OFF!")
#endif /*ifdef CAN_ON*/

#endif /* ifndef _MAIN_H_ */
