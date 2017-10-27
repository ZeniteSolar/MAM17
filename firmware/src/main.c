// coding: utf-8

#include "main.h"

int main(void)
{
    #ifdef USART_ON
        usart_init(MYUBRR,1,1);                         // inicializa a usart
        VERBOSE0_MSG(usart_send_string("\n\n\nUSART... OK!\n"));
    #endif

    #ifdef CAN_ON
        VERBOSE0_MSG(usart_send_string("CAN (125kbps)..."));
        can_init(BITRATE_125_KBPS);
        VERBOSE0_MSG(usart_send_string(" OK!\n"));
        VERBOSE0_MSG(usart_send_string("CAN filters..."));
        can_static_filter(can_filter);
        VERBOSE0_MSG(usart_send_string(" OK!\n"));
    #endif

    #ifdef ADC_ON
        VERBOSE0_MSG(usart_send_string("ADC..."));
        adc_init();
        VERBOSE0_MSG(usart_send_string(" OK!\n"));
    #endif

    #ifdef PWM_ON
        VERBOSE0_MSG(usart_send_string("PWM..."));
        pwm_init();
        VERBOSE0_MSG(usart_send_string(" OK!\n"));
    #endif 

    #ifdef SLEEP_ON 
        VERBOSE0_MSG(usart_send_string("SLEEP..."));
        sleep_init();
        VERBOSE0_MSG(usart_send_string(" OK!\n"));
    #endif

    #ifdef WATCHDOG_ON
        VERBOSE0_MSG(usart_send_string("WATCHDOG..."));
        wdt_init();
        VERBOSE0_MSG(usart_send_string(" OK!\n"));
    #endif

 	#ifdef MACHINE_ON
        VERBOSE0_MSG(usart_send_string("MACHINE..."));
		machine_init();
        VERBOSE0_MSG(usart_send_string(" OK!\n"));
	#endif

    sei();
	
	for(;;){
		#ifdef WATCHDOG_ON
            wdt_reset();
		#endif

        #ifdef MACHINE_ON
            machine_run();
        #endif

		#ifdef SLEEP_ON
            sleep_mode();
		#endif
	}
}


/**
 * @brief se em debug, fica chaveando os pinos de debugs até o reset do watchdog
 */
ISR(BADISR_vect)
{
    for(;;){
        VERBOSE0_MSG(usart_send_string("\nFATAL ERROR: BAD ISR."));
        #ifdef WATCHDOG_ON
            VERBOSE0_MSG(usart_send_string("WAITING FOR WATCHDOG TO RESET...\n"));
        #endif
        #ifdef DEBUG_ON
            DEBUG0;
            DEBUG1;
            _delay_ms(100);
        #endif
    }
}

