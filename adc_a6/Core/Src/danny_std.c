/*
 * danny_std.c
 *
 *  Created on: May 14, 2024
 *      Author: danny
 */
#include "danny_std.h"

void mv_to_v_string(char* str, uint16_t mv) {
    str[0] = (mv / 1000) + '0';
    str[1] = '.';
    str[2] = ((mv % 1000) / 100) + '0';
    str[3] = ((mv % 100) / 10) + '0';
    str[4] = 'V';
    str[5] = '\0';

    return;
}

void uart_print_statement(char* str, uint16_t val) {
    int i = 0;
    str[i++] = 'M';
    str[i++] = 'i';
    str[i++] = 'n';
    str[i++] = ' ';
    str[i++] = (val / 1000) + '0';
    str[i++] = '.';
    str[i++] = ((val % 1000) / 100) + '0';
    str[i++] = ((val % 100) / 10) + '0';
    str[i++] = (val % 10) + '0';
    str[i++] = 'V';
    str[i++] = '\0';

    return;
}

