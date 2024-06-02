/*
 * Objects.c
 *
 *  Created on: May 29, 2024
 *      Author: danny
 */

#include "Objects.h"

static BodyPart_t old_tail;

uint8_t same_point(Point_t a, Point_t b) {
    return (a.x == b.x && a.y == b.y);
}


/**
 * @brief Initialize the grid with black color
*/
void grid_init(uint8_t high_score) {
    uart_clear_screen();
    uart_send_escape("[0m"); // reset color

    // print top and bottom border
    sprintf(snake_print_buffer, "[%u;%uH", TOP_BOUND, LEFT_BOUND);
    uart_send_escape(snake_print_buffer);
    for (int i = LEFT_BOUND; i < RIGHT_BOUND; i++) {
        uart_send_char('=');
    }

    sprintf(snake_print_buffer, "[%u;%uH", BOTTOM_BOUND, LEFT_BOUND);
    uart_send_escape(snake_print_buffer);
    for (int i = LEFT_BOUND; i < RIGHT_BOUND; i++) {
        uart_send_char('=');
    }

    // print left and right border
    for (int i = TOP_BOUND; i < BOTTOM_BOUND + 1; i++) {
        sprintf(snake_print_buffer, "[%u;%uH", i, LEFT_BOUND);
        uart_send_escape(snake_print_buffer);
        uart_send_char('|');

        sprintf(snake_print_buffer, "[%u;%uH", i, RIGHT_BOUND);
        uart_send_escape(snake_print_buffer);
        uart_send_char('|');
    }

    sprintf(snake_print_buffer, "[%u;%uH", BOTTOM_BOUND + 5, RIGHT_BOUND - 15);
    uart_send_escape(snake_print_buffer);
    sprintf(snake_print_buffer, "High Score: %u", high_score);
    uart_send_string(snake_print_buffer);

    sprintf(snake_print_buffer, "[%u;%uH", SCORE_CURSOR_Y, SCORE_CURSOR_X - 7);
    uart_send_escape(snake_print_buffer);
    uart_send_string("Score: ");




    return;
}


/**
 * @brief Clear the grid with black color
*/
void grid_clear() {
    // TODO: SPI display
    // for (int i = 0; i < VGA_WIDTH; i++) {
    //     for (int j = 0; j < VGA_HEIGHT; j++) {
    //         frame_buffer[i + j * VGA_WIDTH].color = 0x000; // black
    //     }
    // }

    return;
}


// TODO: change to SPI display
/**
 * @brief Draw the snake and food on the grid
 * @param snake: the snake object
 * @param food: the food object
*/
void grid_draw(Snake_t snake, Food_t food) {
    // uart_clear_screen();
    snake_draw(snake);

    return;
}


/**
 * @brief Draw the snake object
 * @param snake: the snake object
*/
void snake_draw(Snake_t snake) {
    // erase tail
    // sprintf(snake_print_buffer, "[%u;%uH", snake.tail->pos.y, snake.tail->pos.x);
    sprintf(snake_print_buffer, "[%u;%uH", old_tail.pos.y, old_tail.pos.x);
    uart_send_escape(snake_print_buffer);
    uart_send_char(' ');

    // draw head
    sprintf(snake_print_buffer, "[%u;%uH", snake.body[0].pos.y, snake.body[0].pos.x);
    uart_send_escape(snake_print_buffer);
    uart_send_escape("[32m"); // green
    uart_send_char('O');

    // print current score
    sprintf(snake_print_buffer, "[%u;%uH", SCORE_CURSOR_Y, SCORE_CURSOR_X);
    uart_send_escape(snake_print_buffer);
    sprintf(snake_print_buffer, "%u", snake.score);
    uart_send_string(snake_print_buffer);
    
    return;
}


/**
 * @brief Initialize the snake object
 * @return the snake object
*/
void snake_init(Snake_t* snake) {
    snake->len = START_LEN;
    snake->dir = NORTH;
    snake->alive = 1;
    snake->score = 0;
    
    uint8_t x = get_random(LEFT_BOUND + START_BOUNDARY_OFFSET, RIGHT_BOUND - START_BOUNDARY_OFFSET);
    uint8_t y = get_random(TOP_BOUND + START_BOUNDARY_OFFSET, BOTTOM_BOUND - START_BOUNDARY_OFFSET - snake->len - 1);

    for (int i = 0; i < snake->len; i++) {
        snake->body[i].valid = 1;
        snake->body[i].pos.x = x;
        snake->body[i].pos.y = y + i;
    }

    snake->tail = &(snake->body[snake->len - 1]);
    old_tail = *(snake->tail);

    for (int i = snake->len; i < MAX_SNAKE_LEN; i++) {
        snake->body[i].valid = 0;
    }
    
    // draw initial snake
    uart_send_escape("[32m"); // green
    BodyPart_t body;
    for (int i = 0; i < snake->len; i++) {
        body = snake->body[i];
        sprintf(snake_print_buffer, "[%u;%uH", body.pos.y, body.pos.x);
        uart_send_escape(snake_print_buffer);
        uart_send_char('O');
    }

    return;
}


