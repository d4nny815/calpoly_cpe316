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




// DC MODE STUFF
#define DC_VOLT_SIZE (2048)
uint16_t get_avg_arr(uint16_t *arr, int size);
uint16_t get_min_arr(uint16_t *arr, int size);
uint16_t get_max_arr(uint16_t *arr, int size);


// AC MODE STUFF 
// FREQ STUFF
#define FFT_SIZE (2048)
#define FFT_SIZE_DIV_2 (FFT_SIZE/2)
#define FFT_SIZE_MASK (FFT_SIZE - 1)
#define SAMPLING_RATE ((uint32_t)(FFT_SIZE))
#define FFT_CCR ((uint32_t)(CPU_FREQ/SAMPLING_RATE))
// RMS STUFF
struct VoltStats_t {
    uint16_t vpp_mv;
    uint16_t voltage;
};
#define RMS_SIZE (FFT_SIZE)
#define RMS_SAMPLE_RATE ((uint32_t)RMS_SIZE)
#define RMS_CCR ((uint32_t)(CPU_FREQ/RMS_SAMPLE_RATE))
#define MV_TO_V_FLOAT(x) ((double)x/1000)
void get_voltage_stats(struct VoltStats_t *stats, uint16_t *arr, int size, uint16_t freq);

// UART PRINTING STUFF
#define MODE_CURSOR "[1;7H"
#define VOLT_CURSOR "[3;7H"
#define SCALE_CURSOR "[5;0H"
#define FREQ_CURSOR "[9;12H"
#define VPP_CURSOR "[11;7H"
#define MOVE_CURSOR(x) uart_send_escape(x)
#define PRINT_PERIOD_SEC (2.1f)
#define PRINT_RATE ((double)(1 / PRINT_PERIOD_SEC))
#define PRINT_CCR ((uint32_t)(CPU_FREQ/PRINT_RATE))
void print_stats(int AC, struct VoltStats_t stats, uint16_t freq);
void print_start_screen();
void mv_to_str(char* buffer, uint16_t volt);

void timer_init();

typedef enum {
    IDLE_ST,
    DC_ST,
    FFT_ST,
    VRMS_ST,
    PRINT_ST
} State_t;
volatile State_t state = IDLE_ST;
int AC = 1;


void SystemClock_Config(void);

int main(void) {
    HAL_Init();
    SystemClock_Config();


    // * init  
    uart_init();
    print_start_screen();

    ADC_init();
    arm_rfft_instance_q15 rfft_instance;
    arm_status status = arm_rfft_init_q15(&rfft_instance, FFT_SIZE, 0, 1);
    while (status != ARM_MATH_SUCCESS) {
        status = arm_rfft_init_q15(&rfft_instance, FFT_SIZE, 0, 1);
    }

    timer_init();

    // * real work
    uint16_t voltage;
    uint16_t freq = 0;

    uint16_t dc_adc_vals[DC_VOLT_SIZE];
    size_t dc_ind = 0;

    q15_t ac_adc_vals[FFT_SIZE];
    size_t ac_ind = 0;
    q15_t fft_out[FFT_SIZE * 2];
    q15_t mag_out[FFT_SIZE];
    q15_t max;
    uint32_t max_ind;


    uint16_t rms_adc_vals[RMS_SIZE];
    size_t rms_ind = 0;
    struct VoltStats_t stats = {0, 0};


    while (1) {
        switch (state) {
        case IDLE_ST:

            break;
        case DC_ST:
            if (ADC_check_flag()) {
                ADC_clear_flag();
                dc_adc_vals[dc_ind] = get_ADC_val();
                dc_ind++;
                
                if (dc_ind == DC_VOLT_SIZE) { // done getting measurements
                    dc_ind = 0;
                    stats.voltage = get_avg_arr(dc_adc_vals, DC_VOLT_SIZE);
                    stats.voltage = ADC_to_mv(stats.voltage);

                    state = PRINT_ST;
                    break;
                }

                ADC_start_conversion();
            }

            break;
        case FFT_ST:
            if (ADC_check_flag()) {
                ADC_clear_flag();

                ac_adc_vals[ac_ind] = get_ADC_val() << 3;
                ac_ind++;
                if (ac_ind == FFT_SIZE) {
                    ac_ind = 0;
                    arm_rfft_q15(&rfft_instance, ac_adc_vals, fft_out);
                    arm_cmplx_mag_q15(fft_out, mag_out, FFT_SIZE);
                    mag_out[0] = 0;
                    arm_max_q15(mag_out, FFT_SIZE_DIV_2, &max, &max_ind);
                    freq = max_ind * SAMPLING_RATE / FFT_SIZE;

                    state = VRMS_ST;
                    ADC_ABORT_1();
                }
            }

            break;
        case VRMS_ST:
            if (ADC_check_flag()) {
                ADC_clear_flag();
                voltage = get_ADC_val();
                rms_adc_vals[rms_ind] = ADC_to_mv(voltage);
                rms_ind++;
                if (rms_ind == RMS_SIZE) {
                    rms_ind = 0;
                    get_voltage_stats(&stats, rms_adc_vals, RMS_SIZE, freq);
                    voltage = stats.voltage;
                    state = PRINT_ST;
                    ADC_ABORT_1();
                }
            }

            break;
        case PRINT_ST:
            print_stats(AC, stats, freq);
            state = IDLE_ST;
            break;
        } 
    }

    return 0;
}



