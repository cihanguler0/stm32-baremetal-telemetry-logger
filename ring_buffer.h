#ifndef RING_BUFFER_H
#define RING_BUFFER_H
#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>
#define RING_BUFFER_SIZE 64
#define RING_BUFFER_MASK (RING_BUFFER_SIZE - 1)

typedef struct {

    uint8_t buffer[RING_BUFFER_SIZE];
    volatile uint8_t head;
    volatile uint8_t tail;

} RingBuffer_t;

void RingBuffer_Init(RingBuffer_t *rb) {             //Restarting the code

    if (rb == NULL) {
        return;
    }

    rb->head = 0;
    rb->tail = 0;

}

bool RingBuffer_Pop(RingBuffer_t *rb, uint8_t *data) {

    if (data == NULL || rb == NULL) {
        return false;
    }

    if (rb->head == rb->tail) {
        return false;
    }

    *data = rb->buffer[rb->tail];
    rb->tail = (rb->tail + 1)  & RING_BUFFER_MASK;

    return true;
    
}

bool RingBuffer_Push(RingBuffer_t *rb, uint8_t data) {

    if ( rb == NULL) {
        return false;
    }

    uint8_t next_head = (rb->head + 1) & RING_BUFFER_MASK;

    if ( next_head == rb->tail ) {
        return false;
    }

    rb->buffer[rb->head] = data;
    rb->head = next_head;

    return true;
}

bool RingBuffer_IsEmpty (const RingBuffer_t *rb) {

    if (rb == NULL) {
        return true; 
    }

    return (rb->head == rb->tail);
}

bool RingBuffer_IsFull (const RingBuffer_t *rb) {

    if (rb == NULL) {
        return false; 
    }

    return ((rb->head + 1) & RING_BUFFER_MASK) == rb->tail;

}

uint8_t RingBuffer_GetCount(const RingBuffer_t *rb) {

    return (rb->head - rb->tail) & RING_BUFFER_MASK;

}

#endif