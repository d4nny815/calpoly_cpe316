/*
 * usart.h
 *
 *  Created on: May 3, 2024
 *      Author: danny
 */

#ifndef SRC_USART_H_
#define SRC_USART_H_
#include "stm32l476xx.h"

#define USART_PORT GPIOA
#define USART_AF 7
#define USART_BAUD 115200
#define CPU_FREQ 4000000
#define USART_BRR (int)(CPU_FREQ / USART_BAUD)
#define ESC_CHAR (char)0x1B
#define RED_KEY 'R'
#define BLUE_KEY 'B'
#define GREEN_KEY 'G'
#define WHITE_KEY 'W'

void usart_pin_init(void);
void usart_init(void);

void usart_send_char(char c);
void usart_send_string(const char* str);
void usart_send_escape(const char* str);

#endif /* SRC_USART_H_ */