void TIM2_IRQHandler() {
    if (TIM2->SR & TIM_SR_CC1IF) { // print interval

        if (uart_check_flag()) {
            uart_clear_flag();
            switch (get_uart_char()) {
            case 'd':
                AC = 0;
                    break;
            case 'a':
                AC = 1;
                break;
            default: break;
            }
        }

        if (AC) { state = FFT_ST; }
        else { state = DC_ST; }

        TIM2->CCR1 += PRINT_CCR;
        TIM2->SR &= ~TIM_SR_CC1IF;
    }
    if (TIM2->SR & TIM_SR_CC2IF) { // get adc samples for fft
        ADC_start_conversion();

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
    TIM2->DIER = TIM_DIER_CC1IE | TIM_DIER_CC2IE ;
    TIM2->CCER = TIM_CCER_CC1E | TIM_CCER_CC2E ;

    DBGMCU->APB1FZR1 |= 1;

    NVIC_EnableIRQ(TIM2_IRQn);
    __enable_irq();
    TIM2->CR1 |= TIM_CR1_CEN;

    return;
}



void print_stats(int AC, struct VoltStats_t v_stats, uint16_t freq) {
    static char voltage_buffer[5] = "0.00";
    static char freq_buffer[15] = "1000 Hz    ";
    
    MOVE_CURSOR(MODE_CURSOR);
    if (AC) uart_send_string("AC");
    else uart_send_string("DC");
    
    MOVE_CURSOR(VOLT_CURSOR);
    mv_to_str(voltage_buffer, v_stats.voltage);
    uart_send_string(voltage_buffer);


    MOVE_CURSOR(SCALE_CURSOR);
    uint16_t x = 0;
    uart_send_escape("[2K");
    while (x <= v_stats.voltage) {
        uart_send_string("#");
        x += 100;
    }

    MOVE_CURSOR(FREQ_CURSOR);
    if (!AC) freq = 0;
    snprintf(freq_buffer, 15, "%hu Hz   ", freq);
    uart_send_string(freq_buffer);

    MOVE_CURSOR(VPP_CURSOR);
    mv_to_str(voltage_buffer, v_stats.vpp_mv);
    uart_send_string(voltage_buffer);

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

    uart_send_escape("[11;0H");
    uart_send_string("Vpp:      V");
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


uint16_t get_min_arr(uint16_t *arr, int size) {
    uint16_t min = (uint16_t)arr[0];
    uint16_t val;
    for (int i = 1; i < size; i++) {
        val = *(((uint16_t*)arr) + i);
        if (val < min) {
            min = val;
        }
    }
    return min;
}


uint16_t get_max_arr(uint16_t *arr, int size) {
    uint16_t max = (uint16_t)arr[0];
    uint16_t val;
    for (int i = 1; i < size; i++) {
        val = *(((uint16_t*)arr) + i); // from when it was q15_t
        if (val > max) {
            max = val;
        }
    }
    return max;
}

// pretty sure I can just reuse this for dc average
uint16_t get_ac_rms(uint16_t *arr, int size) {
    double sum = 0;
    double volt;
    // can probably leave in mV but i spent too long on this :')
    for (int i = 0; i < size; i++) {
        volt = MV_TO_V_FLOAT(arr[i]);
        sum += volt * volt;
    }
    sum /= size;
    sum = sqrt(sum);

    return (uint16_t)(sum * 1000);
}


void get_voltage_stats(struct VoltStats_t *stats, uint16_t *arr, int size, uint16_t freq) {
    uint16_t v_max = get_max_arr(arr, size);
    uint16_t v_min = get_min_arr(arr, size);
    stats->vpp_mv = v_max - v_min;
    stats->voltage = get_ac_rms(arr, size);

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
