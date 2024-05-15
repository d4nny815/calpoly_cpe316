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
#define MAGIC_CAL_X 805
#define MAGIC_CAL_Y 5700
#define UV_TO_MV 1000

#define ADC_start_conversion() (ADC1->CR |= ADC_CR_ADSTART)
//#define ADC_to_mv(val) (uint16_t)(val * VREF * MAGIC_CAL_NUM / ADC_REF)
#include "stm32l476xx.h"

void ADC_pin_init(void);
void ADC_init(void);
// void ADC_start_conversion(void);
 uint16_t ADC_to_mv(uint16_t val);
#endif /* SRC_ADC_H_ */
