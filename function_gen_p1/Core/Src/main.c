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

#include "main.h"
#include "FuncGen.h"
#include "DAC.h"


void SystemClock_Config(void);

typedef enum {
    SQUARE,
    SAWTOOTH,
    SINE,
    TRIANGLE
} WaveShape_t;
WaveShape_t state = SQUARE;

void change_freq(int8_t key);
void change_duty_cycle(int8_t key);
void change_shape(int8_t key);

uint32_t wave_lut_ind;
uint16_t freq = DEFAULT_FREQ;
uint32_t next_lut_offset = DEFAULT_FREQ / FREQ_MIN;
uint8_t duty_cycle = DEFAULT_DC;

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void) {
    HAL_Init();
    SystemClock_Config();

    DAC_init();
    keypad_init();
    square_wave_init(freq, duty_cycle);
    
    int8_t key = KEYPAD_NO_PRESS;
    while (1) {
        key = keypad_read_oneshot();
        if (isFreqChangePin(key)) {
            change_freq(key);
            wave_lut_ind = 0;
            if (state == SQUARE) square_wave_init(freq, duty_cycle);
        }
        else if (isDutyChangePin(key) && state == SQUARE) {
            change_duty_cycle(key);
            square_wave_init(freq, duty_cycle);
        }
        else if (isWaveChangePin(key)) {
            change_shape(key);
            wave_lut_ind = 0;
        }
    }

    return 0;
}

void change_freq(int8_t key) {
    freq = key * FREQ_MIN;
    next_lut_offset = key;

    return;
}

void change_duty_cycle(int8_t key) {
    switch (key) {
    case 0:
        duty_cycle = 50;
        return;
    case KEYPAD_STAR:
        if (duty_cycle > MIN_DC) {
            duty_cycle -= 10;
        }
        return;
    case KEYPAD_POUND:
        if (duty_cycle < MAX_DC) {
            duty_cycle += 10;
        }
        return;
    }
}

void change_shape(int8_t key) {
    switch (key) {
    case SINE_KEY:
        if (state == SQUARE) continous_mode_init();
        state = SINE;
        return;
    case TRIANGLE_KEY:
        if (state == SQUARE) continous_mode_init();
        state = TRIANGLE;
        return;
    case SAWTOOTH_KEY:
        if (state == SQUARE) continous_mode_init();
        state = SAWTOOTH;
        return;
    case SQUARE_KEY:
        square_wave_init(freq, duty_cycle);
        state = SQUARE;
        return;
    }
}


void TIM2_IRQHandler(void) {
    switch (state) {
    case SQUARE:
        if (TIM2->SR & CCR1_BIT) {
            DAC_write(volt_to_dac_val(MIN_FUNC_VOLTAGE));
        } else {
            DAC_write(volt_to_dac_val(MAX_FUNC_VOLTAGE));
        }
        break;
    case SAWTOOTH:
        wave_lut_ind = (wave_lut_ind + next_lut_offset) % WAVE_LUT_SIZE;
        DAC_write(SAWTOOTH_LUT[wave_lut_ind]);
        break;
    case SINE:
        wave_lut_ind = (wave_lut_ind + next_lut_offset) % WAVE_LUT_SIZE;
        DAC_write(SINE_LUT[wave_lut_ind]);
        break;
    case TRIANGLE:
        wave_lut_ind = (wave_lut_ind + next_lut_offset) % WAVE_LUT_SIZE;
        DAC_write(TRIANGLE_LUT[wave_lut_ind]);
        break;
    default: break;
    }
    TIM2->SR &= ~(TIM_INTR_FLAGS);
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
  RCC_OscInitStruct.PLL.PLLN = 20;
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

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_2) != HAL_OK)
  {
    Error_Handler();
  }
}

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
