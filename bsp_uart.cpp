#include "bsp_uart.h"
#include <unistd.h>

#define PERIPH_BASE             (0x40000000UL)
#define APB2PERIPH_BASE         (PERIPH_BASE + 0x00010000UL)
#define AHB1PERIPH_BASE         (PERIPH_BASE + 0x00020000UL)

#define RCC_BASE                (AHB1PERIPH_BASE + 0x3800UL)
/* Controls clocks of peripherals connected to AHB 1*/
#define RCC_AHB1ENR             (*(volatile uint32_t *)(RCC_BASE + 0x30UL))
#define RCC_APB2ENR             (*(volatile uint32_t *)(RCC_BASE + 0x44UL))

#define GPIOA_BASE              (AHB1PERIPH_BASE + 0x0000UL)
#define GPIOA_MODER             (*(volatile uint32_t *)(GPIOA_BASE + 0x00UL))
#define GPIOA_AFRH              (*(volatile uint32_t *)(GPIOA_BASE + 0x24UL))

#define USART1_BASE             (APB2PERIPH_BASE + 0x1000UL)
#define USART1_SR               (*(volatile uint32_t *)(USART1_BASE + 0x00UL))
#define USART1_DR               (*(volatile uint32_t *)(USART1_BASE + 0x04UL))
#define USART1_BRR              (*(volatile uint32_t *)(USART1_BASE + 0x08UL))
#define USART1_CR1              (*(volatile uint32_t *)(USART1_BASE + 0x0CUL))

void UART1_Init(void) {

    /* *Bit 0, controls clock of GPIOA port (GPIOAEN) */
    RCC_AHB1ENR |= (1UL << 0);
    /* *Controls clock of USART1's own peripheral (USART1EN) */
    RCC_APB2ENR |= (1UL << 4);

    /* *This part controls together because we need double digit for MODER register.
       00 = input
       01 = output
       10 = alternate function
       11 = analog
       We chose 10 = alternate function because UART pins do not use GPIO's general output, they work with USART's
       peripherals */
    GPIOA_MODER &= ~((3UL << 18) | (3UL << 20));
    GPIOA_MODER |=  ((2UL << 18) | (2UL << 20));

    /* *This part is for defining "which alternate function we are using". */
    GPIOA_AFRH &= ~((0xFUL << 4) | (0xFUL << 8));
    GPIOA_AFRH |=  ((7UL << 4) | (7UL << 8));

    /* *BRR (Baud Rate Register)
       *This register controls communication speed of USART */
    USART1_BRR = 0x008A;

    /* *Bit 13 = 1 Enables USART's peripheral. 
       *Bit 3 = 1 Enables USART's transmitter. 
       *Bit 2 = 1 Enables USART's receiver. 
       *Bit 5 = 1 When RXNE flag is set, sends an interrupt signal (RXNEIE) */
    USART1_CR1 = (1UL << 13) | (1UL << 3) | (1UL << 2) | (1UL << 5);
}

void UART1_SendChar(char c) {
    while (!(USART1_SR & (1UL << 7)));
    USART1_DR = (uint8_t)c;
}

void UART1_SendString(const char *str) {
    if (!str) return;
    while (*str) {
        if (*str == '\n') {
            UART1_SendChar('\r');
        }
        UART1_SendChar(*str++);
    }
}

extern "C" int _write(int file, char *ptr, int len) {
    (void)file;
    for (int i = 0; i < len; i++) {
        if (ptr[i] == '\n') {
            UART1_SendChar('\r');
        }
        UART1_SendChar(ptr[i]);
    }
    return len;
}