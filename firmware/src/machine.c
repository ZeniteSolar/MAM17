#include "machine.h"

/*
 * to-do:
 *      - modularize state sinalization
 *      - 
 *
 */

/**
 * @brief checks if the switches updating system flags
 */
inline void check_switches(void)
{
    VERBOSE_MSG(usart_send_string("Motor Switch: "));
    if(bit_is_clear(SWITCHES_PIN, ON_OFF_SWITCH)){
        VERBOSE_MSG(usart_send_string("On.  "));
        system_flags.on_off_switch = 1;
    }else{
        VERBOSE_MSG(usart_send_string("Off. "));
        system_flags.on_off_switch = 0;
    }
    
    VERBOSE_MSG(usart_send_string("Deadman' Switch: "));
    if(bit_is_set(SWITCHES_PIN, DEAD_MAN_SWITCH)){
        VERBOSE_MSG(usart_send_string("On.  "));
        system_flags.dms_switch = 1;   
    }else{
        VERBOSE_MSG(usart_send_string("Off. "));
        system_flags.dms_switch = 0;   
    }
}

/**
 * @brief prints all known information about a given can_t msg
 * @param *msg is a pointer to the can_t message 
 */
void can_print_msg(can_t *msg)
{
    usart_send_string("ID: ");
    usart_send_uint16(msg->id);
    usart_send_string("F: ");
    usart_send_uint16(msg->flags.rtr);
    usart_send_string("L: ");
    usart_send_uint16(msg->length);
    usart_send_string("D: ");
    for(uint8_t i = 0; i< msg->length; i++){
        usart_send_uint16(msg->data[i]);
        usart_send_char(' ');
    }
    usart_send_char('\n');
}

/**
 * @brief checks if the pot is zeroed when idle.
 */
inline void check_idle_zero_pot(void)
{
    VERBOSE_MSG(usart_send_string("Potentiometer: "));
    if(pwm_zero_width(ma_adc0())){  // computa a media
        VERBOSE_MSG(usart_send_string("ok.  "));
        system_flags.pot_zero_width = 1;   
    }else{
        VERBOSE_MSG(usart_send_string("Nok. "));
        system_flags.pot_zero_width = 0;
    } 
}

/**
 * @brief checks the quantity of the faults.
 */
inline void check_pwm_fault(void)
{
    if(pwm_fault_count >= FAULT_COUNT_LIMIT){
        error_flags.fault = 1;
        check_pwm_fault_times = 0;
        set_state_error();
    }else if(check_pwm_fault_times++ > CHECKS_BEFORE_RESET_FAULT_COUNTER){
        pwm_fault_count = 0;    
    }
}

/**
 * @brief checks the buffer and waits it fills up
 */
inline void check_buffers(void)
{
    VERBOSE_MSG(usart_send_string("Checking buffers..."));
    while(!CBUF_IsFull(cbuf_adc0));
    VERBOSE_MSG(usart_send_string(" \t\tdone.\n")); 
}

/**
 * @brief checks if the current level is ok for IDLE state
 */
inline void check_idle_current(void)
{
    VERBOSE_MSG(usart_send_string("Checking motor current levels..."));
    //if(motor_current <= MAXIMUM_OFF_CURRENT);
        VERBOSE_MSG(usart_send_string(" \t\tdone.\n"));
    //else error_flag: motor_current at off_state bigger than it should be. 
}

/**
 * @brief checks if the voltage level is ok for IDLE state
 */
inline void check_idle_voltage(void)
{
    VERBOSE_MSG(usart_send_string("Checking motor voltage levels..."));
    //if(output_voltage <= MAXIMUM_OFF_VOLTAGE);
        VERBOSE_MSG(usart_send_string(" \t\tdone.\n"));
    //else error_flag: motor_voltage at off_state bigger than it should be.
}

/**
 * @brief checks if the temperature level is ok for IDLE state
 */
inline void check_idle_temperature(void)
{
    VERBOSE_MSG(usart_send_string("Checking motor temperature levels..."));
    //if(temperature <= MAXIMUM_OFF_TEMPERATURE);
        VERBOSE_MSG(usart_send_string(" \t\tdone.\n"));
    //else error_flag: motor_temperature at off_state bigger than it should be.
}
 

/**
 * @brief checks if the current level is ok for running state
 */
inline void check_running_current(void)
{
    //
}

/**
 * @brief checks if the voltage level is ok for running state
 */
inline void check_running_voltage(void)
{

}

/**
 * @brief checks if the temperature level is ok for running state
 */
inline void check_running_temperature(void)
{
    
}
 
/**
 * @brief set error state
 */
inline void set_state_error(void)
{
    state_machine = STATE_ERROR;
}

/**
* @brief set initializing state
*/ 
inline void set_state_initializing(void)
{
    state_machine = STATE_INITIALIZING;
}

/**
* @brief set idle state
*/ 
inline void set_state_idle(void)
{
    state_machine = STATE_IDLE;
}

