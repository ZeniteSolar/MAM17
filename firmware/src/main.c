/**
 * 
 * @todo: adicionar um filtro janelado para cada adc
 *
 */

#include "main.h"
#include "can_filters.h"

volatile uint8_t CTRL_CLK = 0;       // CLOCK de controle (frequencia definida pelo timer2)

/**
 * @brief configura o PWM usando o timer TC1
 */
void pwm_init()
{
    // configuracao do Timer TC1 --> TIMER DO PWM
	TCCR1A |= 0b10100010;                            // modo PWM com fase corrigida, valor TOP = ICR1(registrador),   --> OBS: alterar bits DDRx do OC1A
    //TCCR1A  = ((1 << COM1A1) | (1 << COM1A0));
    //TCCR1B  = ((1 << CS10) | (1 << WGM13));

	TCCR1B |= 0b00010001;                            // prescaler = 1
    // Equacao para Frequencia do PWM:       ICR1 = (f_osc)/(2*f_pwm);
	ICR1   = 640;                                   // valor TOP para f_pwm = 12 KHz
	OCR1A  = INITIAL_D;                             // D = %*ICR1
    
}

/**
 * @brief configuracao do timer TC2  --> Timer de controle, timer2
 */
void ctrl_init()
{
	TCCR2A  =   (1 << WGM21) | (0 << WGM20)			// Timer 2 in Mode 2 = CTC (clear on compare)
			| (0 << COM2A1) | (0 << COM2A0)			// do nothing with OC2A
			| (0 << COM2B1) | (0 << COM2B0);		// do nothing with OC2B
	TCCR2B  =   (0 << WGM22)                        // Timer 0 in Mode 2 = CTC (clear on compare)
            | (0 << FOC0A) | (0 << FOC0B)           // dont force outputs
            | (1 << CS02)			                // clock enabled, prescaller = 1024
			| (1 << CS01) 
            | (1 << CS00);			                 
	OCR2A   =   222;			                    // Valor para igualdade de comparacao A para frequencia de 35 Hz
	TIMSK2 |=   (1 << OCIE2A);                      // Ativa a interrupcao na igualdade de comparação do TC2 com OCR2A
}    
 

/**
 * @brief configura os periféricos
 */
inline static void setup(void)
{

#ifdef USART_ON
    usart_init(MYUBRR,1,1);                         // inicializa a usart
    VERBOSE_MSG(usart_send_string("\n\n\nUSART... OK!\n"));
#endif

    VERBOSE_MSG(usart_send_string("I/O's..."));

    // configuracao dos pinos I/O
    set_bit(PWM_DDR, PWM);                      // PWM como saida
    set_bit(LED_DDR, LED);                      // LED como saída
    clr_bit(SWITCHES_DDR, DEAD_MAN_SWITCH);     // DEADMAN como entrada
    set_bit(SWITCHES_PORT, DEAD_MAN_SWITCH);    // DEADMAN com pull-up
    clr_bit(SWITCHES_DDR, ON_OFF_SWITCH);       // ON/OFF como entrada
    set_bit(SWITCHES_PORT, ON_OFF_SWITCH);      // ON/OFF com pull-up
    set_bit(FAULT_PORT, FAULT);                 // FAULT com pull-up
    VERBOSE_MSG(usart_send_string(" OK!\n"));

    VERBOSE_MSG(usart_send_string("External Interrupts..."));
    // Configuracoes da interrupcao externa para as chaves e a interrupcao externa por FAULT (IR2127)
    set_bit(PCMSK2, PCINT20);                   // DEADMAN com interrupcao
    set_bit(PCMSK2, PCINT21);                   // ON/OFF com interrupcao
    set_bit(PCMSK2, PCINT19);                   // FAULT (IR2127)
    set_bit(PCICR, PCIE2);                      // enables external interrupts for PCINT23~16
    set_bit(PCIFR, PCIF2);                      // clears external interrupt requests for PCINT23~16
    VERBOSE_MSG(usart_send_string(" OK!\n"));

#ifdef DEBUG_ON
    VERBOSE_MSG(usart_send_string("Debug I/O's..."));
    set_bit(DDRB, PB5);
    set_bit(DDRB, PB4);
    VERBOSE_MSG(usart_send_string(" OK!\n"));
#endif 

#ifdef ADC_ON
    VERBOSE_MSG(usart_send_string("ADC..."));
    adc_init();
    VERBOSE_MSG(usart_send_string(" OK!\n"));
#endif

#ifdef CTRL_ON
    VERBOSE_MSG(usart_send_string("CTRL..."));
	ctrl_init();
    VERBOSE_MSG(usart_send_string(" OK!\n"));
#endif


#ifdef SLEEP_ON 
    VERBOSE_MSG(usart_send_string("SLEEP..."));
    set_sleep_mode(SLEEP_MODE_IDLE);                // configura sleep com o modo IDLE
    VERBOSE_MSG(usart_send_string(" OK!\n"));
#endif
    
#ifdef CAN_ON
    VERBOSE_MSG(usart_send_string("CAN (125kbps)..."));
    can_init(BITRATE_125_KBPS);
    VERBOSE_MSG(usart_send_string(" OK!\n"));
    VERBOSE_MSG(usart_send_string("CAN filters..."));
    can_static_filter(can_filter);
    VERBOSE_MSG(usart_send_string(" OK!\n"));
#endif

#ifdef PWM_ON
    VERBOSE_MSG(usart_send_string("PWM..."));
    pwm_init();
    VERBOSE_MSG(usart_send_string(" OK!\n"));
#endif 

#ifdef WATCHDOG_ON
    VERBOSE_MSG(usart_send_string("WATCHDOG..."));
    wdt_init();
    VERBOSE_MSG(usart_send_string(" OK!\n"));
#endif
 
	sei();				                            // liga a chave geral das interrupcoes
                                               
}


