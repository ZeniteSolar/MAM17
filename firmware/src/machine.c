#include "machine.h"

control_t control;
state_machine_t state_machine;
state_machine_t last_state;
system_flags_t system_flags;
error_flags_t error_flags;
volatile uint8_t machine_clk;
uint8_t total_errors; // Contagem de ERROS
uint8_t pwm_fault_count;
uint8_t check_pwm_fault_times;
uint8_t led_clk_div;
state_contactor_t state_contactor;
contactor_t contactor;
uint16_t pwm_last_value; // Used to know the OCR1A before each call of set_state_contactor()

/*
 * to-do:
 *      - modularize state sinalization
 *      - 
 *
 */

/**
 * @brief 
 */
void machine_init(void)
{
    TCCR2A  =   (1 << WGM21) | (0 << WGM20)         // Timer 2 in Mode 2 = CTC (clear on compar  e)
            | (0 << COM2A1) | (0 << COM2A0)         // do nothing with OC2A
            | (0 << COM2B1) | (0 << COM2B0);        // do nothing with OC2B
    TCCR2B  =   (0 << WGM22)                        // Timer 0 in Mode 2 = CTC (clear on compar  e)
            | (0 << FOC0A) | (0 << FOC0B)           // dont force outputs
            | (1 << CS22)                           // clock enabled, prescaller = 1024
            | (1 << CS21)
            | (1 << CS20);
    OCR2A   =   80;                                // Valor para igualdade de comparacao A par  a frequencia de 150 Hz
    TIMSK2 |=   (1 << OCIE2A);                      // Ativa a interrupcao na igualdade de comp  aração do TC2 com OCR2A

	
}

/**
* @brief checks if the pot is zero and updates the system flags
*/
inline void check_idle_zero_pot(void)
{
    system_flags.pot_zero_width = pwm_zero_width(control.D_raw_target);
}

/**
 * @brief checks if the reverse has changed
 */
inline void check_reverse(void)
{
    static uint8_t last_reverse = 0;
    if(last_reverse != system_flags.reverse){
        set_state_contactor();
    }
    last_reverse = system_flags.reverse;
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
    VERBOSE_MSG_MACHINE(usart_send_string("Checking buffers..."));
    //while(!CBUF_IsFull(cbuf_adc0));
    VERBOSE_MSG_ERROR(usart_send_string("<No buffers to check>"));
    //VERBOSE_MSG_MACHINE(usart_send_string(" \t\tdone.\n")); 
}

/**
 * @brief checks if the current level is ok for IDLE state
 */
inline void check_idle_current(void)
{
    VERBOSE_MSG_MACHINE(usart_send_string("Checking motor current levels..."));
    //if(motor_current <= MAXIMUM_OFF_CURRENT);
        VERBOSE_MSG_MACHINE(usart_send_string(" \t\tdone.\n"));
    //else error_flag: motor_current at off_state bigger than it should be. 
}

/**
 * @brief checks if the voltage level is ok for IDLE state
 */
inline void check_idle_voltage(void)
{
    VERBOSE_MSG_MACHINE(usart_send_string("Checking motor voltage levels..."));
    //if(output_voltage <= MAXIMUM_OFF_VOLTAGE);
        VERBOSE_MSG_MACHINE(usart_send_string(" \t\tdone.\n"));
    //else error_flag: motor_voltage at off_state bigger than it should be.
}

/**
 * @brief checks if the temperature level is ok for IDLE state
 */
