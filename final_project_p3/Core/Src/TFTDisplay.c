/*
 * TFTDisplay.c
 *
 *  Created on: May 29, 2024
 *      Author: danny
 */

#include "TFTDisplay.h"

// Configure GPIOs and SPI Regs
void spi_init() {
    // PA4 - CS, SW controlled
    // PA5 - SCLK
    // PA6 - DC, SW controlled
    // PA7 - MOSI


    RCC->AHB2ENR |= RCC_AHB2ENR_GPIOAEN;

    GPIOA->AFR[0] &= ~(GPIO_AFRL_AFSEL5 | GPIO_AFRL_AFSEL7);
    GPIOA->AFR[0] |= (SPI_AF << GPIO_AFRL_AFSEL5_Pos |
                      SPI_AF << GPIO_AFRL_AFSEL7_Pos);

    GPIOA->MODER &= ~(GPIO_MODER_MODE4 | GPIO_MODER_MODE5 | GPIO_MODER_MODE6 | GPIO_MODER_MODE7); // clear mode
    GPIOA->MODER |= (GPIO_MODER_MODE5_1 | GPIO_MODER_MODE7_1);  // AF for mosi, sclk
    GPIOA->MODER |= (GPIO_MODER_MODE4_0 | GPIO_MODER_MODE6_0);  // digital out for cs, dc 
    GPIOA->OTYPER &= ~(GPIO_OTYPER_OT4 | GPIO_OTYPER_OT5 | GPIO_OTYPER_OT6 | GPIO_OTYPER_OT7); // push pull
    GPIOA->PUPDR &= ~(GPIO_PUPDR_PUPD4 | GPIO_PUPDR_PUPD5 | GPIO_PUPDR_PUPD6 | GPIO_PUPDR_PUPD7); // no res
    GPIOA->OSPEEDR |= (GPIO_OSPEEDR_OSPEED4 | GPIO_OSPEEDR_OSPEED5 | GPIO_OSPEEDR_OSPEED6 | GPIO_OSPEEDR_OSPEED7); // high speed

    RCC->APB2ENR |= RCC_APB2ENR_SPI1EN;

    // baudRate = f_clk / 2, 1st edge, idle low, 1-line bidirectional, output enabled (transmit only), msb first, no crc, software slave select, master
    SPI1->CR1 = 0;  // disable
    SPI1->CR1 &= ~(SPI_CR1_BR); // f_pclk / 2
    SPI1->CR1 &= ~(SPI_CR1_CPHA | SPI_CR1_CPOL); // 1st edge, idle low
    SPI1->CR1 |= SPI_CR1_BIDIMODE | SPI_CR1_BIDIOE; // 1-line bidirectional, output enabled
    SPI1->CR1 &= ~(SPI_CR1_LSBFIRST); // msb first
    SPI1->CR1 &= ~(SPI_CR1_CRCEN); // no crc
    SPI1->CR1 |= SPI_CR1_SSM; // software slave select
    SPI1->CR1 |= SPI_CR1_MSTR; // master



    // SPI1->CR1 = (SPI_CR1_BIDIMODE |
                // SPI_CR1_BIDIOE | 
                // SPI_CR1_CRCEN |
                // SPI_CR1_SSM |
                // SPI_CR1_MSTR);

    // 8bit data frame    
    SPI1->CR2 = (0x7 << SPI_CR2_DS_Pos);    

    SPI1->CR1 |= SPI_CR1_SPE;   // enable SPI
    
    return;
}

void display_init() {
    spi_init();

    uint8_t num_commands = ILI9341_cmds[0];
    uint8_t cmd = 0;
    uint8_t num_args = 0;
    uint8_t arg = 0;

    for (uint8_t i = 1; i < num_commands; i++) {
        cmd = ILI9341_cmds[i];
        num_args = ILI9341_cmds[++i];

        send_command(cmd);

        for (uint8_t j = 0; j < num_args; j++) {
            arg = ILI9341_cmds[++i];
            send_data(arg);
            delay_ms(250);
        }
    }


    return;
}


void send_command(uint8_t command) {
    COMMAND_DC();
    CS_L();
    spi_write(command);
    CS_H();

    return;
}

void send_data(uint8_t data) {
    DATA_DC();
    CS_L();
    spi_write(data);
    CS_H();

    return;
}

void spi_write(uint8_t data) {
    while (!(SPI1->SR & SPI_SR_TXE));
    SPI1->DR = data;

    return;
}

void delay_ms(uint32_t ms) {
    uint32_t cnt = CPU_FREQ / 1000 * ms;  
    while (cnt--);
    return;
}

