#include "ring_buffer.h"
#include "fsm.h"
#include "cli.h"
#include "bsp_uart.h"

/* *We use the E address because SysTick is not in peripheral area, it's in core area. */
#define SYSTICK_BASE            (0xE000E010UL)
/* *Controls SysTick. */
#define SYSTICK_CTRL            (*(volatile uint32_t *)(SYSTICK_BASE + 0x00UL))
/* *Controls period. */
#define SYSTICK_LOAD            (*(volatile uint32_t *)(SYSTICK_BASE + 0x04UL))
/* *Holds current value of register and countdowns */
#define SYSTICK_VAL             (*(volatile uint32_t *)(SYSTICK_BASE + 0x08UL))

/* *Address is on APB2 Bus, peripheral side. */
#define USART1_BASE             (0x40011000UL)
/* *Holds flag. */
#define USART1_SR               (*(volatile uint32_t *)(USART1_BASE + 0x00UL))
/* *Reads & Writes bytes. */
#define USART1_DR               (*(volatile uint32_t *)(USART1_BASE + 0x04UL))

static RingBuffer_t rx_buffer;
/* *For using states */
static SystemContext_t sys_context;
/* *Global counter for Systick Interrupt */
static volatile uint32_t g_system_tick_ms = 0;


/* *This one is defined and written in startup code of ARM Cortex-M*/
extern "C" void SysTick_Handler(void) {
    g_system_tick_ms++;
}

/* *This one is defined and written in startup code of ARM Cortex-M*/
/* *This one is for triggering IRQHandler when anny error happens. We decide which error handler to rise
   by checking the flag number. */
extern "C" void USART1_IRQHandler(void) {

    /* *Read Data Register Not Empty (5 = RXNE) */
    if (USART1_SR & (1UL << 5)) { 
        /* We use this part in if block, otherwise we would read a meaningless/unnecessary data. */
        uint8_t byte = (uint8_t)(USART1_DR & 0xFF);
        RingBuffer_Push(&rx_buffer, byte);
    }
}

static void SysTick_Init(void) {
    /* *16MHz in internal clock means 1ms period = 16.000 cycles. */  
    SYSTICK_LOAD = 16000UL - 1UL;
    /* Fresh Start */
    SYSTICK_VAL  = 0UL;
    /* *Bit 0 = 1 -> Enables SysTick counter. */
    /* *Bit 1 = 1 -> Enables Tick Interrupt (TICKINT). With this, when counter reaches 0 it produces an interrupt. */
    /* *Bit 2 = 1 -> Enables core clock. */
    SYSTICK_CTRL = (1UL << 2) | (1UL << 1) | (1UL << 0);
}

int main(void) {
    SysTick_Init();
    UART1_Init();
    RingBuffer_Init(&rx_buffer);
    FSM_Init(&sys_context);
    CLI_Init(&rx_buffer, &sys_context);

    while (1) {
        CLI_Process();
        FSM_Update(&sys_context, g_system_tick_ms);
    }

    return 0;
}