/*
 * timer.c
 *
 *  Created on: May 16, 2024
 *      Author: danny
 */
#include "timer.h"

void timer_init(uint32_t freq) {
    RCC->APB1ENR1 |= RCC_APB1ENR1_TIM2EN;
    
    TIM2->ARR = CPU_FREQ / freq - 1;
    TIM2->PSC = 0;
    TIM2->DIER |= TIM_DIER_UIE;

//    DBGMCU->APB1FZR1 |= 1;

    NVIC_EnableIRQ(TIM2_IRQn);
    __enable_irq();
    TIM2->CR1 |= TIM_CR1_CEN;
    
    return;
}


