/*
 * usart.c
 *
 *  Created on: May 3, 2024
 *      Author: danny
 */

#include "usart.h"

// configure the USART pins
void usart_pin_init(void) {
    RCC->AHB2ENR |= RCC_AHB2ENR_GPIOAEN;

    USART_PORT->AFR[0] &= ~(GPIO_AFRL_AFSEL2 | GPIO_AFRL_AFSEL3);
    USART_PORT->AFR[0] |= (USART_AF << GPIO_AFRL_AFSEL2_Pos | USART_AF << GPIO_AFRL_AFSEL3_Pos);
    
    USART_PORT->MODER = ~(GPIO_MODER_MODE2 | GPIO_MODER_MODE3);
    USART_PORT->MODER |= (GPIO_MODER_MODE2_1 | GPIO_MODER_MODE3_1);
    USART_PORT->OSPEEDR |= (GPIO_OSPEEDR_OSPEED2 | GPIO_OSPEEDR_OSPEED3);
    USART_PORT->PUPDR &= ~(GPIO_PUPDR_PUPD2 | GPIO_PUPDR_PUPD3);
    USART_PORT->OTYPER &= ~(GPIO_OTYPER_OT2 | GPIO_OTYPER_OT3);
    return;
}

// configure the USART peripheral
void usart_init(void) {
    usart_pin_init();
    
    RCC->APB1ENR1 |= RCC_APB1ENR1_USART2EN;
    USART2->CR1 &= ~(USART_CR1_M1 | USART_CR1_M0);   // 8-bit data
    USART2->CR1 &= ~USART_CR1_OVER8;   // oversampling by 16
    USART2->BRR = USART_BRR;        // set baud rate
    USART2->CR2 &= ~USART_CR2_STOP;   // 1 stop bit
    USART2->CR1 |= USART_CR1_TE | USART_CR1_RE;   // enable transmitter and receiver
    // no parity bit
    
    // will have to enable rx intr later
    USART2->CR1 |= USART_CR1_RXNEIE;   // disable RXNE interrupt
    NVIC_EnableIRQ(USART2_IRQn);
    __enable_irq();

    USART2->CR1 |= USART_CR1_UE;   // enable USART2


    // clear screen and reset cursor
    usart_send_escape("[2J");
    usart_send_escape("[H");
    return;
}

void usart_send_char(char c) {
    while (!(USART2->ISR & USART_ISR_TXE));
    USART2->TDR = c;
    
    return;
}

void usart_send_string(const char* str) {
    while (*str != '\0') {
        usart_send_char(*str);
        str++;
    }
    
    return;
}

void usart_send_escape(const char* str) {
    usart_send_char(ESC_CHAR);
    usart_send_string(str);
    return;
}
