/**
 * 
 * @todo: adicionar um filtro janelado para cada adc
 *
 */

#include "main.h"

uint8_t CTRL_CLK = 0;       // CLOCK de controle (frequencia definida pelo timer2)
uint16_t D = 0;             // DUTY CYCLE do PWM
uint8_t led_div = 0;        // CLOCK division for led control
uint8_t fault_count = 0;    // fault counter;

/*
 *  @brief funcao conta o tempo em que o Duty cycle fica em zero
 *  @param registrador de 16 bits; nesse caso, do timer 1 (OCR1A)
 *  @ret  retorna 1 se registrador esta em zero por mais de um segundo (para
 *  freq = 35 Hz)
 */
uint8_t zero_width(uint16_t duty_cycle)
{   
    static uint8_t times = 0;
    uint8_t one_sec = 0;
    
    if(!duty_cycle) times += 1; 
    else times = one_sec = 0;

    if(times >= MIN_ZERO_WIDTH_TIMES){ 
        one_sec = 1;
        times = 0;
    }
    return one_sec;
}


/**
 * @brief Calcula e aplica PWM a partir da entrada do ADC
 *      min: 4, max 128 elements
 *      it is taking 6us using mean of 4 elements
 *      it is taking 11.2us using mean of 8 elements
 *      it is taking 17.5us using mean of 16 elements
 *      it is taking 31.9us using mean of 32 elements
 *      it is taking 64.7us using mean of 64 elements
 *      it is taking 119.6us using mean of 128 elements
 */
/*inline void calc_d(void)
{   
    //DEBUG0;
    uint16_t D = (ma_adc0()*5) >> 1;        // duty cycle D = ADCH*5/2
    if(D > 634) D = 640;                    // limite maximo para não ficar em 99%
    else if(D < 6) D = 0;                   // limite minimo para não ficar em 1%
    OCR1A = D;                              // aplica duty cycle D
    //DEBUG0;
}*/

//uint16_t D = 0;
#define eps             20
#define k2_             1                   // se maior ou igual a 7, usar uint32_t
#define k_              1                   // se maior ou igual a 7, usar uint32_t
#define CALC_D_DIV_COUNTS 2
volatile uint8_t calc_d_div = 0;
volatile uint32_t D_raw_target;
inline void calc_d(uint8_t out)
{   
    static uint32_t D_raw = 0;

    if(!out){
        D = D_raw = D_raw_target = 0;
    }else if(out == 100){
        if(D_raw_target > 10) {
            D_raw_target = D_raw_target -6;
        }
    }else{ 
        D_raw_target = ma_adc0();
    }

    // y = (1 -e^-x), x>=0:
    //D_raw = ((D_raw << k_ ) +(D_raw_target -D_raw) ) >> k_;
    //D_raw = ((D_raw << k_ ) +((D_raw_target -D_raw) << k2_ ) ) >> k_;
    //D_raw = D_raw_target;
    
    if(D_raw < D_raw_target){
        if(calc_d_div++ >= CALC_D_DIV_COUNTS){
            D_raw += 1;
            calc_d_div = 0;
        }
    }else if((D_raw >= D_raw_target)){
        //D_raw = ((D_raw << k_ ) +(D_raw_target -D_raw )) >> k_;
        D_raw = D_raw_target;
    }

    D = (D_raw*5) >> 1;                         // converte valores

    if(D > 636) D = 640;                        // limite maximo para não ficar em 99%
    else if(D < 6) D = 0;                       // limite minimo para não ficar em 1% 

    

    OCR1A = D;                                  // aplica duty_cycle
}

/**
 * @brief configura o PWM usando o timer TC1
 */
void pwm_init()
{
    // configuracao do Timer TC1 --> TIMER DO PWM
	TCCR1A = 0b10100010;                            // modo PWM com fase corrigida, valor TOP = ICR1(registrador),   --> OBS: alterar bits DDRx do OC1A
	TCCR1B = 0b00010001;                            // prescaler = 1
    //Equacao para Frequencia do PWM:       ICR1 = (f_osc)/(2*f_pwm);
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

    // configuracao dos pinos I/O
    set_bit(PWM_DDR, PWM);                      // PWM como saida
    set_bit(LED_DDR, LED);                      // LED como saída
    clr_bit(SWITCHES_DDR, DEAD_MAN_SWITCH);     // DEADMAN como entrada
    set_bit(SWITCHES_PORT, DEAD_MAN_SWITCH);    // DEADMAN com pull-up
    clr_bit(SWITCHES_DDR, ON_OFF_SWITCH);       // ON/OFF como entrada
    set_bit(SWITCHES_PORT, ON_OFF_SWITCH);      // ON/OFF com pull-up
    set_bit(FAULT_PORT, FAULT);                 // FAULT com pull-up


    // Configuracoes da interrupcao externa para as chaves e a interrupcao externa por FAULT (IR2127)
    set_bit(PCMSK2, PCINT20);                   // DEADMAN com interrupcao
    set_bit(PCMSK2, PCINT21);                   // ON/OFF com interrupcao
    set_bit(PCMSK2, PCINT19);                   // FAULT (IR2127)
    set_bit(PCICR, PCIE2);                      // enables external interrupts for PCINT23~16
    set_bit(PCIFR, PCIF2);                      // clears external interrupt requests for PCINT23~16


#ifdef DEBUG_ON
    set_bit(DDRB, PB5);
    set_bit(DDRB, PB4);
#endif 

#ifdef ADC_ON
    adc_init();
#endif

#ifdef PWM_ON
    pwm_init();
#endif

#ifdef CTRL_ON
	ctrl_init();
#endif

#ifdef USART_ON
    usart_init(MYUBRR,1,1);                         // inicializa a usart
#endif

#ifdef WATCHDOG_ON
    wdt_init();
#endif

#ifdef SLEEP_ON 
    set_sleep_mode(SLEEP_MODE_IDLE);                // configura sleep com o modo IDLE
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

    //rb_in = ring_buffer_create(INPUT_BUFFER_SIZE);
    //rb_out = ring_buffer_create(OUTPUT_BUFFER_SIZE);

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
