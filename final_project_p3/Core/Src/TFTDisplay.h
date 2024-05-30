/*
 * TFTDisplay.h
 *
 *  Created on: May 29, 2024
 *      Author: danny
 */

#ifndef SRC_TFTDISPLAY_H_
#define SRC_TFTDISPLAY_H_

#include "main.h"
#define SPI_AF (5)

#define CS_PIN (GPIO_PIN_4) // ! placeholder for now
#define DC_PIN (GPIO_PIN_6) // ! placeholder for now

#define DC_L() (GPIOA->BRR = DC_PIN)
#define DC_H() (GPIOA->BSRR = DC_PIN)
#define COMMAND_DC() (DC_L())
#define DATA_DC() (DC_H())
#define CS_L() (GPIOA->BRR = CS_PIN)
#define CS_H() (GPIOA->BSRR = CS_PIN)

#define DISPLAY_WIDTH (240)
#define DISPLAY_HIGHT (320)

// INITIALIZE COMMANDS
#define SW_RESET       (0x01)
#define INIT_DELAY     (0x80)
#define PW_CNTRL1      (0xC0)
#define PW_CNTRL2      (0xC1)
#define VCOM_CNTRL1    (0xC5)
#define VCOM_CNTRL2    (0xC7)
#define MEM_ACC_CNTRL  (0x36)
#define PIX_FORMAT     (0x3A)
#define FRM_CNTRL1     (0xB1)
#define DISP_FUNC      (0xB6)
#define GAMMASET       (0x26)
#define GAMMA_POS      (0xE0)
#define GAMMA_NEG      (0xE1)
#define SLEEP_OUT      (0x11)
#define DISP_ON        (0x29)

// DRAW COMMANDS
#define COL_ADDR_SET   (0x2A)
#define ROW_ADDR_SET   (0x2B)
#define MEM_WR         (0x2C)
#define COLOR_SET      (0x2D)


static const uint8_t ILI9341_cmds[] = {
	22,                             // Number of commands 
	SW_RESET, INIT_DELAY,           // 1
	5,
	0xEF, 3,                        // 2
	0x03, 0x80, 0x02,
	0xCF, 3,                        // 3
	0x00, 0xC1, 0x30,
	0xED, 4,                        // 4
	0x64, 0x03, 0x12, 0x81,
	0xE8, 3,                        // 5
	0x85, 0x00, 0x78,
	0xCB, 5,                        // 6
	0x39, 0x2C, 0x00, 0x34, 0x02,
	0xF7, 1,                        // 7
	0x20,
	0xEA, 2,                        // 8
	0x00, 0x00,
	PW_CNTRL1, 1,                   // 9 power control 
	0x23,                           // VRH[5:0]      
	PW_CNTRL2, 1,                   // 10 power control 
	0x10,                           // SAP[2:0];BT[3:0]  
	VCOM_CNTRL1, 2,                 // 11 VCM control 
	0x3e, 0x28,
	VCOM_CNTRL2, 1,                 // 12 VCM control2 
	0x86,                           // --
	MEM_ACC_CNTRL, 1,               // 13
    (0x40 | 0x08),
    PIX_FORMAT, 1,                  // 14
    0x55,
    FRM_CNTRL1, 2,                  // 15
    0x00, 0x18,
    DISP_FUNC, 3,                   // 16
    0x08, 0x82, 0x27,
    0xF2, 1,                        // 17 3Gamma Function Disable 
    0x00,
    GAMMASET, 1,                    // 18 Gamma curve selected 
    0x01,
	GAMMA_POS, 15,                  // 19 Set Gamma 
	0x0F, 0x31, 0x2B, 0x0C, 0x0E, 0x08, 0x4E, 0xF1, 0x37, 0x07, 0x10, 0x03, 0x0E, 0x09, 0x00,
	GAMMA_NEG, 15,                  // 20
	0x00, 0x0E, 0x14, 0x03, 0x11, 0x07, 0x31, 0xC1, 0x48, 0x08, 0x0F, 0x0C, 0x31, 0x36, 0x0F,
	SLEEP_OUT, INIT_DELAY,          // 21
	120,
	DISP_ON, 0,                     // 22
};

void delay_ms(uint32_t ms);

void spi_init(void);
void display_init(void);
void spi_write(uint8_t data);

void send_command(uint8_t command);
void send_data(uint8_t data);

void draw_pixel(uint16_t x, uint16_t y, uint16_t color);

#endif /* SRC_TFTDISPLAY_H_ */
