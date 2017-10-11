/**
 * @file adc.h
 *
 * @brief Este arquivo implementa todo o ADC, baseado numa máquina de estados
 * para realizar a multiplexação.
 *
 */

#ifndef _ADC_H_
#define _ADC_H_

#include "avr/io.h"
#include "avr/interrupt.h"
#include "../lib/bit_utils.h"
#include "dbg_vrb.h"
#include "../lib/cbuf.h"
#include "../lib/log2.h"

#define ADC_LAST_CHANNEL 4                  // quantidade de canais utilizados

typedef volatile enum adc_channels{ 
    ADC0, ADC1 ,ADC2, ADC3, ADC4, ADC5  
} adc_channels_t;                           // 
static adc_channels_t ADC_CHANNEL = ADC0;   // canal escolhido
uint8_t raw_adc[ADC_LAST_CHANNEL];          // vetor das leituras puras

void adc_select_channel(adc_channels_t __ch);
void adc_init(void);

// MOVING AVERAGE BELOW //
//
void init_buffers(void);

#define cbuf_adc0_SIZE          128       // size of buffers
#define cbuf_adc0_SIZE_2        7         // size of buffers in 2^n
volatile struct cbuf{
    uint8_t     m_getIdx;
    uint8_t     m_putIdx;
    uint8_t     m_entry[cbuf_adc0_SIZE];
} cbuf_adc0;

uint8_t ma_adc0(void);
uint8_t avg_adc0;

#endif /* ifndef _ADC_H_ */
