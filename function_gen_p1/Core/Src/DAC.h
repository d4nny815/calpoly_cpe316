/*
 * DAC.h
 *
 *  Created on: Apr 21, 2024
 *      Author: danny
 */

#ifndef SRC_DAC_H_
#define SRC_DAC_H_

#include "stm32l476xx.h"

#define DAC_PORT GPIOA
#define DAC_CONFIG_MASK 0x3 << 12
#define MASK_12BIT 0xfff
#define SPI_AF_MODE 5
#define DATA_FRAME_16BIT 0xf
#define MAX_VOLTAGE 3300
#define volt_to_dac_val(voltage) (uint16_t)(DAC_CONFIG_MASK | (MASK_12BIT & (uint16_t)((double)voltage / MAX_VOLTAGE * MASK_12BIT)))


void SPI_init();
void DAC_init();
void DAC_write(uint16_t voltage);

#endif /* SRC_DAC_H_ */
