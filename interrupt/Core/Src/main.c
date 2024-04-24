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
#define SYSCLK 4000000
#define FREQ 5000
#define NEXT_SWITCH SYSCLK / FREQ / 2
#define ARR_BIT TIM_SR_UIF
#define CCR1_BIT TIM_SR_CC1IF

void signal_pin_init(void) {
    // part A
    RCC->AHB2ENR |= RCC_AHB2ENR_GPIOCEN;

    GPIOC->MODER   &= ~(GPIO_MODER_MODE0);          // clear mode bits for pin 5
    GPIOC->MODER   |= GPIO_MODER_MODE0_0;           // output mode
    GPIOC->OTYPER  &= ~(GPIO_OTYPER_OT0);           // push pull
    GPIOC->OSPEEDR &= ~(GPIO_OSPEEDR_OSPEED0_Msk);  // low speed
    GPIOC->PUPDR   &= ~(GPIO_PUPDR_PUPD0);          // no resistor

    GPIOC->BSRR = GPIO_PIN_0;

    // for part B
    GPIOC->MODER   &= ~(GPIO_MODER_MODE1);          // clear mode bits for pin 5
    GPIOC->MODER   |= GPIO_MODER_MODE1_0;           // output mode
    GPIOC->OTYPER  &= ~(GPIO_OTYPER_OT1);           // push pull
    GPIOC->OSPEEDR &= ~(GPIO_OSPEEDR_OSPEED1_Msk);  // low speed
    GPIOC->PUPDR   &= ~(GPIO_PUPDR_PUPD1);          // no resistor

    GPIOC->BSRR = GPIO_PIN_1;

}

// part A
void square_wave(uint32_t freq, uint8_t duty_cycle) {
    RCC->APB1ENR1 |= RCC_APB1ENR1_TIM2EN;

    uint32_t max_arr = SYSCLK / freq ;
    TIM2->ARR = max_arr - 1;
    TIM2->CCR1 = (uint32_t)(max_arr * ((float)duty_cycle / 100));
    TIM2->CCER |= TIM_CCER_CC1E;
    TIM2->DIER |= (TIM_DIER_UIE | TIM_DIER_CC1IE);

    NVIC->ISER[0] = (1 << (TIM2_IRQn & 0x1f));
    __enable_irq();
    TIM2->CR1 |= TIM_CR1_CEN;
    return;
}

// part B
void square_wave_continuous_mode() {
    RCC->APB1ENR1 |= RCC_APB1ENR1_TIM2EN;

    TIM2->ARR = -1;
    TIM2->CCR1 = NEXT_SWITCH - 1;
    TIM2->CCER |= TIM_CCER_CC1E;
    TIM2->DIER |= TIM_DIER_CC1IE;

    NVIC->ISER[0] = (1 << (TIM2_IRQn & 0x1f));
    __enable_irq();
    TIM2->CR1 |= TIM_CR1_CEN;
    return;
}

void TIM2_IRQHandler(void) {
    GPIOC->BSRR = GPIO_PIN_1;

    // for part A
//    if (TIM2->SR & ARR_BIT) {
//        TIM2->SR &= ~(ARR_BIT);
//        GPIOC->BSRR = GPIO_PIN_0;
//    } else if (TIM2->SR & CCR1_BIT) {
//        TIM2->SR &= ~(CCR1_BIT);
//        GPIOC->BRR = GPIO_PIN_0;
//    }

    // for part B
    GPIOC->ODR ^= GPIO_ODR_OD0;
    TIM2->CCR1 += NEXT_SWITCH;
    TIM2->SR &= ~(CCR1_BIT);


    GPIOC->BRR = GPIO_PIN_1;
    return;
}

void SystemClock_Config(void);

int main(void)
{

  /* MCU Configuration--------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
    HAL_Init();

  /* Configure the system clock */
    SystemClock_Config();

    // Enable MCO, select MSI (4 MHz source)
    RCC->CFGR = ((RCC->CFGR & ~(RCC_CFGR_MCOSEL)) | (RCC_CFGR_MCOSEL_0));

    // Configure MCO output on PA8
    RCC->AHB2ENR   |=  (RCC_AHB2ENR_GPIOAEN);
    GPIOA->MODER   &= ~(GPIO_MODER_MODE8);      // alternate function mode
    GPIOA->MODER   |=  (2 << GPIO_MODER_MODE8_Pos);
    GPIOA->OTYPER  &= ~(GPIO_OTYPER_OT8);       // Push-pull output
    GPIOA->PUPDR   &= ~(GPIO_PUPDR_PUPD8);      // no resistor
    GPIOA->OSPEEDR |=  (GPIO_OSPEEDR_OSPEED8);      // high speed
    GPIOA->AFR[1]  &= ~(GPIO_AFRH_AFSEL8);      // select MCO function

    signal_pin_init();
//    square_wave(5000, 25);
    square_wave_continuous_mode();

    uint32_t x = 0;
    while (1) {
      x++;
    }
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
