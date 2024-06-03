/*
 * HighScore.h
 *
 *  Created on: Jun 2, 2024
 *      Author: danny
 */

#ifndef SRC_HIGHSCORE_H_
#define SRC_HIGHSCORE_H_

#include "main.h"
#include "eeprom.h"

#define HIGH_SCORES_START_ADDR (0x69)
#define HIGH_SCORES_NUM (10)
#define MAX_NAME_LEN (20)

typedef struct {
    char name[MAX_NAME_LEN];
    uint8_t score;
} HighScore_t;

static HighScore_t high_scores[HIGH_SCORES_NUM];

void highscore_init();
uint8_t is_a_highscore(uint8_t score);
void update_highscores(HighScore_t high_score);
void store_highscore(HighScore_t highscore, uint8_t index);
HighScore_t load_highscore(uint8_t index);

uint8_t get_lowest_score();
uint8_t get_highest_score();


#endif /* SRC_HIGHSCORE_H_ */
