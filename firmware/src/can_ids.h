#ifndef _CAN_IDS_H_
#define _CAN_IDS_H_ 

#define CAN_FILTER_ACCEPTALL            0b00000000000  //<! accept all messages
#define CAN_FILTER_IGNOREALL            0b11111111111  //<! ignore all messages
 

// GENERIC FIELDS FROM ANY MODULE's STATE MESSAGE
#define CAN_IDC                         1       //<! ID from compilation
#define CAN_STATE                       2       //<! state flags
#define CAN_ERROR                       3       //<! error flags


// MESSGES FROM MIC17:
#define CAN_MASK_MIC17                  0b00000100011   //<! Mask for MIC17
#define CAN_FILTER_MSG_MIC17_STATE      0b00000100000   //<! Informs about state and errors
#define CAN_FILTER_MSG_MIC17_MOTOR      0b00000100001   //<! Informs about motor controls.
#define CAN_FILTER_MSG_MIC17_PUMPS      0b00000100010   //<! Informs about pumps controls.
#define CAN_FILTER_MSG_MIC17_MPPTS      0b00000100011   //<! Informs about mppts controls.

// MESSAGES FROM AM17:
//#define CAN_FILTER_MSG_AM17_STATE       0b00000000000    //<! Informs about state and errors
//#define CAN_FILTER_MSG_AM17_MOTOR       0b00000000000      //<! Informs about motor

// MESSAGES FROM MAB17
//#define CAN_FILTER_MSG_MAB17_STATE      0b00000000000     //<! Informs about state and errors
//#define CAN_FILTER_MSG_MAB17_PUMP       0b00000000000    //<! Informs about motor

#endif /* ifndef _CAN_IDS_H_ */
