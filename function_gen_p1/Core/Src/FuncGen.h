/*
 * FuncGen.h
 *
 *  Created on: Apr 26, 2024
 *      Author: danny
 */

#ifndef SRC_FUNCGEN_H_
#define SRC_FUNCGEN_H_

#include "stm32l476xx.h"
#include "Waveform_LUT.h"
#include "keypad.h"

#define MAX_FUNC_VOLTAGE 3000
#define MIN_FUNC_VOLTAGE 0
#define ARR_BIT TIM_SR_UIF
#define CCR1_BIT TIM_SR_CC1IF
#define TIM_INTR_FLAGS ARR_BIT | CCR1_BIT
#define DEFAULT_FREQ 100
#define DEFAULT_DC 50
#define MIN_DC 10
#define MAX_DC 90
#define FREQ_MIN 100
#define SINE_KEY 6
#define SAWTOOTH_KEY 7
#define TRIANGLE_KEY 8
#define SQUARE_KEY 9

#define isFreqChangePin(x) ((uint8_t)x >= 1 && (uint8_t)x <= 5)
#define isWaveChangePin(x) ((uint8_t)x >= 6 && (uint8_t)x <= 9)
#define isDutyChangePin(x) ((uint8_t)x == 0 || (uint8_t)x == KEYPAD_STAR || (uint8_t)x == KEYPAD_POUND)


void square_wave_init(uint16_t freq, uint8_t duty_cycle);
void continous_mode_init(void);


#endif /* SRC_FUNCGEN_H_ */
