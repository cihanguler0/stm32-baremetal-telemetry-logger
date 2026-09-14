#ifndef BSP_UART_H
#define BSP_UART_H

#include <stdint.h>

void UART1_Init(void);
void UART1_SendChar(char c);
void UART1_SendString(const char *str);

#endif