/**
 * @brief main
 */
int main(void)
{
    system_flags.all = error_flags.all = 0;
    state_machine = STATE_INITIALIZING;

    setup();

    VERBOSE_MSG(usart_send_string("Hi! I am the PMDC Chopper version 2017 and I am here to serve you.\n"));
    VERBOSE_MSG(usart_send_string("If you are receiving this message it means that all the peripherals was sucessyfully initialized.\n"));
    VERBOSE_MSG(usart_send_string("I will start my machine state right now. The first stage is called INITIALIZING STATE and it checks if this system is safe and able to correctly operate.\n"));
    VERBOSE_MSG(usart_send_string("...\n"));
   
    for(;;){

        wdt_reset();                                // checkpoint: reset watchdog timer

        // Main Task runs at CTRL_CLK frequency (defined by TIMER2)
		if(CTRL_CLK) {
            machine_run();
            CTRL_CLK = 0;
        }
   
#ifdef SLEEP_ON 
        sleep_mode();           // entra em modo IDLE até a próxima interrupção
#endif

    }
}

/**
 * @brief ISR para ações de controle
 */
ISR(TIMER2_COMPA_vect)
{
    CTRL_CLK = 1;
}

/**
 * @brief se em debug, fica chaveando os pinos de debugs até o reset do watchdog
 */
ISR(BADISR_vect)
{   
    for(;;){
#ifdef DEBUG_ON
        DEBUG0;
        DEBUG1;
        VERBOSE_MSG(usart_send_string("BADISR ERROR! FIX IT NOW !!!\n"));
        _delay_ms(100);
#endif
    }
}

 /*
EMPTY_INTERRUPT(TIMER0_COMPB_vect);
EMPTY_INTERRUPT(TIMER0_OVF_vect);
*/
/*
EMPTY_INTERRUPT(TIMER1_CAPT_vect);
EMPTY_INTERRUPT(TIMER1_COMPA_vect);
EMPTY_INTERRUPT(TIMER1_COMPB_vect);
EMPTY_INTERRUPT(TIMER1_OVF_vect);
*/
/*
EMPTY_INTERRUPT(TIMER2_COMPB_vect);
EMPTY_INTERRUPT(TIMER2_OVF_vect);
*/
