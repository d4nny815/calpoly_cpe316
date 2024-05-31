/*
 * RNG.c
 *
 *  Created on: May 30, 2024
 *      Author: danny
 */

#include "RNG.h"

static void MX_RNG_Init(void);
RNG_HandleTypeDef hrng;


static void MX_RNG_Init(void) {

    hrng.Instance = RNG;
    if (HAL_RNG_Init(&hrng) != HAL_OK) {
        Error_Handler();
    }
}

static uint32_t value;

void rng_init() {
    MX_RNG_Init();


    // Enable AHB RNG clock
    RCC->AHB2ENR |= RCC_AHB2ENR_RNGEN;

    // enable rng_clk pll


    return;
}

uint32_t get_random(uint32_t n) {
    RNG->CR |= RNG_CR_RNGEN;

    while (!(RNG->SR & RNG_SR_DRDY));

    value = RNG->DR;

    return value % n;
}
