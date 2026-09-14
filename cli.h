#ifndef CLI_H
#define CLI_H
#include "ring_buffer.h"
#include "fsm.h"

void CLI_Init(RingBuffer_t *rb, SystemContext_t *ctx);
void CLI_Process(void);

#endif 