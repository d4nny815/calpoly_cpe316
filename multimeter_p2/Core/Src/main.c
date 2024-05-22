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
#define ARM_MATH_CM4

#include "main.h"
#include "stdio.h"
#include "uart.h"
#include "ADC.h"
#include "arm_math.h"

// UART PRINTING STUFF
#define MODE_CURSOR "[1;7H"
#define VOLT_CURSOR "[3;7H"
#define SCALE_CURSOR "[5;0H"
#define FREQ_CURSOR "[9;12H"
#define MOVE_CURSOR(x) uart_send_escape(x)
#define PRINT_PERIOD_SEC (1.5f)
#define PRINT_RATE ((double)(1 / PRINT_PERIOD_SEC))
#define PRINT_CCR ((uint32_t)(CPU_FREQ/PRINT_RATE))
void print_stats(int AC, uint16_t voltage, uint16_t freq);
void print_start_screen();
void mv_to_str(char* buffer, uint16_t volt);



// DC MODE STUFF
#define DC_VOLT_SIZE (1024)
uint16_t get_avg_arr(uint16_t *arr, int size);


// AC MODE STUFF 
#define FFT_SIZE (2048)
#define FFT_SIZE_DIV_2 (FFT_SIZE/2)
#define FFT_SIZE_MASK (FFT_SIZE - 1)
#define SAMPLING_RATE ((uint32_t)(FFT_SIZE))
#define FFT_CCR ((uint32_t)(CPU_FREQ/SAMPLING_RATE))


#define LED1_ON() (GPIOC->BSRR = GPIO_BSRR_BS0)
#define LED1_OFF() (GPIOC->BSRR = GPIO_BSRR_BR0)
#define LED2_ON() (GPIOC->BSRR = GPIO_BSRR_BS1)
#define LED2_OFF() (GPIOC->BSRR = GPIO_BSRR_BR1)



void timer_init();

typedef enum {
    DC_ST,
    AC_ST
} State_t;

State_t state = AC_ST;

volatile int working = 0;
volatile int start_calcs = 0;
volatile int fft_work = 0;

void SystemClock_Config(void);

int main(void) {
    HAL_Init();
    SystemClock_Config();


    // * init  
    // helper led
    RCC->AHB2ENR |= RCC_AHB2ENR_GPIOCEN;
    GPIOC->MODER &= ~(GPIO_MODER_MODE0 | GPIO_MODER_MODE1);
    GPIOC->MODER |= GPIO_MODER_MODE0_0 | GPIO_MODER_MODE1_0;
    GPIOC->PUPDR &= ~(GPIO_PUPDR_PUPD0 | GPIO_PUPDR_PUPD1) ;
    GPIOC->OTYPER &= ~(GPIO_OTYPER_OT0 | GPIO_OTYPER_OT1);
    GPIOC->OSPEEDR |= GPIO_OSPEEDR_OSPEED0 | GPIO_OSPEEDR_OSPEED1;
    LED1_OFF();
    LED2_OFF();

    uart_init();
    print_start_screen();
    // print_stats(0, 2680, 333);

    ADC_init();
    arm_rfft_instance_q15 rfft_instance;
    arm_status status = arm_rfft_init_q15(&rfft_instance, FFT_SIZE, 0, 1);
    while (status != ARM_MATH_SUCCESS) {
        status = arm_rfft_init_q15(&rfft_instance, FFT_SIZE, 0, 1);
    }

    timer_init();

    // * real work
    int new_screen = 0;

    uint16_t adc_vals[DC_VOLT_SIZE];
    size_t dc_ind = 0;

    uint16_t voltage;

    
    uint16_t freq;
    q15_t ac_adc_vals[FFT_SIZE];
    size_t ac_ind = 0;
    q15_t fft_out[FFT_SIZE * 2];
    q15_t mag_out[FFT_SIZE];
    q15_t max;
    uint32_t max_ind;


    while (1) {
        // change mode
        if (uart_check_flag()) { 
            switch (get_uart_char()) {
            case 'd':
                // print_stats(0, 263, 3133);
                // ADC_start_conversion();
                working = 1;
                state = DC_ST;
                break;
            case 'a':
                // print_stats(1, 1720, 127);
                working = 1;
                state = AC_ST;
                break;
            default: break;
            }
            
            uart_clear_flag();
        } 

        if (!working) continue; // idle

        if (start_calcs) {
            start_calcs = 0;
            fft_work = 1;
            ADC_start_conversion();
        }

        switch (state) {
        case DC_ST:
            if (new_screen) {
                print_stats(0, voltage, 0);
                new_screen = 0;
                working = 0;
            }

            if (ADC_check_flag()) {
                adc_vals[dc_ind] = (get_ADC_val() >> 3) & 0xfff;
                dc_ind++;
                
                if (dc_ind == DC_VOLT_SIZE) { // done getting measurements
                    dc_ind = 0;
                    voltage = get_avg_arr(adc_vals, DC_VOLT_SIZE);
                    voltage = ADC_to_mv(voltage);
                    new_screen = 1;


                    ADC_clear_flag();
                    break;
                }

                ADC_clear_flag();
                ADC_start_conversion();
            }

            break;
        case AC_ST:
            if (new_screen) {
                print_stats(1, 1200, freq);
                new_screen = 0;
                working = 0;
                LED1_OFF();
            }

            if (fft_work) {
                if (ADC_check_flag()) {
                    ac_adc_vals[ac_ind] = get_ADC_val();
                    ac_ind++;

                    if (ac_ind == FFT_SIZE) {
                        ac_ind = 0;

                        arm_rfft_q15(&rfft_instance, ac_adc_vals, fft_out);
                        arm_cmplx_mag_q15(fft_out, mag_out, FFT_SIZE);
                        mag_out[0] = 0;
                        arm_max_q15(mag_out, FFT_SIZE_DIV_2, &max, &max_ind);
                        freq = max_ind * SAMPLING_RATE / FFT_SIZE;

                        fft_work = 0;
                        new_screen = 1;
                    }

                    ADC_clear_flag();
                }
            }

            break;
        }

        
    }

    return 0;
}

