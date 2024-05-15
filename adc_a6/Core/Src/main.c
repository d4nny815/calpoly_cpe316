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
#include "ADC.h"
#include "uart.h"
#include "danny_std.h"
#include "stdio.h"
#include "string.h"

uint16_t get_min_arr(uint16_t *arr, int size);
uint16_t get_max_arr(uint16_t *arr, int size);
uint16_t get_avg_arr(uint16_t *arr, int size);
void print_screen(int ind, uint16_t* arr, int arr_size);

#define ADC_ARRAY_SIZE 20
volatile int adc_flag = 0;
volatile uint16_t adc_value;


void SystemClock_Config(void);
void delay(uint32_t time);

int main(void) {
    HAL_Init();
    SystemClock_Config();

    uart_init();
    ADC_init();


    uint16_t adc_values[ADC_ARRAY_SIZE] = {0};
    int adc_values_index = 0;

    uint16_t min, max, avg;
    char uart_string_buffer[6];

    uart_send_string("starting");
    ADC_start_conversion();

    while (1) {
        if (adc_flag) { // gets set in ISR
            adc_values[adc_values_index] = adc_value;
            adc_values_index++;

            if (adc_values_index == ADC_ARRAY_SIZE) {
                adc_values_index = 0;

                min = get_min_arr(adc_values, ADC_ARRAY_SIZE);
                max = get_max_arr(adc_values, ADC_ARRAY_SIZE);
                avg = get_avg_arr(adc_values, ADC_ARRAY_SIZE);

                min = ADC_to_mv(min);
                max = ADC_to_mv(max);
                avg = ADC_to_mv(avg);

                // sprintf(uart_string_budelay(10000);ffer, "Min: %hu, Max: %hu, Avg: %hu\n", min, max, avg); // just here for debug rn
                // uart_send_string(uart_string_buffer);
//                uart_clear_screen();
                uart_send_escape("[H");
                uart_send_string("Min: ");
                mv_to_v_string(uart_string_buffer, min);
                uart_send_string(uart_string_buffer);
                uart_send_string(" Max: ");
                mv_to_v_string(uart_string_buffer, max);
                uart_send_string(uart_string_buffer);
                uart_send_string(" Avg: ");
                mv_to_v_string(uart_string_buffer, avg);
                uart_send_string(uart_string_buffer);

                delay(50000);
            }

            adc_flag = 0;
            ADC_start_conversion();
        }

    }



    return 0;
}

void ADC1_2_IRQHandler() {
    adc_flag = 1;
    adc_value = ADC1->DR;
    // if (ADC1->ISR & ADC_ISR_EOC) {
        // adc_flag = 1;
        // adc_value = ADC1->DR;
    // }

    return;
}

uint16_t get_min_arr(uint16_t *arr, int size) {
    uint16_t min = arr[0];
    for (int i = 1; i < size; i++) {
        if (arr[i] < min) {
            min = arr[i];
        }
    }
    return min;
}

uint16_t get_max_arr(uint16_t *arr, int size) {
    uint16_t max = arr[0];
    for (int i = 1; i < size; i++) {
        if (arr[i] > max) {
            max = arr[i];
        }
    }
    return max;
}

uint16_t get_avg_arr(uint16_t *arr, int size) {
    uint32_t sum = 0;
    for (int i = 0; i < size; i++) {
        sum += arr[i];
    }
    return sum / size;
}


void delay(uint32_t time) {
    for (uint32_t i = 0; i < time; i++);
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
