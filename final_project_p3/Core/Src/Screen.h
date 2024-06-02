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

#define START_SCREEN_LEN (10)
static const char* START_SCREEN[START_SCREEN_LEN] = {
		" ____ _____  _    ____ _____ ",
		"/ ___|_   _|/ \\  |  _ \\_   _|",
		"\\___ \\ | | / _ \\ | |_) || |  ",
		" ___) || |/ ___ \\|  _ < | |  ",
		"|____/ |_/_/   \\_\\_| \\_\\|_|  "
		" ",
		" ",
		" ",
		" ",
		"Press joystick to start the game",
	};

#define GAME_OVER_LEN (10)
static const char* GAME_OVER_SCREEN[GAME_OVER_LEN] = {
	"  ____    _    __  __ _____    _____     _______ ____  ",
	" / ___|  / \\  |  \\/  | ____|  / _ \\ \\   / / ____|  _ \\ ",
	"| |  _  / _ \\ | |\\/| |  _|   | | | \\ \\ / /|  _| | |_) |",
	"| |_| |/ ___ \\| |  | | |___  | |_| |\\ V / | |___|  _ < ",
	" \\____/_/   \\_\\_|  |_|_____|  \\___/  \\_/  |_____|_| \\_\\",
	" ",
	" ",
	" ",
	" ",
	"Press joystick to restart the game",
};
		


void print_start_screen();
void print_game_over(uint8_t score, uint8_t high_score);

#endif /* SRC_SCREEN_H_ */