inline void check_idle_temperature(void)
{
    VERBOSE_MSG_MACHINE(usart_send_string("Checking motor temperature levels..."));
    //if(temperature <= MAXIMUM_OFF_TEMPERATURE);
        VERBOSE_MSG_MACHINE(usart_send_string(" \t\tdone.\n"));
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
* @brief set contactor state
*/
inline void set_state_contactor(void)
{
    usart_send_string("SET STATE CONTACTOR\n");
    pwm_last_value = OCR1A;
    set_pwm_off();
    state_contactor = STATE_CONTACTOR_WAITING_MOTOR;
    state_machine = STATE_CONTACTOR;
}

/**
* @brief set idle state
*/ 
inline void set_state_idle(void)
{
    set_pwm_off();
    pwm_zero_width(PWM_D_MIN_THRESHHOLD + 1);   //reset pwm_zero_width state
    state_machine = STATE_IDLE;
}

/**
* @brief set running state
*/ 
inline void set_state_running(void)
{
    usart_send_string("SET STATE RUNNING\n");
    state_machine = STATE_RUNNING;
}

/**
* @brief prints the system flags
*/
inline void print_system_flags(void)
{
	VERBOSE_MSG_MACHINE(usart_send_string(" Motor Sw: "));
    VERBOSE_MSG_MACHINE(usart_send_char(48+system_flags.motor_on));
    
    VERBOSE_MSG_MACHINE(usart_send_string(" DMS Sw: "));
    VERBOSE_MSG_MACHINE(usart_send_char(48+system_flags.dms));

    VERBOSE_MSG_MACHINE(usart_send_string(" Pot_zero: "));
    VERBOSE_MSG_MACHINE(usart_send_char(48+system_flags.pot_zero_width)); 

    VERBOSE_MSG_MACHINE(usart_send_string(" Rev: "));
    VERBOSE_MSG_MACHINE(usart_send_char(48+system_flags.reverse));
}

/**
* @brief prints the error flags
*/
inline void print_error_flags(void)
{
    VERBOSE_MSG_MACHINE(usart_send_string("\n OvrI: "));
    VERBOSE_MSG_MACHINE(usart_send_char(48+error_flags.overcurrent));
    
    VERBOSE_MSG_MACHINE(usart_send_string(" OvrV: "));
    VERBOSE_MSG_MACHINE(usart_send_char(48+error_flags.overvoltage));

    VERBOSE_MSG_MACHINE(usart_send_string(" OvrT: "));
    VERBOSE_MSG_MACHINE(usart_send_char(48+error_flags.overheat));

    VERBOSE_MSG_MACHINE(usart_send_string(" NOCAN: "));
    VERBOSE_MSG_MACHINE(usart_send_char(48+error_flags.no_canbus));
    VERBOSE_MSG_MACHINE(usart_send_char(' '));
}
 
/**
* @brief prints the error flags
*/
inline void print_control(void)
{
    VERBOSE_MSG_MACHINE(usart_send_string("D: "));
    VERBOSE_MSG_MACHINE(usart_send_uint16(control.D_raw));
    VERBOSE_MSG_MACHINE(usart_send_char(' '));
    VERBOSE_MSG_MACHINE(usart_send_uint16(control.D_raw_target));
    VERBOSE_MSG_MACHINE(usart_send_char(' '));
    VERBOSE_MSG_MACHINE(usart_send_uint16(control.D));
    
    VERBOSE_MSG_MACHINE(usart_send_string(" I: "));
    VERBOSE_MSG_MACHINE(usart_send_uint16(control.I_raw));
    VERBOSE_MSG_MACHINE(usart_send_char(' '));
    VERBOSE_MSG_MACHINE(usart_send_uint16(control.I_raw_target));
    VERBOSE_MSG_MACHINE(usart_send_char(' '));
    VERBOSE_MSG_MACHINE(usart_send_uint16(control.I));

    VERBOSE_MSG_MACHINE(usart_send_string(" V: "));
    VERBOSE_MSG_MACHINE(usart_send_uint16(control.V));

    VERBOSE_MSG_MACHINE(usart_send_string(" R: "));
    VERBOSE_MSG_MACHINE(usart_send_uint16(control.R));

    VERBOSE_MSG_MACHINE(usart_send_string(" T: "));
    VERBOSE_MSG_MACHINE(usart_send_uint16(control.T));
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
    pwm_reset();

    //check_buffers();
    //check_idle_current();
    //check_idle_voltage();
    //check_idle_temperature();
       
    if(!error_flags.all){
        VERBOSE_MSG_INIT(usart_send_string("System initialized without errors.\n"));
        set_state_idle();
        set_initial_state();
    }
    else{
        VERBOSE_MSG_ERROR(usart_send_string("Sorry. I have have found errors in the initialilation process. \n\nI will begin to process it...\n"));
        set_state_error();
    }
}

/**
 * @brief
 * - sets the duty-cycle to zero,
 * - waits until the motor stops (based on some experimental time),
 * - sends the switch task to the contactor,
 * - waits until the contactor finishes its task, or timeout,
 * - go to idle state,
 */
inline void task_change_contactor(void)
{
    set_pwm_off();

    if(led_clk_div++ >= 100){
        cpl_led();
        led_clk_div = 0;
    }

    uint16_t motor_clk_div_max = 0;
    {
        // Calculated in pwm_signals.ipynb as y = a*x +b
        const int a = 4;
        const int b = 375;
        motor_clk_div_max = a * pwm_last_value + b;
    }

    switch(state_contactor){
        default: case STATE_CONTACTOR_WAITING_MOTOR:
            if(contactor.motor_stop_clk_div++ >= motor_clk_div_max){
                state_contactor = STATE_CONTACTOR_SEND_REQUEST;
                contactor.motor_stop_clk_div = 0;
            }

            break;
        case STATE_CONTACTOR_SEND_REQUEST:
            contactor_request_t request = CONTACTOR_REQUEST_TURN_OFF;

            if(system_flags.motor_on){
                if(system_flags.reverse){
                    request = CONTACTOR_REQUEST_SET_REVERSE;
                }else{
                    request = CONTACTOR_REQUEST_SET_FORWARD;
                }
            }

            can_app_send_contactor_request((uint8_t)request);
            state_contactor = STATE_CONTACTOR_WAITING_RESPONSE;
            contactor.timeout_clk_div = 0;

            break;
        case STATE_CONTACTOR_WAITING_RESPONSE:

            if(contactor.message_sent == contactor.message_received){
                contactor.acknowledged = 1;
                contactor.timeout_clk_div = 0;
            }

            if(contactor.timeout_clk_div++ >= 500){
                VERBOSE_MSG_MACHINE(usart_send_string("Contactor request timeout!\n"));
                state_contactor = STATE_CONTACTOR_SEND_REQUEST;
            }

            if(contactor.acknowledged){
                VERBOSE_MSG_MACHINE(usart_send_string("CHANGE CONTACTOR task done, going to IDLE STATE!\n"));
                set_state_idle();
            }

            break;
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

    check_reverse();
    check_idle_zero_pot();
    //check_idle_current();
    //check_idle_voltage();
    //check_idle_temperature();

    if(system_flags.motor_on && system_flags.dms && system_flags.pot_zero_width){
        VERBOSE_MSG_MACHINE(usart_send_string("Enjoy, the system is at its RUNNING STATE!!\n"));
        set_state_running();
    }
    if(!system_flags.motor_on)
        set_state_contactor();

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

    check_reverse();
    //check_pwm_fault();
    //check_running_current();
    //check_running_voltage();
    //check_running_temperature();

    if(system_flags.motor_on && system_flags.dms){
        pwm_compute();
    }else{
        usart_send_string("going to idle");
        set_state_idle();
    }

}

void set_initial_state(void)
{
	can_app_send_contactor_request(CONTACTOR_REQUEST_TURN_OFF);
    system_flags.all__ = 0;
    error_flags.all = 0;

    control.D_raw = control.D_raw_target = control. D = 0;

    control.I_raw = control.I_raw_target = control.I = 0;
    control.V = control.R = control.T = control.fault = 0;
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

    pwm_reset();

    total_errors++;         // incrementa a contagem de erros
    VERBOSE_MSG_ERROR(usart_send_string("The error code is: "));
    VERBOSE_MSG_ERROR(usart_send_uint16(error_flags.all));
    VERBOSE_MSG_ERROR(usart_send_char('\n'));

    if(error_flags.overcurrent)
        VERBOSE_MSG_ERROR(usart_send_string("\t - Motor over-current!\n"));
    if(error_flags.overvoltage)
        VERBOSE_MSG_ERROR(usart_send_string("\t - Motor over-voltage!\n"));
    if(error_flags.overheat)
        VERBOSE_MSG_ERROR(usart_send_string("\t - Motor over-heat!\n"));
    if(error_flags.fault)
        VERBOSE_MSG_ERROR(usart_send_string("\t - IR2127 FAULT!\n"));
    if(error_flags.no_canbus)
        VERBOSE_MSG_ERROR(usart_send_string("\t - No canbus communication with MIC19!\n"));
    if(!error_flags.all)
        VERBOSE_MSG_ERROR(usart_send_string("\t - Oh no, it was some unknown error.\n"));
 
    VERBOSE_MSG_ERROR(usart_send_string("The error level is: "));
    VERBOSE_MSG_ERROR(usart_send_uint16(total_errors));
    VERBOSE_MSG_ERROR(usart_send_char('\n'));
    
    if(total_errors < 2){
        VERBOSE_MSG_ERROR(usart_send_string("I will reset the machine state.\n"));
    }
    if(total_errors >= 20){
        VERBOSE_MSG_ERROR(usart_send_string("The watchdog will reset the whole system.\n"));
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
    can_app_task();
    print_system_flags();
    print_error_flags();
    print_control();

    if(machine_clk){
        machine_clk = 0;
        switch(state_machine){
            case STATE_INITIALIZING:
                task_initializing();

                break;
            case STATE_CONTACTOR:
                task_change_contactor();

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
}

/**
 * @brief Interrupcao das chaves: se alguma chave desligar, o motor desliga.
 */
ISR(INT1_vect)
{    
    /*if(bit_is_clear(FAULT_PIN, FAULT)){
        pwm_treat_fault();
        cpl_led();
        pwm_fault_count++;
    }
    */
    control.fault = 1;
    DEBUG1;
}

/**
* @brief ISR para ações de controle
*/
ISR(TIMER2_COMPA_vect)
{
    //VERBOSE_MSG_ERROR(if(machine_clk) usart_send_string("\nERROR: CLOCK CONFLICT!!!\n"));
	machine_clk = 1;
}

