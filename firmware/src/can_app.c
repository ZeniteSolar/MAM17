#include "can_app.h"

uint8_t can_app_send_state_clk_div;
uint8_t can_app_send_motor_clk_div;
uint16_t can_app_checks_without_mic19_msg;
uint16_t can_app_checks_without_mac22_msg;
uint16_t can_app_checks_without_mswi19_msg;
uint8_t mswi19_connected;

/**
 * @brief Prints a can message via usart
 */
inline void can_app_print_msg(can_t *msg)
{
    usart_send_string("ID: ");
    usart_send_uint16(msg->id);
    usart_send_string(". D: ");

    for(uint8_t i = 0; i < msg->length; i++){
      usart_send_uint16(msg->data[i]);
      usart_send_char(' ');
    }

    usart_send_string(". ERR: ");
    can_error_register_t err = can_read_error_register();
    usart_send_uint16(err.rx);
    usart_send_char(' ');
    usart_send_uint16(err.tx);
    usart_send_char('\n');
}

/**
* @brief Manages the canbus application protocol
*/
inline void can_app_task(void)
{
    check_can();

    if(can_app_send_state_clk_div++ >= CAN_APP_SEND_STATE_CLK_DIV){
        VERBOSE_MSG_CAN_APP(usart_send_string("state msg was sent.\n"));
        can_app_send_state();
        can_app_send_state_clk_div = 0;
    }

    if(can_app_send_motor_clk_div++ >= CAN_APP_SEND_MOTOR_CLK_DIV){
        VERBOSE_MSG_CAN_APP(usart_send_string("motor msg was sent.\n"));
        can_app_send_motor();
        can_app_send_motor_clk_div = 0;
    }

}

inline void can_app_send_state(void)
{
    can_t msg;
    msg.id                                  = CAN_MSG_MAM19_STATE_ID;
    msg.length                              = CAN_MSG_GENERIC_STATE_LENGTH;
    msg.flags.rtr = 0;

    msg.data[CAN_MSG_GENERIC_STATE_SIGNATURE_BYTE]  = CAN_SIGNATURE_SELF;
    msg.data[CAN_MSG_GENERIC_STATE_STATE_BYTE]      = (uint8_t) state_machine;
    // msg.data[CAN_MSG_GENERIC_STATE_CONTACTOR_BYTE]    = (uint8_t) state_contactor; // TODO: adicionar?
    msg.data[CAN_MSG_GENERIC_STATE_ERROR_BYTE]      = error_flags.all;

    can_send_message(&msg);
}

inline void can_app_send_motor(void)
{
    can_t msg;
    msg.id                                  = CAN_MSG_MAM19_MOTOR_ID;
    msg.length                              = CAN_MSG_MAM19_MOTOR_LENGTH;
    msg.flags.rtr = 0;

    msg.data[CAN_MSG_GENERIC_STATE_SIGNATURE_BYTE]  = CAN_SIGNATURE_SELF;
    msg.data[CAN_MSG_MAM19_MOTOR_D_BYTE]            = control.D_raw;
    msg.data[CAN_MSG_MAM19_MOTOR_I_BYTE]            = control.I;
    // msg.data[CAN_MSG_MAM19_MOTOR_R_BYTE]            = control.R; // TODO: adicionar?

    can_send_message(&msg);
}

inline void can_app_send_contactor_request(uint8_t request)
{
    can_t msg;
    msg.id                                  = CAN_MSG_MAM19_CONTACTOR_ID;
    msg.length                              = CAN_MSG_MAM19_CONTACTOR_LENGTH;
    msg.flags.rtr = 0;

    msg.data[CAN_MSG_GENERIC_STATE_SIGNATURE_BYTE]  = CAN_SIGNATURE_SELF;
    msg.data[CAN_MSG_MAM19_CONTACTOR_REQUEST_BYTE]  = request;

    can_send_message(&msg);

    contactor.message_received = CONTACTOR_REQUEST_UNKNOWN;
    contactor.message_sent = request;
    contactor.timeout_clk_div = 0;
}

/**
 * @brief extracts the specific MIC19 MOTOR  message
 *
 * The msg is AAAAAAAABBBBBBBB
 * A is the Signature of module
 * B is the Response
 *
 * @param *msg pointer to the message to be extracted
*/
inline void can_app_extractor_mac22_contactor_response(can_t *msg)
{
    if(msg->data[CAN_MSG_GENERIC_STATE_SIGNATURE_BYTE] == CAN_SIGNATURE_MAC22){
        can_app_checks_without_mac22_msg = 0;

        contactor.message_received        = msg->data[CAN_MSG_MAC22_CONTACTOR_RESPONSE_BYTE];
    }
}

