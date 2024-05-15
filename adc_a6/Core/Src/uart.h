/*
 * uart.h
 *
 *  Created on: May 3, 2024
 *      Author: danny
 */

#ifndef SRC_UART_H_
#define SRC_UART_H_
#include "stm32l476xx.h"

#define USART_PORT GPIOA
#define USART_AF 7
#define USART_BAUD 115200
#define CPU_FREQ (unsigned int)40e6
#define USART_BRR (int)(CPU_FREQ / USART_BAUD)
#define ESC_CHAR (char)0x1B

void uart_pin_init(void);
void uart_init(void);

void uart_send_char(char c);
void uart_send_string(const char* str);
void uart_send_escape(const char* str);
void uart_clear_screen(void);

#endif /* SRC_UART_H_ */
