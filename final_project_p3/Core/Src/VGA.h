/*
 * VGA.h
 *
 *  Created on: May 29, 2024
 *      Author: danny
 */

#ifndef SRC_VGA_H_
#define SRC_VGA_H_

#include "main.h"

// #define VGA_WIDTH (256)
// #define VGA_HEIGHT (192)
#define VGA_WIDTH (100)
#define VGA_HEIGHT (100)
#define FRAME_BUFFER_SIZE (VGA_WIDTH * VGA_HEIGHT)

typedef struct {
    uint8_t h_addr;
    uint8_t v_addr;
    uint16_t color; // x[15:12], R[11:8], G[7:4], B[3:0]
} PixelData_t;

// static PixelData_t frame_buffer[FRAME_BUFFER_SIZE];

void spi_pin_init();
void vga_init();



#endif /* SRC_VGA_H_ */
