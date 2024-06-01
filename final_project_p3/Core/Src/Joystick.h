/*
 * Joystick.h
 *
 *  Created on: Jun 1, 2024
 *      Author: danny
 */

#ifndef SRC_JOYSTICK_H_
#define SRC_JOYSTICK_H_

#include "main.h"

#define VREF (3300)
#define ADC_REF ((1 << 12) - 1)
#define ADC_TO_MV(x) (x * VREF / ADC_REF)

void joystick_pin_init();
void ADC_init();
void joystick_init();
uint16_t get_joystick_x();
uint16_t get_joystick_y();
uint8_t get_joystick_button();

#endif /* SRC_JOYSTICK_H_ */
