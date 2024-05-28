/*
 * i2c.c
 *
 *  Created on: May 24, 2024
 *      Author: danny
 */

#include "i2c.h"

void i2c_pin_init() {
    // PB8 - SCL
    // PB9 - SDA
    RCC->AHB2ENR |= RCC_AHB2ENR_GPIOBEN;

    GPIOB->AFR[1] &= ~(GPIO_AFRH_AFSEL8 | GPIO_AFRH_AFSEL9);
    GPIOB->AFR[1] |= (I2C_AF << GPIO_AFRH_AFSEL8_Pos | I2C_AF << GPIO_AFRH_AFSEL9_Pos);

    GPIOB->MODER &= ~(GPIO_MODER_MODE8 | GPIO_MODER_MODE9);
    GPIOB->MODER |= GPIO_MODER_MODE8_1 | GPIO_MODER_MODE9_1;
    GPIOB->PUPDR &= ~(GPIO_PUPDR_PUPD8 | GPIO_PUPDR_PUPD9);
    GPIOB->OTYPER |= GPIO_OTYPER_OT8 | GPIO_OTYPER_OT9;
    GPIOB->OSPEEDR |= GPIO_OSPEEDR_OSPEED8 | GPIO_OSPEEDR_OSPEED9;

    return;
}


void eeprom_init() {
    i2c_pin_init();

    RCC->APB1ENR1 |= RCC_APB1ENR1_I2C1EN;
    
    // clear PE
    I2C1->CR1 &= ~I2C_CR1_PE;

    // configure ANFOFF DNF
    I2C1->CR1 &= ~I2C_CR1_ANFOFF;
    I2C1->CR1 &= ~I2C_CR1_DNF;

    I2C1->TIMINGR = 0x2010091A; // taken from ioc

    // configure NOSTRETCH
    I2C1->CR1 &= ~I2C_CR1_NOSTRETCH;

    // set PE
    I2C1->CR1 |= I2C_CR1_PE;


    return;
}


void eeprom_store_byte(uint8_t data, uint16_t addr) {
    // start -> control -> upper addr -> lower addr -> data -> stop

    // configure i2c sending register
    while (I2C1->ISR & I2C_ISR_BUSY);
    I2C1->ICR |= I2C_ICR_STOPCF; // clear stop flag ? do i need this here?
    I2C1->CR2 = (ADDRING_MODE << I2C_CR2_ADD10_Pos) |
                (EEPROM_ADDR << (I2C_CR2_SADD_Pos + 1)) | // 7 bit addr in SADD[7:1]
                (0 << I2C_CR2_RD_WRN_Pos) | 
                (1 << I2C_CR2_START_Pos) | 
                (0 << I2C_CR2_AUTOEND_Pos) | 
                (BYTES_PER_STORE << I2C_CR2_NBYTES_Pos);

    // send addr upper byte
    while (!(I2C1->ISR & I2C_ISR_TXE));
    I2C1->TXDR = (addr >> 8) & 0xFF; // send addr upper byte

    // send addr lower byte
    while (!(I2C1->ISR & I2C_ISR_TXE));
    I2C1->TXDR = addr & 0xFF; // send addr lower byte

    // send data byte 
    while (!(I2C1->ISR & I2C_ISR_TXE));
    I2C1->TXDR = data; // send data byte

    // end of transmission
    while (!(I2C1->ISR & I2C_ISR_TC));
    I2C1->CR2 |= I2C_CR2_STOP;
    while (I2C1->ISR & I2C_ISR_STOPF);
    I2C1->ICR |= I2C_ICR_STOPCF;

    for (uint32_t i=0; i<(uint32_t)250e3; i++); // 5ms delay for write to process


    return;
}

uint8_t eeprom_load_byte(uint16_t addr) {
    uint8_t data;

    // configure i2c sending register
    while (I2C1->ISR & I2C_ISR_BUSY);
    I2C1->CR2 = (ADDRING_MODE << I2C_CR2_ADD10_Pos) |
                (EEPROM_ADDR << (I2C_CR2_SADD_Pos + 1)) |
                (0 << I2C_CR2_RD_WRN_Pos) | 
                (1 << I2C_CR2_START_Pos) | 
                (0 << I2C_CR2_AUTOEND_Pos) | 
                (BYTES_PER_ADDR << I2C_CR2_NBYTES_Pos);

    // send addr upper byte
    while (!(I2C1->ISR & I2C_ISR_TXE));
    I2C1->TXDR = (addr >> 8) & 0xFF;

    // send addr lower byte
    while (!(I2C1->ISR & I2C_ISR_TXE));
    I2C1->TXDR = addr & 0xFF;

    // wait
    while (!(I2C1->ISR & I2C_ISR_TC));

    // configure i2c reg to read
    I2C1->CR2 = (ADDRING_MODE << I2C_CR2_ADD10_Pos) |
                (EEPROM_ADDR << (I2C_CR2_SADD_Pos + 1)) |
                (1 << I2C_CR2_RD_WRN_Pos) | 
                (1 << I2C_CR2_START_Pos) | 
                (0 << I2C_CR2_AUTOEND_Pos) | 
                (BYTES_PER_LOAD << I2C_CR2_NBYTES_Pos);

    while (!(I2C1->ISR & I2C_ISR_RXNE));
    data = I2C1->RXDR;

    I2C1->CR2 |= I2C_CR2_NACK | I2C_CR2_STOP;

    return data;
}




