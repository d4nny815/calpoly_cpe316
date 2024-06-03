/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2024 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "uart.h"
#include "Objects.h"
#include "RNG.h"
#include "Screen.h"
#include "Joystick.h"
#include "HighScore.h"
#include <stdio.h>

#define START_FPS (10)
#define SPEED_UP_FACTOR (1.15f)

typedef enum {
    START,
    PLAYING,
    GAME_OVER
} GameState_t;



int continue_on();
int8_t get_player_name(char* name);

void change_fps(uint32_t fps);
volatile int start_frame = 0;

void SystemClock_Config(void);

int main(void) {
    HAL_Init();
    SystemClock_Config();

    rng_init();
    uart_init();
    joystick_init();
    highscore_init();


    // reset the score
//    HighScore_t tmp;
//     for (int i = 0; i < HIGH_SCORES_NUM; i++) {
//         snprintf(tmp.name, MAX_NAME_LEN, "Player %d :)", i + 1);
//         tmp.score = 0;
//         store_highscore(tmp, i);
//     }

    Snake_t snake;
    Food_t food;
    GameState_t state = START;
    print_start_screen();
    uint32_t fps;
    HighScore_t highscore;

    int first_move = 0;
    while (1) {
        switch (state) {
        case START:
            if (continue_on()) {
                state = PLAYING;
                grid_init(load_highscore(0).score);
                snake_init(&snake);
                food = food_init();
                fps = START_FPS;
                change_fps(fps);

            }
            break;
        case PLAYING:
            if (!first_move) {
                if (snake_change_dir(&snake)) {
                    first_move = 1;
                    break;
                }
                continue;
            }

            if (!start_frame) continue;
            start_frame = 0;

            snake_move(&snake);
            if (snake_check_food(snake, food)) {
                snake_eat(&snake, &food);
                fps *= SPEED_UP_FACTOR;
                change_fps(fps);
            }
            grid_draw(snake, food);

            if (!snake.alive) {
                if (is_a_highscore(snake_get_score(snake))) {
                    if (get_player_name(highscore.name)) {
                        highscore.score = snake_get_score(snake);
                        update_highscores(highscore);
                    }
                }
                state = GAME_OVER;
                print_game_over(snake_get_score(snake));
            }
            break;
        case GAME_OVER:
            if (continue_on()) {
            	state = PLAYING;
            	grid_init(load_highscore(0).score);
            	snake_init(&snake);
            	food = food_init();
            	first_move = 0;
                fps = START_FPS;
                change_fps(fps);
            }
            break;
        }
    }

    return 0;
}

int8_t get_player_name(char* name) {
    char c;
    int i = 0;
    uart_clear_screen();
    uart_println("Enter your name: ");


    while (1) {
        if (uart_check_flag()) {
            uart_clear_flag();
            c = get_uart_char();

            if (i == 0 && c == '\r') {
                return 0;
            }
            else if (c == '\r' || i == MAX_NAME_LEN - 1) {
                uart_println(" ");
                uart_println("Give it a sec, memory being is  VERY slow");
                name[i] = '\0';
                return 1;
            }
            else if (c == 127) {
                if (i > 0) {
                    i--;
                    uart_send_escape("[1D");
                    uart_send_escape("[K");
                }
            }
            else {
                name[i] = c;
                uart_send_char(c);
                i++;
            }
        }
    }
}


void TIM2_IRQHandler() {
    start_frame = 1;
    TIM2->SR &= ~TIM_SR_UIF;

    return;
}

int continue_on() {
    return get_joystick_button();
}


void change_fps(uint32_t fps) {
    RCC->APB1ENR1 |= RCC_APB1ENR1_TIM2EN;
    
    NVIC_DisableIRQ(TIM2_IRQn);
    TIM2->CR1 &= ~TIM_CR1_CEN;
    TIM2->CNT = 0;
    TIM2->SR &= ~TIM_SR_UIF;  
    
    
    TIM2->ARR = CPU_FREQ / fps;
    TIM2->PSC = 0;
    TIM2->DIER |= TIM_DIER_UIE;

//    DBGMCU->APB1FZR1 |= 1;

    NVIC_EnableIRQ(TIM2_IRQn);
    __enable_irq();
    TIM2->CR1 |= TIM_CR1_CEN;
    return;


}




/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

  /** Configure the main internal regulator output voltage
  */
  if (HAL_PWREx_ControlVoltageScaling(PWR_REGULATOR_VOLTAGE_SCALE1) != HAL_OK)
  {
    Error_Handler();
  }

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_MSI;
  RCC_OscInitStruct.MSIState = RCC_MSI_ON;
  RCC_OscInitStruct.MSICalibrationValue = 0;
  RCC_OscInitStruct.MSIClockRange = RCC_MSIRANGE_6;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_MSI;
  RCC_OscInitStruct.PLL.PLLM = 1;
  RCC_OscInitStruct.PLL.PLLN = 16;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV7;
  RCC_OscInitStruct.PLL.PLLQ = RCC_PLLQ_DIV2;
  RCC_OscInitStruct.PLL.PLLR = RCC_PLLR_DIV2;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_1) != HAL_OK)
  {
    Error_Handler();
  }
}

/**
  * @brief RNG Initialization Function
  * @param None
  * @retval None
  */


/* USER CODE BEGIN 4 */

/* USER CODE END 4 */

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */
  __disable_irq();
  while (1)
  {
  }
  /* USER CODE END Error_Handler_Debug */
}

#ifdef  USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t *file, uint32_t line)
{
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */
