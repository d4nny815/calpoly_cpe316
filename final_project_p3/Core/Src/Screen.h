/*
 * Screen.h
 *
 *  Created on: May 31, 2024
 *      Author: D4nny
 */

#ifndef SRC_SCREEN_H_
#define SRC_SCREEN_H_

#include "main.h"
#include "uart.h"
#include "HighScore.h"
#include <stdio.h>

#define START_SCREEN_LEN (10)
extern const char* START_SCREEN[START_SCREEN_LEN];

#define GAME_OVER_LEN (10)
extern const char* GAME_OVER_SCREEN[GAME_OVER_LEN];
		


void print_start_screen();
void print_game_over(uint8_t score);
void print_high_scores();

#endif /* SRC_SCREEN_H_ */
