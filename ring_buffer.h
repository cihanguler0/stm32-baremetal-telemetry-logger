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

void RingBuffer_Init(RingBuffer_t *rb);
bool RingBuffer_Pop(RingBuffer_t *rb, uint8_t *data);
bool RingBuffer_Push(RingBuffer_t *rb, uint8_t data);
bool RingBuffer_IsEmpty(const RingBuffer_t *rb);
bool RingBuffer_IsFull(const RingBuffer_t *rb);
uint8_t RingBuffer_GetCount(const RingBuffer_t *rb);

#endif