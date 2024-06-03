/*
 * eeprom.h
 *
 *  Created on: May 24, 2024
 *      Author: danny
 */

#ifndef SRC_EEPROM_H_
#define SRC_EEPROM_H_

#include "main.h"
#define I2C_AF (4)
#define EEPROM_ADDR (0x51)
#define BYTES_PER_STORE (3) // 2 bytes for address, 1 byte for data
#define BYTES_PER_ADDR (2) // 2 bytes for address
#define BYTES_PER_LOAD (1) // 1 byte for data
#define ADDRING_MODE (0) // 7-bit addressing
#define MAGIC_IOC_TIMING (0x300F38)


void i2c_pin_init(void);
void eeprom_init(void);
uint8_t eeprom_load_byte(uint16_t addr);
void eeprom_store_byte(uint8_t data, uint16_t addr);


#endif /* SRC_EEPROM_H_ */
