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

void print_game_over(uint8_t score) {
    char buf[100];
    uart_clear_screen();
    uart_send_escape("[0m"); // reset color
    for (int i = 0; i < GAME_OVER_LEN; i++) {
        uart_println(GAME_OVER_SCREEN[i]);
    }
    sprintf(buf, "Your Score: %u", score);
    uart_println(buf);

    print_high_scores();
    return;
}


void print_high_scores() {
    HighScore_t tmp;
    char uart_buf[100];

    uart_println(" ====================================");
    uart_println("| Name          | Score              |");
    uart_println(" ====================================");

    for (int i = 0; i < HIGH_SCORES_NUM; i++) {
        tmp = load_highscore(i);
        sprintf(uart_buf, "| %-13s | %-18u |", tmp.name, tmp.score);
        uart_println(uart_buf);
        uart_println(" ====================================");
    }


    return;
}

