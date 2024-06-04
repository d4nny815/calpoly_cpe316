/*
 * Screen.c
 *
 *  Created on: May 31, 2024
 *      Author: D4nny
 */

#include "Screen.h"

const char* START_SCREEN[START_SCREEN_LEN] = {
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


const char* GAME_OVER_SCREEN[GAME_OVER_LEN] = {
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


/**
 * @brief Print the start screen
*/
void print_start_screen() {
    uart_clear_screen();
    uart_send_escape("[0m"); // reset color
    for (int i = 0; i < START_SCREEN_LEN; i++) {
        uart_println(START_SCREEN[i]);
    }

    return;
}


/**
 * @brief Print the game over screen
*/
void print_game_over(uint8_t score) {
    char buf[100];
    uart_clear_screen();
    uart_send_escape("[0m"); // reset color
    for (int i = 0; i < GAME_OVER_LEN; i++) {
        uart_println(GAME_OVER_SCREEN[i]);
    }
    sprintf(buf, "Your Score: %hu", score);
    uart_println(buf);

    print_high_scores();
    
    return;
}


/**
 * @brief Print the high score table
*/
void print_high_scores() {
    HighScore_t tmp;
    char uart_buf[100];

    uart_println(" ====================================");
    uart_println("| Name                 | Score       |");
    uart_println(" ====================================");

    for (int i = 0; i < HIGH_SCORES_NUM; i++) {
        // tmp = load_highscore(i);
        tmp = high_scores[i];
        sprintf(uart_buf, "| %-20s | %-11u |", tmp.name, tmp.score);
        uart_println(uart_buf);
        uart_println(" ====================================");
    }

    return;
}

