#include "pwm.h"

/**
 * @brief configura o PWM usando o timer TC1
 */
void pwm_init()
{
    // configuracao do Timer TC1 --> TIMER DO PWM
    TCCR1B |= ((0<<ICNC1) | (0<<ICES1));
    TCCR1B |= ((1<<WGM13) | (0<<WGM12));
    TCCR1B |= ((0<<CS12) | (0<<CS11)); 
    TCCR1B |=  (1<<CS10);
    TCCR1A |= ((0<<COM1A1) | (1<<COM1A0));
    TCCR1A |= ((0<<COM1B1) | (1<<COM1B0)); 
    TCCR1A |= ((1<<WGM11) | (0<<WGM10));

    // Equacao para Frequencia do PWM:       ICR1 = (f_osc)/(2*f_pwm);
	ICR1   = 640;                                   // valor TOP para f_pwm = 12 KHz
	OCR1A  = INITIAL_D;                             // D = %*ICR1
}

/**
 * @brief reset pwm and its control buffers.
 */
inline void pwm_reset(void)
{
    set_pwm_off();
    control.D_raw = control.D_raw_target = control.D = 0;
    control.I_raw = control.I_raw_target = control.I = 0;
}


/**
 * @brief computs duty-cycle for PWM
 */
inline void pwm_compute(void)
{
    if(control.D_raw < control.D_raw_target){           //!< law for D increasing
        if(pwm_d_clk_div++ >= PWM_D_MAX_DELTA){
            control.D_raw += PWM_D_DELTA;
            pwm_d_clk_div = 0;
        }
    }else if(control.D_raw >= control.D_raw_target){    //!< law for D decreasing
        control.D_raw = control.D_raw_target;
    }

    // converts to OCR1A range.
    control.D = (control.D_raw*PWM_D_LIN_MULT) >> PWM_D_LIN_DIV;

    // apply some threshhold saturation limits
    if(control.D > PWM_D_MAX_THRESHHOLD)        control.D = PWM_D_MAX;
    else if(control.D < PWM_D_MIN_THRESHHOLD)   control.D = PWM_D_MIN;
}

/**
 * @brief decreases pwm by 10% in case of mosfet fault detected by IR2127.
 */
inline void pwm_treat_fault(void)
{
    if(control.D_raw_target > 10)
        control.D_raw_target -= 6;      // -10%
}

/**
 *  @brief funcao conta o tempo em que o Duty cycle fica em zero
 *  @param registrador de 16 bits; nesse caso, do timer 1 (OCR1A)
 *  @ret  retorna 1 se registrador esta em zero por mais de um segundo (para
 *  freq = 35 Hz)
 */
uint8_t pwm_zero_width(uint16_t duty_cycle)
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
