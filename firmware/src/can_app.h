/**
 * @file can_app.h
 *
 * @defgroup CANAPP    Application Module for canbus
 *
 * @brief The application layer module for canbus
 *
 */

#ifndef CAN_APP_H
#define CAN_APP_H 

#include "conf.h"
#include "machine.h"
#include "can.h"
#include "can_ids.h"
#include "usart.h"
#include "dbg_vrb.h"
#include "../lib/bit_utils.h"

void can_app_print_msg(can_t *msg);
void can_app_task(void);
void can_app_send_state(void);
void can_app_send_motor(void);
void can_app_extractor_mic19_state(can_t *msg);
void can_app_extractor_mic19_motor(can_t *msg);
void can_app_extractor_mswi19_state(can_t *msg);
void can_app_extractor_mswi19_motor(can_t *msg);
void can_app_msg_extractors_switch(can_t *msg);
void check_can(void);

#define CAN_APP_SEND_STATE_CLK_DIV  100
#define CAN_APP_SEND_MOTOR_CLK_DIV  50

extern uint8_t can_app_send_state_clk_div;
extern uint8_t can_app_send_motor_clk_div;
extern uint16_t can_app_checks_without_mic19_msg;
#define CAN_APP_CHECKS_WITHOUT_MIC19_MSG 100
extern uint16_t can_app_checks_without_mswi19_msg;
#define CAN_APP_CHECKS_WITHOUT_MSWI19_MSG 200
extern uint8_t mswi19_connected;

#endif /* ifndef CAN_APP_H */
