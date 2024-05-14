/*
 * ADC.h
 *
 *  Created on: May 13, 2024
 *      Author: danny
 */

#ifndef SRC_ADC_H_
#define SRC_ADC_H_

#define VREF 3300
#define ADC_REF (1 << 12) - 1

// #define ADC_start_conversion() (ADC1->CR |= ADC_CR_ADSTART)

#include "stm32l476xx.h"

void ADC_pin_init(void);
void ADC_init(void);
void ADC_start_conversion(void);
uint16_t adc_to_mv(uint16_t val);
#endif /* SRC_ADC_H_ */
