#ifndef _BIT_UTILS_H_
#define _BIT_UTILS_H_

// FROM AVR:  http://www.atmel.com/webdoc/avrlibcreferencemanual/ch20s22s02.html
// #define bit_is_set(sfr,bit)              (_SFR_BYTE(sfr) & _BV(bit)) 
// #define bit_is_clear(sfr,bit)            (!(_SFR_BYTE(sfr) & _BV(bit)))
// #define loop_until_bit_is_clear(sfr,bit) do { } while (bit_is_set(sfr, bit))  
// #define loop_until_bit_is_set(sfr,bit)   do { } while (bit_is_clear(sfr, bit)) 
// #define _BV(bit)                         (1 << (bit))

#define set_bit(y,bit)  (y|=(1<<(bit)))     //coloca em 1 o bit x da variável Y
#define clr_bit(y,bit)  (y&=~(1<<(bit)))    //coloca em 0 o bit x da variável Y
#define cpl_bit(y,bit)  (y^=(1<<(bit)))     //troca o estado lógico do bit x da variável Y
#define tst_bit(y,bit)  (y&(1<<(bit)))      //retorna 0 ou 1 conforme leitura do bit

#endif /* ifndef _BIT_UTILS_H_*/
