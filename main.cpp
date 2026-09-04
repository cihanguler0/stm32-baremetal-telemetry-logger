#include "ring_buffer.h"
#include "cli.h"

static RingBuffer_t rx_buffer;

int main(void) {

    RingBuffer_Init(&rx_buffer);
    CLI_Init(&rx_buffer);

    while (1) {
        CLI_Process();      
    }

    return 0;
}