/**
* @brief set running state
*/ 
inline void set_state_running(void)
{
    pwm_reset();
    state_machine = STATE_RUNNING;
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

/*
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


/**
 * @brief Checks if the system is OK to run:
 *  - all ring_buffers needed to be full
 *  - checks the current
 *  - checks the voltage
 *  - checks the temperature
 *
 */
inline void task_initializing(void)
{
    set_led();
    set_pwm_off();
    pwm_fault_count = 0;

    check_can();
    if(system_flags.can_enabled){
        //check_buffers();
        //check_idle_current();
        //check_idle_voltage();
        //check_idle_temperature();
    }
   
    if(!error_flags.all){
        VERBOSE_MSG(usart_send_string("System initialized without errors.\n"));
        set_state_idle();
    }
    else{
        VERBOSE_MSG(usart_send_string("Sorry. I have have found errors in the initialilation process. \n\nI will begin to process it...\n"));
        set_state_error();
    }
}


/**
 * @brief waits for commands while checking the system:
 *  - checks the deadman's switch state
 *  - checks the on_off_switch state
 *  - checks the potentiometer state
 *  then if its ok, enable the system to operate
 */
inline void task_idle(void)
{
    if(led_clk_div++ >= 50){
        cpl_led();
        led_clk_div = 0;
    }
 
    set_pwm_off();
    
    if(system_flags.can_enabled){
        check_can();
    }else{
        check_idle_zero_pot();
        check_switches();
    }

    VERBOSE_MSG(usart_send_char('\n'));

    if(system_flags.on_off_switch && system_flags.dms_switch && system_flags.pot_zero_width){
        VERBOSE_MSG(usart_send_string("Enjoy, the system is at its RUNNING STATE!!\n"));
        set_state_running();
    }
}


/**
 * @brief running task checks the system and apply the control action to pwm.
 */
inline void task_running(void)
{
    if(led_clk_div++ >= 10){
        cpl_led();
        led_clk_div = 0;
    }

    if(!system_flags.can_enabled){
        check_switches();
        //check_running_current();
        //check_running_voltage();
        //check_running_temperature();
    }

    VERBOSE_MSG(usart_send_char('\n'));

    if(system_flags.on_off_switch && system_flags.dms_switch){
        //calc_d(1);
        pwm_compute();
    }else{
        set_pwm_off();
        set_state_idle();
    }

}

/**
 * @brief error task checks the system and tries to medicine it.
 */
inline void task_error(void)
{
    if(led_clk_div++ >= 5){
        cpl_led();
        led_clk_div = 0;
    }

    OCR1A = INITIAL_D;      // assegura pwm com duty-cycle inicial

    total_errors++;         // incrementa a contagem de erros
    VERBOSE_MSG(usart_send_string("The error code is: "));
    VERBOSE_MSG(usart_send_char(error_flags.all));
    VERBOSE_MSG(usart_send_char('\n'));

    if(error_flags.overcurrent)
        VERBOSE_MSG(usart_send_string("\t - Motor over-current!\n"));
    if(error_flags.overvoltage)
        VERBOSE_MSG(usart_send_string("\t - Motor over-voltage!\n"));
    if(error_flags.overheat)
        VERBOSE_MSG(usart_send_string("\t - Motor over-heat!\n"));
    if(!error_flags.all)
        VERBOSE_MSG(usart_send_string("\t - Oh no, it was some unknown error.\n"));
 
    VERBOSE_MSG(usart_send_string("The error level is: "));
    VERBOSE_MSG(usart_send_uint16(total_errors));
    VERBOSE_MSG(usart_send_char('\n'));
    
    if(total_errors < 2){
        VERBOSE_MSG(usart_send_string("I will reset the machine state.\n"));
    }
    if(total_errors >= 20){
        VERBOSE_MSG(usart_send_string("The watchdog will reset the whole system.\n"));
        for(;;);    // waits the watchdog to reset.
    }
    
    cpl_led();
    set_state_initializing();
}


/**
 * @brief this is the machine state itself.
 */
inline void machine_run(void)
{

//    DEBUG ONLY!!!!!!!!!
    check_can();

    switch(state_machine){
        case STATE_INITIALIZING:
            task_initializing();

            break;
        case STATE_IDLE:
            task_idle();

            break;
        case STATE_RUNNING:
            task_running();

            break;
        case STATE_ERROR:
            task_error();

        default:
            break;
    }
}

/**
 * @brief Interrupcao das chaves: se alguma chave desligar, o motor desliga.
 */
ISR(PCINT2_vect)
{    
    if(bit_is_clear(FAULT_PIN, FAULT)){
        //calc_d(100);                // diminui 10% do duty-cycle imediatamente
        pwm_treat_fault();
        set_led();
        pwm_fault_count++;
    }

    if(bit_is_set(SWITCHES_PIN, ON_OFF_SWITCH)
            || bit_is_clear(SWITCHES_PIN, DEAD_MAN_SWITCH)){
        set_pwm_off();
        set_state_idle();
    }

    DEBUG1;
}

