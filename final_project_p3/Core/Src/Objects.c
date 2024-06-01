/*
 * Objects.c
 *
 *  Created on: May 29, 2024
 *      Author: danny
 */

#include "Objects.h"

uint8_t same_point(Point_t a, Point_t b) {
    return (a.x == b.x && a.y == b.y);
}


/**
 * @brief Initialize the grid with black color
*/
void grid_init() {
    // TODO: SPI display
    // TODO: DMA
    // for (int i = 0; i < VGA_WIDTH; i++) {
        // for (int j = 0; j < VGA_HEIGHT; j++) {
            // frame_buffer[i + j * VGA_WIDTH].h_addr = i;
            // frame_buffer[i + j * VGA_WIDTH].v_addr = j;
            // frame_buffer[i + j * VGA_WIDTH].color = 0x000; // black
        // }
    // }


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
    food_draw(food);

    return;
}


/**
 * @brief Draw the snake object
 * @param snake: the snake object
*/
void snake_draw(Snake_t snake) {
    // erase tail
    sprintf(snake_print_buffer, "[%u;%uH", snake.tail->pos.y, snake.tail->pos.x);
    uart_send_escape(snake_print_buffer);
    uart_send_char(' ');

    // draw head
    sprintf(snake_print_buffer, "[%u;%uH", snake.body[0].pos.y, snake.body[0].pos.x);
    uart_send_escape(snake_print_buffer);
    uart_send_char('O');

    
    return;
}


/**
 * @brief Initialize the snake object
 * @return the snake object
*/
void snake_init(Snake_t* snake) {
    snake->len = 3;
    snake->dir = NORTH;
    snake->alive = 1;
    snake->score = 0;
    
    int i;
    for (i = 0; i < snake->len; i++) {
        snake->body[i].valid = 1;
        snake->body[i].pos.x = START_X;
        snake->body[i].pos.y = START_Y + i;
    }

    snake->tail = &(snake->body[i - 1]);

    for (i = snake->len; i < MAX_SNAKE_LEN; i++) {
        snake->body[i].valid = 0;
    }
    
    // draw initial snake
    BodyPart_t body;
    for (int i = 0; i < snake->len - 1; i++) {
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
    if (snake_out_of_bounds(*snake)) {
        snake_die(snake);
        return -1;
    }

    BodyPart_t* p_head = &snake->body[0];

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

    // TODO: check if fully working
    // check if snake collides with itself
    if (snake_hit_itself(p_head->pos, snake->body) == -1) {
        snake_die(snake);
        return -1;
    }

    return 0;
}


/**
 * @brief Check if snake is within the boundary
 * @return  non-zero if out of boundary
 *          0 if in
*/
int8_t snake_out_of_bounds(Snake_t snake) {
    Point_t snake_head = snake.body[0].pos;
    return ((snake_head.x == 0 && snake.dir == WEST ) ||
            (snake_head.x == VGA_WIDTH - 1 && snake.dir == EAST) ||
            (snake_head.y == 0 && snake.dir == NORTH) ||
            (snake_head.y == VGA_HEIGHT - 1 && snake.dir == SOUTH));
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
*/
void snake_change_dir(Snake_t* snake) {
    if (!uart_check_flag()) return;

    uart_clear_flag();
    Direction_t new_dir;
    char c = get_uart_char();
    
    switch (c) {
    case 'w':
        new_dir = NORTH;
        break;
    case 'a':
        new_dir = WEST;
        break;
    case 's':
        new_dir = SOUTH;
        break;
    case 'd':
        new_dir = EAST;
        break;
    default: return;    
    }

    // change dir as long as not 180, if same dir then nothing happens
    switch (snake->dir) {
    case NORTH:
        if (new_dir != SOUTH) snake->dir = new_dir;
        return;
    case EAST:
        if (new_dir != WEST) snake->dir = new_dir;
        return;
    case SOUTH:
        if (new_dir != NORTH) snake->dir = new_dir;
        return;
    case WEST:
        if (new_dir != EAST) snake->dir = new_dir;
        return;    
    }
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
    // BodyPart_t* p_tail;
    // for (int i = 0; i < MAX_SNAKE_LEN; i++) {
        // if (!snake->body[i].valid) {
            // p_tail = &snake->body[i];
            // break;
        // }
    // }

    
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
    // uart_println("Eating food");
    snake_grow(snake);
    food_respawn(food);

    return;
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
    food_draw(food);

    return food;
}


/**
 * @brief Respawn the food object
 * @param food: the food object
*/
void food_respawn(Food_t* food) {
    food->x = get_random(VGA_WIDTH);
    food->y = get_random(VGA_HEIGHT);

    return;
}


/**
 * @brief Draw the food object
*/
void food_draw(Food_t food) {
    sprintf(snake_print_buffer, "[%u;%uH", food.y, food.x);
    uart_send_escape(snake_print_buffer);
    uart_send_char('X');

    // sprintf(snake_print_buffer, "Food Pos: (%d, %d)", food.x, food.y);
    // uart_println(snake_print_buffer);
    return;
}


