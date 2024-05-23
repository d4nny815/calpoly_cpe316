/*
 * ADC.c
 *
 *  Created on: May 13, 2024
 *      Author: danny
 */

#include "ADC.h"

static volatile uint16_t value;
static volatile int flag;

void ADC_pin_init() {
    RCC->AHB2ENR |= RCC_AHB2ENR_GPIOAEN;
    GPIOA->MODER |= GPIO_MODER_MODE0; // Analog mode
    GPIOA->ASCR |= GPIO_ASCR_ASC0;  // Connect analog switch to ADC input

    return;
}


void ADC_init() {
//    ADC_pin_init();
//
//    RCC->AHB2ENR |= RCC_AHB2ENR_ADCEN;
//    ADC123_COMMON->CCR |= (1 << ADC_CCR_CKMODE_Pos); // HCLK / 1
//    ADC1->CR &= ~ADC_CR_DEEPPWD; // wake up from deep-power-down mode
//    ADC1->CR |= ADC_CR_ADVREGEN; // turn on voltage regulator
//    for (int _=0; _<40; _++); // wait for regulator to start up
//    // ! for 4MHz but will work for higher freqs
//
//    ADC1->CR &= ~ADC_CR_ADCALDIF; // single-ended calibration
//    ADC1->CR |= ADC_CR_ADCAL; // start calibration
//    while (ADC1->CR & ADC_CR_ADCAL); // wait for calibration to finish
//
//    ADC1->DIFSEL &= ~ADC_DIFSEL_DIFSEL_0; // single-ended mode
//
//    ADC1->ISR |= ADC_ISR_ADRDY; // clear ready flag
//    ADC1->CR |= ADC_CR_ADEN; // enable ADC
//    while (!(ADC1->ISR & ADC_ISR_ADRDY)); // wait for ADC to be ready
//
//    ADC1->CFGR = 0; // single conversion, right-aligned data, 12-bit resolution
//    // ADC1->OFR1 = (ADC_OFR1_OFFSET1_EN | 5 << ADC_OFR1_OFFSET1_CH_Pos | (3 << ADC_OFR1_OFFSET1_Pos)); // offset calibration
////    ADC1->CFGR = (ADC_CFGR_ALIGN); // single conversion, left-aligned data, 12-bit resolution
//    ADC1->SQR1 = (5 << ADC_SQR1_SQ1_Pos); // one conversion in sequence
////    ADC1->SMPR1 = (4 << ADC_SMPR1_SMP5_Pos); // 47.5 clock sample on channel 5
//    ADC1->SMPR1 = 0; // 2.5 clock sample on channel 5
//
//    ADC1->IER |= ADC_IER_EOCIE; // enable end-of-conversion interrupt
//    NVIC_EnableIRQ(ADC1_IRQn); // enable ADC interrupt
//    __enable_irq();
//
    ADC_pin_init();

        RCC->AHB2ENR |= RCC_AHB2ENR_ADCEN;
        ADC123_COMMON->CCR |= (1 << ADC_CCR_CKMODE_Pos); // HCLK / 1
        ADC1->CR &= ~ADC_CR_DEEPPWD; // wake up from deep-power-down mode
        ADC1->CR |= ADC_CR_ADVREGEN; // turn on voltage regulator
        for (int _=0; _<100; _++); // wait for regulator to start up
        // ! for 4MHz but will work for higher freqs

        ADC1->CR &= ~ADC_CR_ADCALDIF; // single-ended calibration
        ADC1->CR |= ADC_CR_ADCAL; // start calibration
        while (ADC1->CR & ADC_CR_ADCAL); // wait for calibration to finish

        ADC1->DIFSEL &= ~ADC_DIFSEL_DIFSEL_0; // single-ended mode

        ADC1->ISR |= ADC_ISR_ADRDY; // clear ready flag
        ADC1->CR |= ADC_CR_ADEN; // enable ADC
        while (!(ADC1->ISR & ADC_ISR_ADRDY)); // wait for ADC to be ready

        ADC1->CFGR = 0; // single conversion, right-aligned data, 12-bit resolution
        ADC1->SQR1 = (5 << ADC_SQR1_SQ1_Pos); // one conversion in sequence
        ADC1->SMPR1 = ~ADC_SMPR1_SMP5_Msk; // 2.5 clock sample on channel 5
    //    ADC1->SMPR1 = (7 << ADC_SMPR1_SMP5_Pos); // 640.5 clock sample on channel 5
    //    ADC1->SMPR1 = (4 << ADC_SMPR1_SMP5_Pos); // 47.5 clock sample on channel 5

        ADC1->IER |= ADC_IER_EOCIE; // enable end-of-conversion interrupt
        NVIC_EnableIRQ(ADC1_IRQn); // enable ADC interrupt
        __enable_irq();

        return;

    return;
}

uint16_t ADC_to_mv(uint16_t val) {
//    return val * VREF / ADC_REF;

    // This from the linear calibration but I noticed better accuracy with the ideal conversion
     int32_t uv = val * MAGIC_CAL_X - MAGIC_CAL_Y;
     return uv > 0 ? uv / UV_TO_MV : 0;
 }

void ADC1_2_IRQHandler() {
    value = ADC1->DR;
    flag = 1;
}

uint16_t get_ADC_val() {
    return value;
}

int ADC_check_flag() {
    return flag;
}

void ADC_clear_flag() {
    flag = 0;

    return;
}





