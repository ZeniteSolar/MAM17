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

void can_app_print_msg(can_t *msg);
void can_app_task(void);
void can_app_send_state(void);
void can_app_send_motor(void);
void can_app_extractor_mic17_state(can_t *msg);
void can_app_extractor_mic17_motor(can_t *msg);
void can_app_msg_extractors_switch(can_t *msg);
void check_can(void);

//extern control_t control;
//extern flags_errors_t flags_errors;
//extern system_flags_t system_flags;

#endif /* ifndef CAN_APP_H */
