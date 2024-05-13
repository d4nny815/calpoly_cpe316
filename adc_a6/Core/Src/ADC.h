/*
 * ADC.h
 *
 *  Created on: May 13, 2024
 *      Author: danny
 */

#ifndef SRC_ADC_H_
#define SRC_ADC_H_

#define ADC_start_conversion() (ADC1->CR |= ADC_CR_ADSTART)

#include "stm32l476xx.h"

void ADC_pin_init(void);
void ADC_init(void);
// void ADC_start_conversion(void);

#endif /* SRC_ADC_H_ */
