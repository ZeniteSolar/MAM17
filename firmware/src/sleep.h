/**
 * @file watchdog.h
 *
 * @defgroup SLEEP Sleep Module
 *
 * @brief A simple sleep module
 *
 */

#ifndef SLEEP_H
#define SLEEP_H 

#include <avr/io.h>
#include <avr/sleep.h>

void sleep_init(void);

void sleep_init(void)
{
	#pragma GCC diagnostic ignored "-Wconversion"
		set_sleep_mode(SLEEP_MODE_IDLE);
	#pragma GCC diagnostic pop
}

#endif /* ifndef SLEEP_H */
