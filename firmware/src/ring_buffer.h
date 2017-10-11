/**
 * @file ring_buffer.h
 *
 * @brief This implements a ring buffer to be used as Moving Average.
 *
 */

#ifndef RING_BUFFER_H
#define RING_BUFFER_H value

#include <avr/io.h>
#include <stdlib.h>     // using malloc :'(

#include "../lib/bit_utils.h"
#include "../lib/log2.h"

struct ring_buffer{
    uint8_t   *head;
    uint8_t   size;
    uint8_t   log2_size;
    uint8_t   used_size;
    uint8_t   write;
};

typedef volatile struct ring_buffer ring_buffer_t;

ring_buffer_t *ring_buffer_create(uint8_t size);
void ring_buffer_reset(ring_buffer_t *rb);
void ring_buffer_queue(ring_buffer_t *rb, uint8_t data);
uint8_t ring_buffer_mean(ring_buffer_t *rb);

#endif /* ifndef RING_BUFFER_H */
