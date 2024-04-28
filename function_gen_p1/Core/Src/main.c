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
} state_t;
state_t state = SQUARE;

uint32_t wave_lut_ind;
uint16_t freq = 500;
uint8_t duty_cycle = 50;
uint32_t wave_scalar = 500 / FREQ_MIN;


int main(void) {
    HAL_Init();
    SystemClock_Config();

    RCC->AHB2ENR |= RCC_AHB2ENR_GPIOCEN;

        GPIOC->MODER   &= ~(GPIO_MODER_MODE0);          // clear mode bits for pin 5
        GPIOC->MODER   |= GPIO_MODER_MODE0_0;           // output mode
        GPIOC->OTYPER  &= ~(GPIO_OTYPER_OT0);           // push pull
        GPIOC->OSPEEDR &= ~(GPIO_OSPEEDR_OSPEED0_Msk);  // low speed
        GPIOC->PUPDR   &= ~(GPIO_PUPDR_PUPD0);          // no resistor

        GPIOC->BSRR = GPIO_PIN_0;

    DAC_init();
    keypad_init();
    square_wave_init(freq, duty_cycle);
    
    int8_t key = KEYPAD_NO_PRESS;
    while (1) {
        key = keypad_read_oneshot();
        if (isFreqChangePin(key)) {
            freq = key * FREQ_MIN;
            wave_scalar = freq / FREQ_MIN;
            if (state == SQUARE) square_wave_init(freq, duty_cycle);
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
            switch (key) {
                case SINE_KEY:
                    if (state == SQUARE) continous_mode_init();
                    state = SINE;
                    break;
                case TRIANGLE_KEY:
                    if (state == SQUARE) continous_mode_init();
                    state = TRIANGLE;
                    break;
                case SAWTOOTH_KEY:
                    if (state == SQUARE) continous_mode_init();
                    state = SAWTOOTH;
                    break;
                case SQUARE_KEY:
                    square_wave_init(freq, duty_cycle);
                    state = SQUARE;
                    break;
            }
            wave_scalar = freq / FREQ_MIN;
            wave_lut_ind = 0;
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
    case SAWTOOTH:
        wave_lut_ind = (wave_lut_ind + wave_scalar) % WAVE_LUT_SIZE;
        DAC_write(SAWTOOTH_LUT[wave_lut_ind]);
        break;
    case SINE:
        wave_lut_ind = (wave_lut_ind + wave_scalar) % WAVE_LUT_SIZE;
        DAC_write(SINE_LUT[wave_lut_ind]);
        break;
    case TRIANGLE:
        wave_lut_ind = (wave_lut_ind + wave_scalar) % WAVE_LUT_SIZE;
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
