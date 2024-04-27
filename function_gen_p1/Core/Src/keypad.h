/*
 * keypad.h
 *
 *  Created on: Apr 8, 2024
 *      Author: danny
 */
#ifndef SRC_KEYPAD_H_
#define SRC_KEYPAD_H_
#include "stm32l476xx.h"

#define NUM_ROWS 4
#define NUM_COLS 4
#define ROW_MASK (GPIO_ODR_OD5 | GPIO_ODR_OD6 | GPIO_ODR_OD7 | GPIO_ODR_OD8)
#define COL_MASK (GPIO_IDR_ID9 | GPIO_IDR_ID10 | GPIO_IDR_ID11 | GPIO_IDR_ID12)
#define ROW_PIN_START GPIO_ODR_OD5_Pos
#define KEYPAD_NO_PRESS (int8_t)-1
#define KEYPAD_STAR 10
#define KEYPAD_POUND 11
#define KEYPAD_A 12
#define KEYPAD_B 13
#define KEYPAD_C 14
#define KEYPAD_D 15
#define COL1 0x1
#define COL2 0x2
#define COL3 0x4
#define COL4 0x8
#define COL_PINS (KEYPAD_PORT->IDR & COL_MASK) >> 9 // 9 is the first col pin
#define KEYPAD_PORT GPIOC

void keypad_init();
int8_t keypad_read();
int8_t keypad_read_oneshot();
void keypad_setAllCols();
uint8_t col_pins_to_int(uint8_t pins);
void delay(uint32_t n);

#endif /* SRC_KEYPAD_H_ */
