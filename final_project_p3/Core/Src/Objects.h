/*
 * Objects.h
 *
 *  Created on: May 29, 2024
 *      Author: danny
 */

#ifndef SRC_OBJECTS_H_
#define SRC_OBJECTS_H_

#include "main.h"
#include "VGA.h"
#include "uart.h"

#define MAX_SNAKE_LEN (50)

typedef struct {
    uint8_t x;
    uint8_t y;
} Point_t;

typedef struct {
    uint8_t valid;
    Point_t pos;
} BodyPart_t;

typedef struct {
    uint8_t len;
    BodyPart_t body[MAX_SNAKE_LEN];
    uint8_t dir;
    // uint8_t speed;
    uint8_t alive;
    uint8_t score;
} Snake_t;

typedef Point_t Food_t;

void grid_init();
void grid_draw();
void grid_clear();

void snake_init(Snake_t *snake);
void snake_move(Snake_t *snake);
void snake_change_dir(Snake_t *snake, uint8_t dir);
void snake_eat(Snake_t *snake, Food_t *food);
void snake_die(Snake_t *snake);
void snake_draw(Snake_t *snake);
// void snake_respawn(Snake_t *snake);
void snake_print(Snake_t *snake);

// void food_init(Food_t *food);
void food_respawn(Food_t *food, Snake_t *snake);
void food_draw(Food_t *food);

#endif /* SRC_OBJECTS_H_ */