/**
 * @brief extracts the specific MIC19 STATE message
 * @param *msg pointer to the message to be extracted
 */
inline void can_app_extractor_mic19_state(can_t *msg)
{
    if(msg->data[CAN_MSG_GENERIC_STATE_SIGNATURE_BYTE] == CAN_SIGNATURE_MIC19){
        can_app_checks_without_mic19_msg = 0;
        if(msg->data[CAN_MSG_GENERIC_STATE_ERROR_BYTE]){
            //ERROR!!!
        }
        /*if(contador == maximo)*/{
            //ERROR!!!
        }


    }
}

inline void can_app_extractor_mac22_state(can_t *msg)
{
    if(msg->data[CAN_MSG_GENERIC_STATE_SIGNATURE_BYTE] == CAN_SIGNATURE_MAC22){
        can_app_checks_without_mac22_msg = 0;
        if(msg->data[CAN_MSG_GENERIC_STATE_ERROR_BYTE]){
            //ERROR!!!
        }
        /*if(contador == maximo)*/{
            //ERROR!!!
        }


    }
}

inline void can_app_extractor_mswi19_state(can_t *msg)
{
    if(msg->data[CAN_MSG_GENERIC_STATE_SIGNATURE_BYTE] == CAN_SIGNATURE_MSWI19){
        can_app_checks_without_mswi19_msg = 0;
        if(msg->data[CAN_MSG_GENERIC_STATE_ERROR_BYTE]){
            //ERROR!!!
        }
        /*if(contador == maximo)*/{
            //ERROR!!!
        }


    }
}
/**
 * @brief extracts the specific MIC19 MOTOR  message
 *
 * The msg is AAAAAAAA0000000CBEEEEEEEEFFFFFFFF
 * A is the Signature of module
 * B is the motor on/off switch
 * C is the deadman's switch
 * E is the voltage potentiometer
 * F is the current potentiometer
 *
 * @param *msg pointer to the message to be extracted
*/
inline void can_app_extractor_mic19_motor(can_t *msg)
{
    if(msg->data[CAN_MSG_GENERIC_STATE_SIGNATURE_BYTE] == CAN_SIGNATURE_MIC19){
            can_app_checks_without_mic19_msg = 0;

        system_flags.motor_on       = bit_is_set(msg->data[
            CAN_MSG_MIC19_MOTOR_MOTOR_BYTE],
            CAN_MSG_MIC19_MOTOR_MOTOR_MOTOR_ON_BIT);

        system_flags.dms            = bit_is_set(msg->data[
            CAN_MSG_MIC19_MOTOR_MOTOR_BYTE],
            CAN_MSG_MIC19_MOTOR_MOTOR_DMS_ON_BIT);

        system_flags.reverse        = bit_is_set(msg->data[
            CAN_MSG_MIC19_MOTOR_MOTOR_BYTE],
            CAN_MSG_MIC19_MOTOR_MOTOR_REVERSE_BIT);

        if(!mswi19_connected){
            control.D_raw_target    = msg->data[CAN_MSG_MIC19_MOTOR_D_BYTE];
        }

        control.I_raw_target        = msg->data[CAN_MSG_MIC19_MOTOR_I_BYTE];
    }
}

inline void can_app_extractor_mswi19_motor(can_t *msg)
{
    if(msg->data[CAN_MSG_GENERIC_STATE_SIGNATURE_BYTE] == CAN_SIGNATURE_MSWI19){
        can_app_checks_without_mswi19_msg = 0;
        mswi19_connected = 1;

        control.D_raw_target    = msg->data[CAN_MSG_MSWI19_MOTOR_D_BYTE];
    }
}

inline void can_app_msg_mic19_extractors_switch(can_t *msg)
{
    switch(msg->id){
        case CAN_MSG_MIC19_MOTOR_ID:
            VERBOSE_MSG_CAN_APP(usart_send_string("got a motor msg from mic19: "));
            VERBOSE_MSG_CAN_APP(can_app_print_msg(msg));
            can_app_extractor_mic19_motor(msg);
            break;
        case CAN_MSG_MIC19_STATE_ID:
            VERBOSE_MSG_CAN_APP(usart_send_string("got a state msg from mic19: "));
            VERBOSE_MSG_CAN_APP(can_app_print_msg(msg));
            can_app_extractor_mic19_state(msg);
            break;
        default:
            VERBOSE_MSG_CAN_APP(usart_send_string("got a unknown msg from mic19: "));
            VERBOSE_MSG_CAN_APP(can_app_print_msg(msg));
            break;
    }
}