void TIM2_IRQHandler() {
    if (TIM2->SR & TIM_SR_CC1IF) { // print interval
        LED2_ON();
        LED1_ON();
        working = 1;
        start_calcs = 1;
        // if (state == DC_ST) ADC_start_conversion();

        TIM2->CCR1 += PRINT_CCR;
        TIM2->SR &= ~TIM_SR_CC1IF;
        LED2_OFF();
    }
    else if (TIM2->SR & TIM_SR_CC2IF) { // get adc samples for fft
        if (state == AC_ST && fft_work) {
            ADC_start_conversion();
        }

        TIM2->CCR2 += FFT_CCR;
        TIM2->SR &= ~TIM_SR_CC2IF;
    }


    return;
}



void timer_init() {
    RCC->APB1ENR1 |= RCC_APB1ENR1_TIM2EN;

    TIM2->ARR = -1;
    TIM2->CCR1 = PRINT_CCR - 1;
    TIM2->CCR2 = FFT_CCR - 1;
    TIM2->PSC = 0;
    TIM2->DIER = TIM_DIER_CC1IE | TIM_DIER_CC2IE;
    TIM2->CCER = TIM_CCER_CC1E | TIM_CCER_CC2E;

    DBGMCU->APB1FZR1 |= 1;

    NVIC_EnableIRQ(TIM2_IRQn);
    __enable_irq();
    TIM2->CR1 |= TIM_CR1_CEN;

    return;
}


void print_stats(int AC, uint16_t voltage, uint16_t freq) {
    static char voltage_buffer[5] = "0.00";
    static char freq_buffer[15] = "1000 Hz    ";
    
    MOVE_CURSOR(MODE_CURSOR);
    if (AC) uart_send_string("AC");
    else uart_send_string("DC");
    
    MOVE_CURSOR(VOLT_CURSOR);
    mv_to_str(voltage_buffer, voltage);
    uart_send_string(voltage_buffer);


    MOVE_CURSOR(SCALE_CURSOR);
    uint16_t x = 0;
    uart_send_escape("[2K");
    while (x <= voltage) {
        uart_send_string("#");
        x += 100;
    }

    MOVE_CURSOR(FREQ_CURSOR);
    snprintf(freq_buffer, 15, "%hu Hz   ", freq);
    uart_send_string(freq_buffer);

    return;
}


void print_start_screen() {
    uart_send_escape("[1;0H");
    uart_send_string("Mode: ");

    uart_send_escape("[3;0H");
    uart_send_string("Volt:      V");

    uart_send_escape("[6;0H");
    uart_send_string("|----|----|----|----|----|----|");

    uart_send_escape("[7;0H");
    uart_send_string("0.0  0.5  1.0  1.5  2.0  2.5  3.0");


    uart_send_escape("[9;0H");
    uart_send_string("Frequency:");
    return;
}


void mv_to_str(char* buffer, uint16_t volt) {
    // 3000 -> 3.00
    buffer[0] = (volt / 1000) + '0';
    buffer[2] = ((volt % 1000) / 100) + '0';
    buffer[3] = ((volt % 100) / 10) + '0';

    return;
}

uint16_t get_avg_arr(uint16_t *arr, int size) {
    uint32_t sum = 0;
    for (int i = 0; i < size; i++) {
        sum += arr[i];
    }
    return sum / size;
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
  RCC_OscInitStruct.PLL.PLLN = 40;
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

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_4) != HAL_OK)
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
