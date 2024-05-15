/*
 * danny_std.h
 *
 *  Created on: May 14, 2024
 *      Author: danny
 */

#ifndef SRC_DANNY_STD_H_
#define SRC_DANNY_STD_H_

#include "stm32l476xx.h"

void mv_to_v_string(char* str, uint16_t mv);
void uart_print_statement(char* str, uint16_t val);


#endif /* SRC_DANNY_STD_H_ */