inline void can_app_msg_mswi19_extractors_switch(can_t *msg)
{
    switch(msg->id){
        case CAN_MSG_MSWI19_MOTOR_ID:
            VERBOSE_MSG_CAN_APP(usart_send_string("got a motor msg from mswi19: "));
            VERBOSE_MSG_CAN_APP(can_app_print_msg(msg));
            can_app_extractor_mswi19_motor(msg);
            break;
        case CAN_MSG_MSWI19_STATE_ID:
            VERBOSE_MSG_CAN_APP(usart_send_string("got a state msg from mswi19: "));
            VERBOSE_MSG_CAN_APP(can_app_print_msg(msg));
            can_app_extractor_mswi19_state(msg);
            break;
        default:
            VERBOSE_MSG_CAN_APP(usart_send_string("got a unknown msg from mswi19: "));
            VERBOSE_MSG_CAN_APP(can_app_print_msg(msg));
            break;
    }
}

inline void can_app_msg_mac22_extractors_switch(can_t *msg)
{
    switch(msg->id){
        case CAN_MSG_MAC22_CONTACTOR_ID:
            VERBOSE_MSG_CAN_APP(usart_send_string("got a motor msg from mac22: "));
            VERBOSE_MSG_CAN_APP(can_app_print_msg(msg));
            can_app_extractor_mac22_contactor_response(msg);
            break;
        case CAN_MSG_MAC22_STATE_ID:
            VERBOSE_MSG_CAN_APP(usart_send_string("got a state msg from mac22: "));
            VERBOSE_MSG_CAN_APP(can_app_print_msg(msg));
            can_app_extractor_mac22_state(msg);
            break;
        default:
            VERBOSE_MSG_CAN_APP(usart_send_string("got a unknown msg from mac22: "));
            VERBOSE_MSG_CAN_APP(can_app_print_msg(msg));
            break;
    }
}

/**
 * @brief redirects a specific message extractor to a given message
 * @param *msg pointer to the message to be extracted
 */
inline void can_app_msg_extractors_switch(can_t *msg)
{
    switch(msg->data[CAN_MSG_GENERIC_STATE_SIGNATURE_BYTE]){
        case CAN_SIGNATURE_MIC19:
            can_app_msg_mic19_extractors_switch(msg);
            break;
        case CAN_SIGNATURE_MSWI19:
            can_app_msg_mswi19_extractors_switch(msg);
            break;
        case CAN_SIGNATURE_MAC22:
            can_app_msg_mac22_extractors_switch(msg);
            break;
        default: break;
    }
}

/**
 * @brief Manages to receive and extract specific messages from canbus
 */
inline void check_can(void)
{
    // If no messages is received from mic19 for
    // CAN_APP_CHECKS_WITHOUT_MIC19_MSG cycles, than it go to a specific error state.
    //VERBOSE_MSG_CAN_APP(usart_send_string("checks: "));
    //VERBOSE_MSG_CAN_APP(usart_send_uint16(can_app_checks_without_mic19_msg));
    if(can_app_checks_without_mic19_msg++ >= CAN_APP_CHECKS_WITHOUT_MIC19_MSG){
        VERBOSE_MSG_CAN_APP(usart_send_string("Error: too many cycles without mic19 messages.\n"));
        can_app_checks_without_mic19_msg = 0;
        error_flags.no_canbus = 1;
        set_state_error();
    }

    if(can_app_checks_without_mswi19_msg++ >= CAN_APP_CHECKS_WITHOUT_MSWI19_MSG){
        VERBOSE_MSG_CAN_APP(usart_send_string("Warning: mswi19 not connected. Using D from mic19."));
        can_app_checks_without_mswi19_msg = 0;
        mswi19_connected = 0;
    }

    if(can_app_checks_without_mac22_msg++ >= CAN_APP_CHECKS_WITHOUT_MAC22_MSG){
        VERBOSE_MSG_CAN_APP(usart_send_string("Error: too many cycles without mac22 messages.\n"));
        can_app_checks_without_mac22_msg = 0;
        #ifdef SET_ERROR_WHEN_NO_STATE_MESSAGES_FROM_MAC22
            error_flags.no_contactor = 1;
            set_state_error();
        #endif
    }

    if(can_check_message()){
        can_t msg;
        if(can_get_message(&msg)){
            can_app_msg_extractors_switch(&msg);
        }
    }
}
