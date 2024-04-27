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
#include "FuncGen.h"
#include "DAC.h"

void SystemClock_Config(void);

int first_time_in_state = 1;
typedef enum {
    SQUARE,
    SAWTOOTH,
    SINE,
    TRIANGLE
} state_t;
state_t state = SQUARE;

int main(void) {
    HAL_Init();
    SystemClock_Config();

    DAC_init();
    keypad_init();
    
    int8_t key;
    uint16_t freq = 500;
    uint8_t duty_cycle = 50;
    
    square_wave_init(freq, duty_cycle);
    while (1) {
        key = keypad_read_oneshot();
        if (isFreqChangePin(key)) {
            freq = key * FREQ_MIN;
            square_wave_init(freq, duty_cycle);
        }
        else if (isDutyChangePin(key) && state == SQUARE) {
            switch (key) {
                case 0:
                    duty_cycle = 50;
                    break;
                case KEYPAD_STAR:
                    duty_cycle = 10;
                    break;
                case KEYPAD_POUND:
                    duty_cycle = 90;
                    break;
            }
            square_wave_init(freq, duty_cycle);
        }
        else if (isWaveChangePin(key)) {
            
        }
    }
    return 0;
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
        default: break;
    }    

    TIM2->SR &= ~(CCR1_BIT | ARR_BIT);
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
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_NONE;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_MSI;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_0) != HAL_OK)
  {
    Error_Handler();
  }
}


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