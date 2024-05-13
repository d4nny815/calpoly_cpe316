/*
 * ADC.c
 *
 *  Created on: May 13, 2024
 *      Author: danny
 */

#include "ADC.h"

void ADC_pin_init() {
    RCC->AHB2ENR |= RCC_AHB2ENR_GPIOAEN;
    GPIOA->MODER |= GPIO_MODER_MODE0; // Analog mode
    GPIOA->ASCR |= GPIO_ASCR_ASC0;  // Connect analog switch to ADC input

    return;
}


void ADC_init() {
    ADC_pin_init();

    RCC->AHB2ENR |= RCC_AHB2ENR_ADCEN;
    ADC123_COMMON->CCR |= (1 << ADC_CCR_CKMODE_Pos); // HCLK / 1
    ADC1->CR &= ~ADC_CR_DEEPPWD; // wake up from deep-power-down mode
    ADC1->CR |= ADC_CR_ADVREGEN; // turn on voltage regulator
    for (int _=0; _<100; _++); // wait for regulator to start up (!only works for 4MHz)

    ADC1->CR &= ~ADC_CR_ADCALDIF; // single-ended calibration
    ADC1->CR |= ADC_CR_ADCAL; // start calibration
    while (ADC1->CR & ADC_CR_ADCAL); // wait for calibration to finish

    ADC1->DIFSEL &= ~ADC_DIFSEL_DIFSEL_0; // single-ended mode

    ADC1->ISR |= ADC_ISR_ADRDY; // clear ready flag
    ADC1->CR |= ADC_CR_ADEN; // enable ADC
    while (!(ADC1->ISR & ADC_ISR_ADRDY)); // wait for ADC to be ready

    ADC1->CFGR = 0; // single conversion, right-aligned data, 12-bit resolution
    ADC1->SQR1 = (5 << ADC_SQR1_SQ1_Pos); // one conversion in sequence
    ADC1->SMPR1 = (2 << ADC_SMPR1_SMP5_Pos); // 12.5 clock sample on channel 5

    ADC1->IER |= ADC_IER_EOCIE; // enable end-of-conversion interrupt
    NVIC_EnableIRQ(ADC1_IRQn); // enable ADC interrupt
    __enable_irq();

    return;
}

// ! make this a MACRO
// void ADC_start_conversion() {
    // ADC1->CR |= ADC_CR_ADSTART; 
    // return;
// }

