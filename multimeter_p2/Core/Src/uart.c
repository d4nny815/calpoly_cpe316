/*
 * uart.c
 *
 *  Created on: May 3, 2024
 *      Author: danny
 */

#include "uart.h"

static volatile int flag;
static volatile char value;

// configure the USART pins
void uart_pin_init(void) {
    RCC->AHB2ENR |= RCC_AHB2ENR_GPIOAEN;

    USART_PORT->AFR[0] &= ~(GPIO_AFRL_AFSEL2 | GPIO_AFRL_AFSEL3);
    USART_PORT->AFR[0] |= (USART_AF << GPIO_AFRL_AFSEL2_Pos | USART_AF << GPIO_AFRL_AFSEL3_Pos);

    USART_PORT->MODER &= ~(GPIO_MODER_MODE2 | GPIO_MODER_MODE3);
    USART_PORT->MODER |= (GPIO_MODER_MODE2_1 | GPIO_MODER_MODE3_1);
    USART_PORT->OSPEEDR |= (GPIO_OSPEEDR_OSPEED2 | GPIO_OSPEEDR_OSPEED3);
    USART_PORT->PUPDR &= ~(GPIO_PUPDR_PUPD2 | GPIO_PUPDR_PUPD3);
    USART_PORT->OTYPER &= ~(GPIO_OTYPER_OT2 | GPIO_OTYPER_OT3);
    return;
}

// configure the USART peripheral
void uart_init(void) {
    uart_pin_init();

    RCC->APB1ENR1 |= RCC_APB1ENR1_USART2EN;
    // 8-bit data, oversampling by 16, 1 stop bit, no parity bit, rx interrupt enabled
    USART2->CR1 = (USART_CR1_TE | USART_CR1_RE | USART_CR1_RXNEIE); // might not need rx intr
    USART2->BRR = USART_BRR;        // set baud rate

    NVIC_EnableIRQ(USART2_IRQn);
    __enable_irq();

    USART2->CR1 |= USART_CR1_UE;   // enable USART2

    uart_clear_screen();
    uart_send_escape("[?25l"); // hide cursor
    return;
}

void uart_send_char(char c) {
    while (!(USART2->ISR & USART_ISR_TXE));
    USART2->TDR = c;

    return;
}

void uart_send_string(const char* str) {
    while (*str != '\0') {
        uart_send_char(*str);
        str++;
    }

    return;
}

void uart_send_escape(const char* str) {
    uart_send_char(ESC_CHAR);
    uart_send_string(str);

    return;
}


void uart_clear_screen(void) {
    uart_send_escape("[2J");
    uart_send_escape("[H");

    return;
}

void USART2_IRQHandler(void) {
    value = USART2->RDR;
    flag = 1;
    return;
}

int uart_check_flag() {
    return flag;
}

void uart_clear_flag() {
    flag = 0;

    return;
}

char get_uart_char() {
    return value;
}
