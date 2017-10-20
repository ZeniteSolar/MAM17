#ifndef _CAN_IDS_H_
#define _CAN_IDS_H_ 

#define CAN_FILTER_ACCEPTALL            0       //<! accept all messages
#define CAN_FILTER_IGNOREALL            999999  //<! ignore all messages
 

// GENERIC FIELDS FROM ANY MODULE's STATE MESSAGE
#define CAN_IDC                         1       //<! ID from compilation
#define CAN_STATE                       2       //<! state flags
#define CAN_ERROR                       3       //<! error flags


// MESSGES FROM MIC17:
#define CAN_FILTER_MSG_MIC17_STATE      7       //<! Informs about state and errors
#define CAN_FILTER_MSG_MIC17_MOTOR      9       //<! Informs about motor controls.
#define CAN_FILTER_MSG_MIC17_PUMPS      11      //<! Informs about pumps controls.
#define CAN_FILTER_MSG_MIC17_MPPTS      10      //<! Informs about mppts controls.

// MESSAGES FROM AM17:
#define CAN_FILTER_MSG_AM17_STATE       17      //<! Informs about state and errors
#define CAN_FILTER_MSG_AM17_MOTOR       18      //<! Informs about motor

// MESSAGES FROM MAB17
#define CAN_FILTER_MSG_MAB17_STATE      276     //<! Informs about state and errors
#define CAN_FILTER_MSG_MAB17_PUMP       277     //<! Informs about motor

#endif /* ifndef _CAN_IDS_H_ */