/**
 * @brief Move the snake object
 * @return  -1 if snake ded
 *           0 if snake is alive
*/
int8_t snake_move(Snake_t* snake) {
    if (!snake->alive) {
        return -1;
    }

    snake_change_dir(snake);

    // check if snake collides with boundary
    if (!WITHIN_BOUND(snake->body[0].pos.x, snake->body[0].pos.y)) {
        snake_die(snake);
        return -1;
    }

    BodyPart_t* p_head = &snake->body[0];

    old_tail = *(snake->tail);

    // move body parts
    for (int i = snake->len - 1; i > 0; i--) {
        snake->body[i].pos.x = snake->body[i - 1].pos.x;
        snake->body[i].pos.y = snake->body[i - 1].pos.y;
    }

    // move head
    switch (snake->dir) {
        case NORTH:
            p_head->pos.y--;
            break;
        case EAST:
            p_head->pos.x++;
            break;
        case SOUTH:
            p_head->pos.y++;
            break;
        case WEST:
            p_head->pos.x--;
            break;
        default:
            break;
    }

    // check if snake collides with itself
    if (snake_hit_itself(p_head->pos, snake->body) == -1) {
        snake_die(snake);
        return -1;
    }

    return 0;
}


/**
 * @brief Check if snake collides with itself
 * @return  0 if snake is alive,
 *          non-zero if snake is ded  
*/
int8_t snake_hit_itself(Point_t snake_head, BodyPart_t* body_parts) {
    for (int i = 1; i < MAX_SNAKE_LEN; i++) {
        if (body_parts[i].valid) {
            if (same_point(snake_head, body_parts[i].pos)) {
                return -1;
            }
        }
        else {
            break;
        }
    }

    return 0;
}


/**
 * @brief kill the snake
 * @param snake: the snake object
*/
void snake_die(Snake_t* snake) {
    snake->alive = 0;
    uart_println("Snake ded");
 
    return;
}


/**
 * @brief Read the UART input and change the snake direction
 * @param snake: the snake object
 * @return 0 if no change
 *          non zero if change
*/
uint8_t snake_change_dir(Snake_t* snake) {
    uint16_t joy_x = get_joystick_x();
    uint16_t joy_y = get_joystick_y();

    Direction_t new_dir;
    uint8_t x_axis;

    x_axis = abs(joy_x - 2048) > abs(joy_y - 2048); 

    if (x_axis) {
        if (joy_x > RIGHT_QUARTER) {
            new_dir = EAST;
        }
        else if (joy_x < LEFT_QUARTER) {
            new_dir = WEST;
        }
        else {
            return 0;
        }
    }
    else {
        if (joy_y > TOP_QUARTER) {
            new_dir = NORTH;
        }
        else if (joy_y < BOTTOM_QUARTER) {
            new_dir = SOUTH;
        } else {
            return 0;
        }
    }

    // change dir as long as not 180, if same dir then nothing happens
    switch (snake->dir) {
    case NORTH:
        if (new_dir != SOUTH) snake->dir = new_dir;
        break;
    case EAST:
        if (new_dir != WEST) snake->dir = new_dir;
        break;
    case SOUTH:
        if (new_dir != NORTH) snake->dir = new_dir;
        break;
    case WEST:
        if (new_dir != EAST) snake->dir = new_dir;
        break;
    }

    return 1;
}


/**
 * @brief check snake head collides with food
 * @return  0 if no
 *          non-zero if yes
*/
uint8_t snake_check_food(Snake_t snake, Food_t food) {
    return same_point((snake.body[0]).pos, food);
}


/**
 * @brief Grow the snake
 * @param snake: the snake object
*/
void snake_grow(Snake_t* snake) {
    snake->body[snake->len].valid = 1;
    snake->body[snake->len].pos = snake->body[snake->len - 1].pos;
    snake->tail = &snake->body[snake->len];
    
    snake->len++;
    snake->score++;

    return;
}


/**
 * @brief Eat the food
 * @param snake: the snake object
 * @param food: the food object
*/
void snake_eat(Snake_t* snake, Food_t* food) {
    snake_grow(snake);
    food_respawn(food);

    return;
}


/**
 * @brief get snake score
 * @param snake: snake object
 * @return snake score
*/
uint8_t snake_get_score(Snake_t snake) {
    return snake.score;
}

// * --------------------------------------------------------
// * FOOD
// * --------------------------------------------------------

/**
 * @brief Initialize the food object
 * @return the food object
*/
Food_t food_init() {
    Food_t food;
    food_respawn(&food);

    return food;
}


/**
 * @brief Respawn the food object
 * @param food: the food object
*/
void food_respawn(Food_t* food) {
    food->x = get_random(LEFT_BOUND + 1, RIGHT_BOUND - 1);
    food->y = get_random(TOP_BOUND + 1, BOTTOM_BOUND - 1);

    food_draw(*food);
    return;
}


/**
 * @brief Draw the food object
*/
void food_draw(Food_t food) {
    sprintf(snake_print_buffer, "[%u;%uH", food.y, food.x);
    uart_send_escape(snake_print_buffer);
    uart_send_escape("[5m"); // blinking
    uart_send_escape("[31m"); // red
    uart_send_char('X');
    uart_send_escape("[0m"); // back to default


    return;
}


