/*
 * Joystick.c
 *
 *  Created on: Jun 1, 2024
 *      Author: danny
 */

#include "Joystick.h"

void joystick_pin_init() {
    // PC0 for vertical, PC1 for horizontal
    // PC2 for button
    RCC->AHB2ENR |= RCC_AHB2ENR_GPIOCEN;
    GPIOC->MODER |= GPIO_MODER_MODE0 | GPIO_MODER_MODE1; // Analog mode
    GPIOC->ASCR |= GPIO_ASCR_ASC0 | GPIO_ASCR_ASC1;  // Connect analog switch to ADC input

    GPIOC->MODER &= ~GPIO_MODER_MODE2; // Input mode
    GPIOC->PUPDR &= ~GPIO_PUPDR_PUPD2; 
    GPIOC->PUPDR |= GPIO_PUPDR_PUPD2_0; // Pull-up
    GPIOC->OTYPER &= ~GPIO_OTYPER_OT2; // Push-pull
    GPIOC->OSPEEDR &= ~GPIO_OSPEEDR_OSPEED2; // Low speed

    return;
}

void ADC_init() {

    // ADC1, IN1 for vertical
    RCC->AHB2ENR |= RCC_AHB2ENR_ADCEN;
    ADC123_COMMON->CCR |= (1 << ADC_CCR_CKMODE_Pos); // HCLK / 1
    ADC1->CR &= ~ADC_CR_DEEPPWD; // wake up from deep-power-down mode
    ADC1->CR |= ADC_CR_ADVREGEN; // turn on voltage regulator
    for (int _=0; _<100; _++); // wait for regulator to start up 

    ADC1->CR &= ~ADC_CR_ADCALDIF; // single-ended calibration
    ADC1->CR |= ADC_CR_ADCAL; // start calibration
    while (ADC1->CR & ADC_CR_ADCAL); // wait for calibration to finish

    ADC1->DIFSEL &= ~ADC_DIFSEL_DIFSEL_0; // single-ended mode

    ADC1->ISR |= ADC_ISR_ADRDY; // clear ready flag
    ADC1->CR |= ADC_CR_ADEN; // enable ADC
    while (!(ADC1->ISR & ADC_ISR_ADRDY)); // wait for ADC to be ready

    ADC1->CFGR = 0; // single conversion, right-aligned data, 12-bit resolution
    ADC1->SQR1 = (1 << ADC_SQR1_SQ1_Pos); // 1 conversion in regular sequence
    ADC1->SMPR1 = (2 << ADC_SMPR1_SMP1_Pos); // 12.5 cycles sampling time

    // ADC3, IN2 for horizontal
    RCC->AHB2ENR |= RCC_AHB2ENR_ADCEN;
    ADC123_COMMON->CCR |= (1 << ADC_CCR_CKMODE_Pos); // HCLK / 1
    ADC3->CR &= ~ADC_CR_DEEPPWD; // wake up from deep-power-down mode
    ADC3->CR |= ADC_CR_ADVREGEN; // turn on voltage regulator
    for (int _=0; _<100; _++); // wait for regulator to start up

    ADC3->CR &= ~ADC_CR_ADCALDIF; // single-ended calibration
    ADC3->CR |= ADC_CR_ADCAL; // start calibration
    while (ADC3->CR & ADC_CR_ADCAL); // wait for calibration to finish

    ADC3->DIFSEL &= ~ADC_DIFSEL_DIFSEL_0; // single-ended mode

    ADC3->ISR |= ADC_ISR_ADRDY; // clear ready flag
    ADC3->CR |= ADC_CR_ADEN; // enable ADC
    while (!(ADC3->ISR & ADC_ISR_ADRDY)); // wait for ADC to be ready

    ADC3->CFGR = 0; // single conversion, right-aligned data, 12-bit resolution
    ADC3->SQR1 = (2 << ADC_SQR1_SQ1_Pos); // 1 conversion in regular sequence
    ADC3->SMPR1 = (2 << ADC_SMPR1_SMP2_Pos); // 12.5 cycles sampling time

    return;
}

void joystick_init() {
    joystick_pin_init();
    ADC_init();

    return;
}

uint16_t get_joystick_x() {
    ADC3->CR |= ADC_CR_ADSTART; // start conversion
    while (!(ADC3->ISR & ADC_ISR_EOC)); // wait for conversion to finish

    return ADC3->DR;
    
}

uint16_t get_joystick_y() {
    ADC1->CR |= ADC_CR_ADSTART; // start conversion
    while (!(ADC1->ISR & ADC_ISR_EOC)); // wait for conversion to finish

    return ADC1->DR;
}

uint8_t get_joystick_button() {
    return !(GPIOC->IDR & GPIO_IDR_ID2);
}






