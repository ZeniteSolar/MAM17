/**
 * @file dbg_vrb.h
 *
 * @defgroup DBG_VRB    Debug And Verbose
 *
 * @brief Debug and Verbose mode definitions
 *
 */

#ifndef _DBG_VRB_H_
#define _DBG_VRB_H_
    
#ifdef VERBOSE_ON
#define VERBOSE_MSG(x) x
#else
#define VERBOSE_MSG(x)
#endif

#ifdef DEBUG_ON
#define DEBUG_MSG(x) x
#define DEBUG0 cpl_bit(PORTB, PB5);
#define DEBUG1 cpl_bit(PORTB, PB4);
#else
#define DEBUG_MSG(x)
#define DEBUG0
#define DEBUG1
#endif

#endif /* ifndef _DBG_VRB_H_ */
