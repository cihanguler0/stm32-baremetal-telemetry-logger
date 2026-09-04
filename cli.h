#ifndef CLI_H
#define CLI_H
#include "ring_buffer.h"

void CLI_Init(RingBuffer_t *rb);
void CLI_Process(void);

#endif 