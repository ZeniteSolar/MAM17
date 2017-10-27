/**
 * @file can_ids.h
 *
 * @defgroup CANAPP    Ids for can application
 *
 * @brief Ids for canbus application
 *
 */

#ifndef CAN_IDS_H
#define CAN_IDS_H 

// GENERIC FIELDS FROM ANY MODULE's STATE MESSAGE
#define CAN_LENGTH_MSG_STATE            3
#define CAN_SIGNATURE_BYTE              0       //<! ID from compilation
#define CAN_STATE_MSG_STATE_BYTE        1       //<! state flags
#define CAN_STATE_MSG_ERROR_BYTE        2       //<! error flags

// SIGNATURES
#define CAN_SIGNATURE_MIC17             243
#define CAN_SIGNATURE_AC17              244

#define CAN_SIGNATURE_SELF              CAN_SIGNATURE_AC17

// MESSGES FROM MIC17:
#define CAN_MASK_MIC17                  0b11111111111   //<! Mask for MIC17
#define CAN_FILTER_MSG_MIC17_STATE      0b00000100000   //<! Informs about state and errors
#define CAN_FILTER_MSG_MIC17_MOTOR      0b00000100001   //<! Informs about motor controls.
#define CAN_FILTER_MSG_MIC17_PUMPS      0b00000100010   //<! Informs about pumps controls.
#define CAN_FILTER_MSG_MIC17_MPPTS      0b00000100011   //<! Informs about mppts controls.
// MIC17 MOTOR:
#define CAN_LENGTH_MSG_MIC17_MOTOR          4
#define CAN_MSG_MIC17_MOTOR_MOTOR_ON_BYTE   1
#define CAN_MSG_MIC17_MOTOR_MOTOR_ON_BIT    0
#define CAN_MSG_MIC17_MOTOR_DMS_BYTE    	1
#define CAN_MSG_MIC17_MOTOR_DMS_BIT      	1
#define CAN_MSG_MIC17_MOTOR_D_RAW_BYTE	    2
#define CAN_MSG_MIC17_MOTOR_I_RAW_BYTE		3
// MIC17 PUMPS:
#define CAN_LENGTH_MSG_MIC17_PUMPS          2
#define CAN_MSG_MIC17_PUMPS_PUMPS_BYTE      1
#define CAN_MSG_MIC17_PUMPS_PUMP1_BIT       0
#define CAN_MSG_MIC17_PUMPS_PUMP2_BIT       1
#define CAN_MSG_MIC17_PUMPS_PUMP3_BIT       2
// MIC17 MPPTS:
#define CAN_LENGTH_MSG_MIC17_MPPTS          2
#define CAN_MSG_MIC17_MPPTS_MPPTS_ON_BYTE   1
#define CAN_MSG_MIC17_MPPTS_MPPTS_ON_BIT    0
#define CAN_MSG_MIC17_MPPTS_POT_BYTE        2


// MESSAGES FROM AC17:
#define CAN_MASK_AC17                   0b11111111111   //<! Mask for AC17
#define CAN_FILTER_MSG_AC17_STATE       0b00001000000   //<! Informs about state and errors
#define CAN_FILTER_MSG_AC17_MOTOR       0b00001000001   //<! Informs about motor
#define CAN_LENGTH_MSG_AC17_MOTOR       6
// AC17 MOTOR:
#define CAN_MSG_AC17_MOTOR_D_BYTE       1
#define CAN_MSG_AC17_MOTOR_V_BYTE       2
#define CAN_MSG_AC17_MOTOR_I_BYTE       3
#define CAN_MSG_AC17_MOTOR_R_BYTE       4
#define CAN_MSG_AC17_MOTOR_T_BYTE       5

// MESSAGES FROM MAB17
#define CAN_MASK_MAB17                  0b11111111111   //<! Mask for MAB17
#define CAN_FILTER_MSG_MAB17_STATE      0b00000000000   //<! Informs about state and errors
#define CAN_FILTER_MSG_MAB17_PUMP       0b00000000000   //<! Informs about motor

#endif /* ifndef CAN_IDS_H */
