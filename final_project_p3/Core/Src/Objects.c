/*
 * Objects.c
 *
 *  Created on: May 29, 2024
 *      Author: danny
 */

#include "Objects.h"

void grid_init() {
    for (int i = 0; i < VGA_WIDTH; i++) {
        for (int j = 0; j < VGA_HEIGHT; j++) {
            frame_buffer[i + j * VGA_WIDTH].h_addr = i;
            frame_buffer[i + j * VGA_WIDTH].v_addr = j;
            frame_buffer[i + j * VGA_WIDTH].color = 0x000; // black
        }
    }

    return;
}

void grid_clear() {
    for (int i = 0; i < VGA_WIDTH; i++) {
        for (int j = 0; j < VGA_HEIGHT; j++) {
            frame_buffer[i + j * VGA_WIDTH].color = 0x000; // black
        }
    }

    return;
}

void snake_print(Snake_t* snake) {
    static char buffer[10000];
    uart_clear_screen();
    sprintf(buffer, "Snake length: %d", snake->len);
    uart_println(buffer);
    sprintf(buffer, "Snake direction: %d", snake->dir);
    uart_println(buffer);
    sprintf(buffer, "Snake alive: %d", snake->alive);
    uart_println(buffer);
    sprintf(buffer, "Snake score: %d", snake->score);
    uart_println(buffer);

    BodyPart_t* p_body = snake->body;
    while (p_body->valid) {
        sprintf(buffer, "Body part: x: %d, y: %d", p_body->pos.x, p_body->pos.y);
        uart_println(buffer);
        p_body++;
    }

    uart_println("End of snake body");

    return;
}

void snake_init(Snake_t* snake) {
    snake->len = 3;
    snake->dir = NORTH;
    snake->alive = 1;
    snake->score = 0;
    
    for (int i = 0; i < snake->len; i++) {
        snake->body[i].valid = 1;
        snake->body[i].pos.x = START_X;
        snake->body[i].pos.y = START_Y + i;
    }

    for (int i = snake->len; i < MAX_SNAKE_LEN; i++) {
        snake->body[i].valid = 0;
    }
    return;
}


/**
 * @return  -1 if snake ded
 *           0 if snake is alive
*/
int8_t snake_move(Snake_t* snake) {
    if (!snake->alive) {
        return -1;
    }

    snake_change_dir(snake);

    BodyPart_t* p_head = &snake->body[0];
    // BodyPart_t* p_tail = &snake->body[snake->len - 1];

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
    if (snake_check_collision(p_head->pos, snake->body) == -1) {
        snake_die(snake);
        return -1;
    }

    // check if snake collides with boundary
    if (!WITHIN_BOUNDARY(p_head->pos.x, p_head->pos.y)) {
        snake_die(snake);
        return -1;
    }

    // ! do outside
    // check if snake eats food


    return 0;
}


/**
 * @return  0 if snake is alive,
 *          non-zero if snake is ded  
*/
int8_t snake_check_collision(Point_t snake_head, BodyPart_t* body_parts) {
    for (int i = 1; i < MAX_SNAKE_LEN; i++) {
        if (body_parts[i].valid) {
            if (snake_head.x == body_parts[i].pos.x && snake_head.y == body_parts[i].pos.y) {
                return -1;
            }
        }
        else {
            break;
        }
    }

    return 0;
}


void snake_die(Snake_t* snake) {
    snake->alive = 0;
    uart_println("Snake ded");
 
    return;
}

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


