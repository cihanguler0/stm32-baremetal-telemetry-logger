#include "ring_buffer.h"

void RingBuffer_Init(RingBuffer_t *rb) {

    if (rb == nullptr) {
        return;
    }

    rb->head = 0;
    rb->tail = 0;
}

bool RingBuffer_Pop(RingBuffer_t *rb, uint8_t *data) {

    if (data == nullptr || rb == nullptr) {
        return false;
    }

    if (rb->head == rb->tail) {
        return false;
    }

    *data = rb->buffer[rb->tail];
    rb->tail = (rb->tail + 1) & RING_BUFFER_MASK;

    return true;
}

bool RingBuffer_Push(RingBuffer_t *rb, uint8_t data) {

    if (rb == nullptr) {
        return false;
    }

    uint8_t next_head = (rb->head + 1) & RING_BUFFER_MASK;

    if (next_head == rb->tail) {
        return false;
    }

    rb->buffer[rb->head] = data;
    rb->head = next_head;

    return true;
}

bool RingBuffer_IsEmpty(const RingBuffer_t *rb) {

    if (rb == nullptr) {
        return true;
    }

    return (rb->head == rb->tail);
}

bool RingBuffer_IsFull(const RingBuffer_t *rb) {

    if (rb == nullptr) {
        return false;
    }

    return (((rb->head + 1) & RING_BUFFER_MASK) == rb->tail);
}

uint8_t RingBuffer_GetCount(const RingBuffer_t *rb) {
    
    if (rb == nullptr) {
        return 0;
    }
    
    return (rb->head - rb->tail) & RING_BUFFER_MASK;
}