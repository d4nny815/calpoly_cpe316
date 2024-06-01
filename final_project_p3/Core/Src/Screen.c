/*
 * Screen.c
 *
 *  Created on: May 31, 2024
 *      Author: D4nny
 */

#include "Screen.h"

void print_start_screen() {
    uart_clear_screen();
    uart_send_escape("[0m"); // reset color
    for (int i = 0; i < START_SCREEN_LEN; i++) {
        uart_println(START_SCREEN[i]);
    }

    return;
}

void print_game_over() {
    uart_clear_screen();
    uart_send_escape("[0m"); // reset color
    for (int i = 0; i < GAME_OVER_LEN; i++) {
        uart_println(GAME_OVER_SCREEN[i]);
    }
    return;
}

