/*
 * FuncGen.c
 *
 *  Created on: Apr 26, 2024
 *      Author: danny
 */
#include "FuncGen.h"

void square_wave_init(uint16_t freq, uint8_t duty_cycle) {
    RCC->APB1ENR1 |= RCC_APB1ENR1_TIM2EN;
    
    NVIC_DisableIRQ(TIM2_IRQn);
    TIM2->CR1 &= ~TIM_CR1_CEN;
    TIM2->CNT = 0;
    TIM2->SR &= ~(ARR_BIT | CCR1_BIT);  
    
    
    uint32_t max_arr = CPU_FREQ / freq;
    TIM2->ARR = max_arr - 1;
    TIM2->PSC = 0;
    TIM2->CCR1 = (uint32_t)(max_arr * ((float)duty_cycle / 100) - 1);
    TIM2->CCER |= TIM_CCER_CC1E;
    TIM2->DIER |= (TIM_DIER_UIE | TIM_DIER_CC1IE);

//    DBGMCU->APB1FZR1 |= 1;

    NVIC_EnableIRQ(TIM2_IRQn);
    __enable_irq();
    TIM2->CR1 |= TIM_CR1_CEN;
    return;
}

void continous_mode_init(void) {
    RCC->APB1ENR1 |= RCC_APB1ENR1_TIM2EN;
    
    NVIC_DisableIRQ(TIM2_IRQn);
    TIM2->CR1 &= ~TIM_CR1_CEN;
    TIM2->CNT = 0;
    TIM2->SR &= ~(ARR_BIT | CCR1_BIT);  
    
    
    TIM2->ARR = ARR_VAL;
    TIM2->PSC = 0;
    TIM2->DIER &= ~(TIM_DIER_UIE | TIM_DIER_CC1IE);
    TIM2->DIER |= TIM_DIER_UIE;

//    DBGMCU->APB1FZR1 |= 1;

    NVIC_EnableIRQ(TIM2_IRQn);
    __enable_irq();
    TIM2->CR1 |= TIM_CR1_CEN;
    return;

}

