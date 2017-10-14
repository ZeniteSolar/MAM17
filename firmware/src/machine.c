#include "machine.h"

#define set_pwm_off()     OCR1A = 0      //!< D of PWM TURNED TO 0

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
 * @brief checks if the pot is zeroed when idle.
 */
inline void check_idle_zero_pot(void)
{
    VERBOSE_MSG(usart_send_string("Potentiometer: "));
    if(zero_width(ma_adc0())){  // computa a media
        VERBOSE_MSG(usart_send_string("ok.  "));
        system_flags.pot_zero_width = 1;   
    }else{
        VERBOSE_MSG(usart_send_string("Nok. "));
        system_flags.pot_zero_width = 0;
    } 
}

/**
 * @brief
 */
inline void check_fault(void)
{
    if(fault_count >= FAULT_COUNT_LIMIT){
        error_flags.fault = 1;
        set_state_error();
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
    calc_d(0);
    state_machine = STATE_RUNNING;
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
    fault_count = 0;

    //check_buffers();
    //check_idle_current();
    //check_idle_voltage();
    //check_idle_temperature();
   
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
    if(led_div++ >= 50){
        cpl_led();
        led_div = 0;
    }
 
    set_pwm_off();
    check_idle_zero_pot();
    check_switches();

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
    if(led_div++ >= 10){
        cpl_led();
        led_div = 0;
    }
    check_switches();
    //check_running_current();
    //check_running_voltage();
    //check_running_temperature();

    VERBOSE_MSG(usart_send_char('\n'));

    if(system_flags.on_off_switch && system_flags.dms_switch){
        calc_d(1);
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
    if(led_div++ >= 5){
        cpl_led();
        led_div = 0;
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
        calc_d(100);                // diminui 10% do duty-cycle imediatamente
        set_led();
        fault_count++;
    }

    if(bit_is_set(SWITCHES_PIN, ON_OFF_SWITCH)
            || bit_is_clear(SWITCHES_PIN, DEAD_MAN_SWITCH)){
        set_pwm_off();
        set_state_idle();
    }

    DEBUG1;
}

