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
#include "stdio.h"
#include "RNG.h"
#include "Joystick.h"
#include "math.h"

#define SNAKE_PRINT_LEN (100)
static char snake_print_buffer[SNAKE_PRINT_LEN] = {'\0'};

#define START_BOUNDARY_OFFSET (10)
#define START_LEN (5)
#define MAX_SNAKE_LEN (50)


#define LEFT_BOUND (8)
#define RIGHT_BOUND (100)
#define TOP_BOUND (3)
#define BOTTOM_BOUND (50)
#define TEXT_VERTICAL_CURSOR (65)
#define SCORE_CURSOR_X (LEFT_BOUND + 7)
#define SCORE_CURSOR_Y (BOTTOM_BOUND + 5)


#define CHECK_HORIZONTAL(x) ((x) > LEFT_BOUND && (x) < RIGHT_BOUND)
#define CHECK_VERTICAL(y) ((y) > TOP_BOUND && (y) < BOTTOM_BOUND)
#define WITHIN_BOUND(x, y) (CHECK_HORIZONTAL(x) && CHECK_VERTICAL(y))


typedef struct {
    uint8_t x;       
    uint8_t y;
} Point_t;

typedef struct {
    uint8_t valid;
    Point_t pos;
} BodyPart_t;

typedef enum {
    NORTH,
    EAST,
    SOUTH,
    WEST
} Direction_t;

typedef struct {
    uint8_t len;
    BodyPart_t body[MAX_SNAKE_LEN];
    BodyPart_t* tail;
    Direction_t dir;
    uint8_t alive;
    uint8_t score;
} Snake_t;

typedef Point_t Food_t;

uint8_t same_point(Point_t a, Point_t b);

void grid_init(uint8_t high_score);
void grid_draw(Snake_t snake, Food_t food); // ? leave this to dma ?
void grid_clear();


void snake_init(Snake_t* snake); // done
int8_t snake_move(Snake_t* snake); 
uint8_t snake_change_dir(Snake_t* snake); // done
void snake_eat(Snake_t* snake, Food_t* food); // done
void snake_grow(Snake_t* snake); // done
void snake_die(Snake_t* snake); // done
void snake_draw(Snake_t snake); // done
uint8_t snake_check_food(Snake_t snake, Food_t food); // done
int8_t snake_hit_itself(Point_t snake_head, BodyPart_t* body_parts); // done

uint8_t snake_get_score(Snake_t snake);
uint8_t snake_is_alive(Snake_t snake);


Food_t food_init(); // done
void food_respawn(Food_t* food); // done
void food_draw(Food_t food); // done

#endif /* SRC_OBJECTS_H_ */
