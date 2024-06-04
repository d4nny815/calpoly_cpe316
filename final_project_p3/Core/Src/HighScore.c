/*
 * HighScore.c
 *
 *  Created on: Jun 2, 2024
 *      Author: danny
 */

#include "HighScore.h"

static uint8_t lowest_score;
static uint8_t highest_score;
HighScore_t high_scores[HIGH_SCORES_NUM]; // Define the high_scores array


/**
 * @brief Initialize the eeprom, load the high scores from the eeprom
*/
void highscore_init() {
    eeprom_init();

     //reset the scores on eeprom
//    HighScore_t tmp;
//     for (int i = 0; i < HIGH_SCORES_NUM; i++) {
//         snprintf(tmp.name, MAX_NAME_LEN, "Player %d :)", i + 1);
//         tmp.score = 0;
//         store_highscore(tmp, i);
//     }
//
    uint16_t addr;
    for (int i = 0; i < HIGH_SCORES_NUM; i++) {
        for (int j = 0; j < MAX_NAME_LEN; j++) {
            addr = (i * sizeof(HighScore_t) + j) + HIGH_SCORES_START_ADDR;
            high_scores[i].name[j] = eeprom_load_byte(addr);
        }

        addr =  (i * sizeof(HighScore_t) + (MAX_NAME_LEN * sizeof(char)) + HIGH_SCORES_START_ADDR);
        high_scores[i].score = eeprom_load_byte(addr);
    }

    lowest_score = high_scores[MAX_NAME_LEN - 1].score;
    highest_score = high_scores[0].score;

    return;
}


/**
 * @brief Store the high score to the eeprom
 * @param high_scores the high score to store
 * @param index the index of the high score, not include the base address
*/
void store_highscore(HighScore_t high_scores, uint8_t index) {
    // write the name
    uint16_t addr;
    for (int j = 0; j < MAX_NAME_LEN; j++) {
        addr = (index * sizeof(HighScore_t) + j) + HIGH_SCORES_START_ADDR;
        eeprom_store_byte(high_scores.name[j], addr);
    }

    // write the score
    addr =  (index * sizeof(HighScore_t) + (MAX_NAME_LEN * sizeof(char)) + HIGH_SCORES_START_ADDR);
    eeprom_store_byte(high_scores.score, addr);

    return;
}


/**
 * @brief Load the high score from the eeprom
 * @param index the index of the high score
 * @return the high score
*/
HighScore_t load_highscore(uint8_t index) {
    return high_scores[index];
}


/**
 * @brief Check if the score is a high score
 * @param score the score to check
 * @return 1 if the score is a high score, 0 otherwise
*/
uint8_t is_a_highscore(uint8_t score) {
    return score > lowest_score;
}


/**
 * @brief Update the high scores with the new high score
 * @note  Needs to be called after is_a_highscore 
 * @param high_score the new high score
 
*/
void update_highscores(HighScore_t high_score) {
    uint8_t i;
    // find the index to insert the new high score
    for (i = 0; i < HIGH_SCORES_NUM; i++) {
        if (high_score.score > high_scores[i].score) {
            break;
        }
    }

    // shift all the high scores down
    for (uint8_t j = HIGH_SCORES_NUM - 1; j > i; j--) {
        high_scores[j] = high_scores[j - 1];
    }

    high_scores[i] = high_score;
    lowest_score = high_scores[MAX_NAME_LEN - 1].score;

    // rewrite the newest high scores to eeprom
    while (i < HIGH_SCORES_NUM) {
        store_highscore(high_scores[i], i);
        i++;
    }

    return;
}


/**
 * @brief Get the lowest score
 * @return the lowest score
*/
uint8_t get_lowest_score() {
    return lowest_score;
}


/**
 * @brief Get the highest score
 * @return the highest score
*/
uint8_t get_highest_score() {
    return highest_score;
}
