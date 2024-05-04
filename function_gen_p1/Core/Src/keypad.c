/*
 * keypad.c
 *
 *  Created on: Apr 8, 2024
 *      Author: danny
 */
#include "keypad.h"


void keypad_init() {
    RCC->AHB2ENR |= RCC_AHB2ENR_GPIOCEN;
    // set rows as outputs
    // PC5-8
    KEYPAD_PORT->MODER &= ~(GPIO_MODER_MODE5 | GPIO_MODER_MODE6 | GPIO_MODER_MODE7 |GPIO_MODER_MODE8);
    KEYPAD_PORT->MODER |= (GPIO_MODER_MODE5_0 | GPIO_MODER_MODE6_0 | GPIO_MODER_MODE7_0 | GPIO_MODER_MODE8_0);  // gp output mode
    KEYPAD_PORT->OTYPER &= ~(GPIO_OTYPER_OT5 | GPIO_OTYPER_OT6 | GPIO_OTYPER_OT7 | GPIO_OTYPER_OT8);  // push-pull
    KEYPAD_PORT->OSPEEDR &= ~(GPIO_OSPEEDR_OSPEED5 | GPIO_OSPEEDR_OSPEED6 | GPIO_OSPEEDR_OSPEED7 | GPIO_OSPEEDR_OSPEED8);  // low speed
    KEYPAD_PORT->PUPDR &= ~(GPIO_PUPDR_PUPD5 | GPIO_PUPDR_PUPD6 | GPIO_PUPDR_PUPD7 | GPIO_PUPDR_PUPD8);   // no resistor

    // set cols as inputs
    // PC9-12
    KEYPAD_PORT->MODER &= ~(GPIO_MODER_MODE9 | GPIO_MODER_MODE10 | GPIO_MODER_MODE11 | GPIO_MODER_MODE12);    // input mode
    KEYPAD_PORT->PUPDR  &= ~(GPIO_PUPDR_PUPD9 | GPIO_PUPDR_PUPD10 | GPIO_PUPDR_PUPD11 | GPIO_PUPDR_PUPD12);
    KEYPAD_PORT->PUPDR  |= (GPIO_PUPDR_PUPD9_1 | GPIO_PUPDR_PUPD10_1 | GPIO_PUPDR_PUPD11_1 | GPIO_PUPDR_PUPD12_1); // pull down resistor

    // set all cols high
    keypad_setAllCols();

    return;
}

void keypad_setAllCols() {
    KEYPAD_PORT->ODR |= ROW_MASK;
    return;
}

void delay(uint32_t n) {
    for (uint32_t delay_cnt=0; delay_cnt<n; delay_cnt++);
    return;
}

int8_t keypad_read() {
    if (!(GPIOC->IDR & COL_MASK)) {
        return KEYPAD_NO_PRESS;
    }
    static const uint8_t btns[NUM_ROWS][NUM_COLS] = {{1, 2, 3, KEYPAD_A}, {4, 5, 6, KEYPAD_B}, {7, 8, 9, KEYPAD_C}, {KEYPAD_STAR, 0, KEYPAD_POUND, KEYPAD_D}};
    uint8_t col, row;
    for (uint32_t i=0; i<NUM_ROWS; i++) {
        GPIOC->ODR = (GPIOC->ODR & ~ROW_MASK) | (1 << (i + ROW_PIN_START));
        delay(5);
        if (!(GPIOC->IDR & COL_MASK)) continue;
        row = i;
        col = col_pins_to_int(COL_PINS);
        keypad_setAllCols();
        return btns[row][col];
    }
    keypad_setAllCols();
    return KEYPAD_NO_PRESS;
}

uint8_t col_pins_to_int(uint8_t pins) {
    switch (pins) {
        case COL1: return 0;
        case COL2: return 1;
        case COL3: return 2;
        case COL4: return 3;
    }
    return 0;
}

int8_t keypad_read_oneshot() {
    int8_t key;
    int8_t prev_key = KEYPAD_NO_PRESS;
    while (1) {
        key = keypad_read();
        delay(5000); // terrible way to debounce
        
        // one shot key press 
        if (prev_key != KEYPAD_NO_PRESS && key == KEYPAD_NO_PRESS) {
            return prev_key;
        }
        if (key == KEYPAD_NO_PRESS) continue;
        prev_key = key;
    }
}





