/*
 * ADC.h
 *
 *  Created on: May 13, 2024
 *      Author: danny
 */

#ifndef SRC_ADC_H_
#define SRC_ADC_H_

#include "main.h"

#define VREF 3300
#define ADC_REF (1 << 12) - 1
#define MAGIC_CAL_X 812
#define MAGIC_CAL_Y 46000
#define UV_TO_MV 1000

#define ADC_start_conversion() (ADC1->CR |= ADC_CR_ADSTART)
// #define ADC_to_mv(val) (uint16_t)(val * VREF * MAGIC_CAL_NUM / ADC_REF)


void ADC_pin_init(void);
void ADC_init(void);
uint16_t ADC_to_mv(uint16_t val);
uint16_t get_ADC_val();
int ADC_check_flag();
void ADC_clear_flag();

#endif /* SRC_ADC_H_ */
