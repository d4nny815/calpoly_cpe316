/*
 * DAC.c
 *
 *  Created on: Apr 21, 2024
 *      Author: danny
 */
#include "DAC.h"

// Configure GPIOs and SPI Regs
void SPI_init() {
    // PA4 - CS
    // PA5 - SCLK
    // PA6 - MISO
    // PA7 - MOSI

    RCC->AHB2ENR |= RCC_AHB2ENR_GPIOAEN;

    DAC_PORT->AFR[0] &= ~(GPIO_AFRL_AFSEL4 | GPIO_AFRL_AFSEL5 | GPIO_AFRL_AFSEL6 | GPIO_AFRL_AFSEL7);
    DAC_PORT->AFR[0] |= (SPI_AF_MODE << GPIO_AFRL_AFSEL4_Pos |
                        SPI_AF_MODE << GPIO_AFRL_AFSEL5_Pos |
                        SPI_AF_MODE << GPIO_AFRL_AFSEL6_Pos |
                        SPI_AF_MODE << GPIO_AFRL_AFSEL7_Pos);

    DAC_PORT->MODER &= ~(GPIO_MODER_MODE4 | GPIO_MODER_MODE5 | GPIO_MODER_MODE6 | GPIO_MODER_MODE7);
    DAC_PORT->MODER |= (GPIO_MODER_MODE4_1 | GPIO_MODER_MODE5_1 | GPIO_MODER_MODE6_1 | GPIO_MODER_MODE7_1);
    DAC_PORT->OTYPER &= ~(GPIO_OTYPER_OT4 | GPIO_OTYPER_OT5 | GPIO_OTYPER_OT6 | GPIO_OTYPER_OT7);
    DAC_PORT->PUPDR &= ~(GPIO_PUPDR_PUPD4 | GPIO_PUPDR_PUPD5 | GPIO_PUPDR_PUPD6 | GPIO_PUPDR_PUPD7);
    DAC_PORT->OSPEEDR |= (GPIO_OSPEEDR_OSPEED4 | GPIO_OSPEEDR_OSPEED5 | GPIO_OSPEEDR_OSPEED6 | GPIO_OSPEEDR_OSPEED7);


    RCC->APB2ENR |= RCC_APB2ENR_SPI1EN;

    // baudRate = f_clk / 2, 1st edge and idle low, full duplex, msb first, disable crc enable, no sw slave select, master config
    SPI1->CR1 = SPI_CR1_MSTR;  // master config

    // 16bit Data frame, SS output is enabled in master mode, NSS pulse
    SPI1->CR2 = (SPI_CR2_SSOE | SPI_CR2_NSSP | DATA_FRAME_16BIT << SPI_CR2_DS_Pos); 

    SPI1->CR1 |= SPI_CR1_SPE;   // enable SPI
    return;
}

void DAC_init() {
    SPI_init();
    uint16_t volt = volt_to_dac_val(2300);
    DAC_write(volt);
    return;
}

void DAC_write(uint16_t voltage) {
    if (voltage > MAX_DAC_VAL) voltage = MAX_DAC_VAL;
    
    while (!(SPI1->SR & SPI_SR_TXE));
    SPI1->DR = voltage;
}
