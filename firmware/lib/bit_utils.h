/**
 * @file bit_utils.h
 *
 * @defgroup BIT_UTILS Bit Utils
 *
 * @brief The basic bitwork a little bit easier using this.
 *
 */
#ifndef _BIT_UTILS_H_
#define _BIT_UTILS_H_

#include <avr/sfr_defs.h>

// FROM AVR:  http://www.atmel.com/webdoc/avrlibcreferencemanual/ch20s22s02.html
// #define _BV(bit)                         (1 << (bit))
// #define bit_is_set(sfr,bit)              (_SFR_BYTE(sfr) & _BV(bit)) 
// #define bit_is_clear(sfr,bit)            (!(_SFR_BYTE(sfr) & _BV(bit)))
// #define loop_until_bit_is_clear(sfr,bit) do { } while (bit_is_set(sfr, bit))  
// #define loop_until_bit_is_set(sfr,bit)   do { } while (bit_is_clear(sfr, bit)) 

/**
 * @brief This method prevents from the error of expanding macros resulting in
 * (1<<0) from _BV(bit) from bit_is_set:
 */
#undef bit_is_set
#define bit_is_set(sfr, bit)        ((_SFR_BYTE(sfr) >> (bit)) & 1)

/**
 * @brief Changes the nth bit 'bit' to 1 of an address/variable 'y'.
 */
#define set_bit(y,bit)  (y|=(_BV(bit)))

/**
 * @brief Changes the nth bit 'bit' to 0 of an address/variable 'y'.
 */ 
#define clr_bit(y,bit)  (y&=~_BV(bit))

/**
 * @brief Changes the nth bit 'bit' of an address/variable 'y' to its logical complement (from 0 to 1, from 1 to 0).
 */ 
#define cpl_bit(y,bit)  (y^=(_BV(bit)))

/**
 * @brief Returns '1' if the nth bit 'bit' of an address/variable 'y' is '1',
 * 0 otherwise.
 */ 
#define tst_bit(y,bit)  (y&(1<<_BV(bit))) 

/**
 * @brief transfers a bit from x to y
 */
#define chg_bit(y,x,bit)      (y = y & ~(1 << (bit)) | (_SFR_BYTE(x) << (bit)));


#endif /* ifndef _BIT_UTILS_H_*/
