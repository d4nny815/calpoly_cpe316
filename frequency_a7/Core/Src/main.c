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
/* Includes ------------------------------------------------------------------*/
#define ARM_MATH_CM4

#include "main.h"
#include "arm_math.h"
#include "timer.h"
#include "ADC.h"
#include "uart.h"
#include "stdio.h"

#define SAMPLING_PERIOD 250e-6
#define SAMPLING_RATE (uint32_t)(1/SAMPLING_PERIOD)
#define FFT_SIZE 2048
//#define FFT_SIZE 4096
#define FFT_SIZE_DIV_2 (FFT_SIZE/2)
#define FFT_SIZE_DIV_4 (FFT_SIZE/4)
#define FFT_SIZE_MASK (FFT_SIZE - 1)
#define FFT_HALF_POINT (FFT_SIZE_DIV_2 - 1)
#define FFT_FULL_POINT (0)
#define FREQ_STR_LEN 20


q15_t adc_vals[FFT_SIZE];
q15_t* p_ADC_VALS_half1 = adc_vals;
q15_t* p_ADC_VALS_half2 = adc_vals + FFT_SIZE_DIV_2;
volatile size_t adc_ind = 0;
volatile int adc_read_flag = 0;
volatile int adc_read_val;

void SystemClock_Config(void);

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void) {
    HAL_Init();
    SystemClock_Config();

    // init stuff
    ADC_init();
    uart_init();
    arm_rfft_instance_q15 rfft_instance;
    arm_status status = arm_rfft_init_q15(&rfft_instance, FFT_SIZE_DIV_2, 0, 1);
    while (status != ARM_MATH_SUCCESS) {
        status = arm_rfft_init_q15(&rfft_instance, FFT_SIZE_DIV_2, 0, 1);
    }
    timer_init(SAMPLING_RATE);

    // actual code
    uint32_t freq;
    q15_t fft_out[FFT_SIZE];
    q15_t mag_out[FFT_SIZE_DIV_2];
    q15_t max;
    uint32_t max_ind;
    char freq_str[FREQ_STR_LEN];

    while (1) {
        if (adc_read_flag) {
            if (adc_ind == 0) {
                arm_rfft_q15(&rfft_instance, p_ADC_VALS_half2, fft_out);
            }

            else if (adc_ind == FFT_SIZE_DIV_2) {
                arm_rfft_q15(&rfft_instance, p_ADC_VALS_half1, fft_out);
            }

            arm_cmplx_mag_q15(fft_out, mag_out, FFT_SIZE_DIV_2);
            MAG_OUT[0] = 0; // kill DC component
            arm_max_q15(mag_out, FFT_SIZE_DIV_4, &max, &max_ind);
            freq = max_ind * SAMPLING_RATE / FFT_SIZE_DIV_2;
            
            snprintf(freq_str, FREQ_STR_LEN, "Freq: %ld Hz", freq);
            uart_clear_screen();
            uart_send_string(freq_str);
            
            adc_read_flag = 0;
        }

    }


    return 0;
}


void TIM2_IRQHandler() {
    ADC_start_conversion();
    TIM2->SR &= ~TIM_SR_UIF;
    return;
}

void ADC1_2_IRQHandler() {
    adc_vals[adc_ind] = ADC1->DR;
    adc_ind = (adc_ind + 1) & FFT_SIZE_MASK;
    adc_read_flag = (adc_ind == FFT_HALF_POINT || adc_ind == FFT_FULL_POINT);

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
  RCC_OscInitStruct.PLL.PLLN = 30;
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

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_3) != HAL_OK)
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
