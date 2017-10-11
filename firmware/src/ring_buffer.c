#include "ring_buffer.h"

/**
 * @brief 
 * 
 */
ring_buffer_t *ring_buffer_create(uint8_t size)
{
    ring_buffer_t *rb;
    uint8_t *vet;
    uint8_t log = 0;

    rb = (ring_buffer_t *)malloc(sizeof(ring_buffer_t));
    vet = (uint8_t *)malloc(size*sizeof(uint8_t));

#ifdef POINTER_CHECK_ON
    if((rb == NULL)||(vet == NULL)){
        error_flags.allocation_failure = 1;
        return NULL;
    }
#endif

    log2(log,size);

    rb->head = vet;
    rb->size = size;
    rb->log2_size = log;
    rb->used_size = 0;
    rb->write = 0;

    return rb;
}


/**
* @brief
*/ 
void ring_buffer_reset(ring_buffer_t *rb)
{

#ifdef POINTER_CHECK_ON
    if(rb == NULL){
        error_flags.invalid_pointer = 1;
        return;
    }
#endif

    rb->used_size = 0;
    rb->write = 0;
}


/**
* @brief
*/ 
void ring_buffer_queue(ring_buffer_t *rb, uint8_t data)
{
#ifdef POINTER_CHECK_ON
    if(rb == NULL){
        error_flags.invalid_pointer = 1;
        return;
    }
#endif

    uint8_t *vet = rb->head;

    vet[rb->write] = data;

    if(rb->write == (rb->size -1)){
        rb->write = 0;
    }else{
        rb->write = rb->write + 1;
    }
}


/**
* @brief
*/ 
uint8_t ring_buffer_mean(ring_buffer_t *rb)
{
#ifdef POINTER_CHECK_ON
    if(rb == NULL){
        error_flags.invalid_pointer = 1;
        return 255;
    }
#endif

    if(rb->used_size != rb->size){
        return 255;
    }

    uint16_t acum = 0;
    uint8_t *vet = rb->head;
    uint8_t i;

    for(i = rb->size - 1; i != 0; i--){
        acum += vet[i];
    }

    return (uint8_t)(acum>>rb->log2_size);
}

