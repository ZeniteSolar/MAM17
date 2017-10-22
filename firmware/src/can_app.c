#include "can_app.h"

/**
 * @brief Prints a can message via usart
 */
inline void can_app_print_msg(can_t *msg)
{
	usart_send_string("ID: ");
	usart_send_uint16(msg->id);
	usart_send_string(". Data: ");

	for(uint8_t i = 0; i < msg->length; i++){
	  usart_send_uint16(msg->data[i]);
	  usart_send_char(' ');
	}

	usart_send_string(". ERR: ");
	
//	usart_send_uint16(can_msg_err);
	usart_send_char('\n');
}

/**
* @brief Manages the canbus application protocol
*/
inline void can_app_task(void)
{
    check_can();
    can_app_send_state();
}

inline void can_app_send_state(void)
{
    can_t msg;
    msg.id                                  = CAN_FILTER_MSG_AC17_STATE;
    msg.length                              = CAN_LENGTH_MSG_STATE;

    msg.data[CAN_SIGNATURE_BYTE]            = CAN_SIGNATURE_SELF;
    msg.data[CAN_STATE_MSG_STATE_BYTE]      = (uint8_t) state_machine;
    msg.data[CAN_STATE_MSG_ERROR_BYTE]      = error_flags.all;

    can_send_message(&msg);
}

inline void can_app_send_motor(void)
{
    can_t msg;
    msg.id                                  = CAN_FILTER_MSG_AC17_MOTOR;
    msg.length                              = CAN_LENGTH_MSG_AC17_MOTOR;

    msg.data[CAN_SIGNATURE_BYTE]            = CAN_SIGNATURE_SELF;
    msg.data[CAN_MSG_AC17_MOTOR_D_BYTE]     = control.D;
    msg.data[CAN_MSG_AC17_MOTOR_V_BYTE]     = control.V;    
    msg.data[CAN_MSG_AC17_MOTOR_I_BYTE]     = control.I;
    msg.data[CAN_MSG_AC17_MOTOR_R_BYTE]     = control.R;
    msg.data[CAN_MSG_AC17_MOTOR_T_BYTE]     = control.T;

    can_send_message(&msg); 
}

/**
 * @brief extracts the specific MIC17 STATE message
 * @param *msg pointer to the message to be extracted
 */
inline void can_app_extractor_mic17_state(can_t *msg)
{
    //
}
 
/**
 * @brief extracts the specific MIC17 MOTOR  message
 *
 * The msg is AAAAAAAA000000DCBEEEEEEEEFFFFFFFF
 * A is the Signature of module
 * B is the on/off switch
 * C is the deadman's switch
 * D is the pot_zero_width
 * E is the voltage potentiometer
 * F is the current potentiometer
 *
 * @param *msg pointer to the message to be extracted
*/ 
inline void can_app_extractor_mic17_motor(can_t *msg)
{
    system_flags.on_off_switch  = bit_is_set(msg->data[2], 0);
    system_flags.dms_switch     = bit_is_set(msg->data[2], 1);
    system_flags.pot_zero_width = bit_is_set(msg->data[2], 2);

    control.D_raw_target        = msg->data[1];
    control.I_raw_target        = msg->data[2];
}

/**
 * @brief redirects a specific message extractor to a given message
 * @param *msg pointer to the message to be extracted
 */
inline void can_app_msg_extractors_switch(can_t *msg)
{
    if(msg->data[CAN_SIGNATURE_BYTE] == CAN_SIGNATURE_MIC17){
        switch(msg->id){
            case CAN_FILTER_MSG_MIC17_MOTOR:
                can_app_extractor_mic17_motor(msg);
                break;
            case CAN_FILTER_MSG_MIC17_STATE:
                can_app_extractor_mic17_state(msg);
                break;
            default:
                break;
        }    
    }
}

/**
 * @brief Manages to receive and extract specific messages from canbus
 */
inline void check_can(void)
{
    if(can_check_message()){
        can_t msg;
        if(can_get_message(&msg)){
            can_app_msg_extractors_switch(&msg);
        }
    }
}

