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

#define START_X (10)
#define START_Y (20)

#define MAX_SNAKE_LEN (50)

// #define CHECK_BOUNDARY(x, y) ((x) >= 0 && (x) < VGA_WIDTH && (y) >= 0 && (y) < VGA_HEIGHT)
#define CHECK_VERTICAL(x) ((x) >= 0 && (x) < VGA_HEIGHT)
#define CHECK_HORIZONTAL(y) ((y) >= 0 && (y) < VGA_WIDTH)
#define WITHIN_BOUNDARY(x, y) (CHECK_HORIZONTAL(x) && CHECK_VERTICAL(y))

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
    Direction_t dir;
    // uint8_t speed;
    uint8_t alive;
    uint8_t score;
} Snake_t;

typedef Point_t Food_t;

void grid_init();
void grid_draw(); // ? leave this to dma ?
void grid_clear();

void snake_init(Snake_t* snake); // done
int8_t snake_move(Snake_t* snake); 
void snake_change_dir(Snake_t* snake);
void snake_eat(Snake_t* snake, Food_t* food);
void snake_grow(Snake_t* snake);
void snake_die(Snake_t* snake);
void snake_draw(Snake_t* snake);
void snake_check_food(Snake_t* snake, Food_t* food);
int8_t snake_check_collision(Point_t snake_head, BodyPart_t* body_parts); // done
// void snake_respawn(Snake_t* snake);
void snake_print(Snake_t* snake); // done

// void food_init(Food_t* food);
void food_respawn(Food_t* food, Snake_t* snake);
void food_draw(Food_t* food);

#endif /* SRC_OBJECTS_H_ */
