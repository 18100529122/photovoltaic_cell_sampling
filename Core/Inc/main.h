/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.h
  * @brief          : Header for main.c file.
  *                   This file contains the common defines of the application.
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2026 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
/* USER CODE END Header */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __MAIN_H
#define __MAIN_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32g4xx_hal.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* Exported types ------------------------------------------------------------*/
/* USER CODE BEGIN ET */

/* USER CODE END ET */

/* Exported constants --------------------------------------------------------*/
/* USER CODE BEGIN EC */

/* USER CODE END EC */

/* Exported macro ------------------------------------------------------------*/
/* USER CODE BEGIN EM */

/* USER CODE END EM */

/* Exported functions prototypes ---------------------------------------------*/
void Error_Handler(void);

/* USER CODE BEGIN EFP */

/* USER CODE END EFP */

/* Private defines -----------------------------------------------------------*/
#define ADC_VS1_Pin GPIO_PIN_0
#define ADC_VS1_GPIO_Port GPIOC
#define ADC_CUR1_Pin GPIO_PIN_1
#define ADC_CUR1_GPIO_Port GPIOC
#define ADC_VS2_Pin GPIO_PIN_2
#define ADC_VS2_GPIO_Port GPIOC
#define ADC_CUR2_Pin GPIO_PIN_3
#define ADC_CUR2_GPIO_Port GPIOC
#define ADC_VS3_Pin GPIO_PIN_0
#define ADC_VS3_GPIO_Port GPIOA
#define ADC_CUR3_Pin GPIO_PIN_1
#define ADC_CUR3_GPIO_Port GPIOA
#define ADC_VS4_Pin GPIO_PIN_2
#define ADC_VS4_GPIO_Port GPIOA
#define ADC_CUR4_Pin GPIO_PIN_3
#define ADC_CUR4_GPIO_Port GPIOA
#define ADC_VS5_Pin GPIO_PIN_4
#define ADC_VS5_GPIO_Port GPIOA
#define ADC_CUR5_Pin GPIO_PIN_5
#define ADC_CUR5_GPIO_Port GPIOA
#define ADC_VS6_Pin GPIO_PIN_6
#define ADC_VS6_GPIO_Port GPIOA
#define ADC_CUR6_Pin GPIO_PIN_7
#define ADC_CUR6_GPIO_Port GPIOA
#define ADC_VS7_Pin GPIO_PIN_4
#define ADC_VS7_GPIO_Port GPIOC
#define ADC_CUR7_Pin GPIO_PIN_5
#define ADC_CUR7_GPIO_Port GPIOC
#define ADC_VS8_Pin GPIO_PIN_0
#define ADC_VS8_GPIO_Port GPIOB
#define ADC_CUR8_Pin GPIO_PIN_1
#define ADC_CUR8_GPIO_Port GPIOB
#define ADC_VS9_Pin GPIO_PIN_7
#define ADC_VS9_GPIO_Port GPIOE
#define ADC_CUR9_Pin GPIO_PIN_8
#define ADC_CUR9_GPIO_Port GPIOE
#define ADC_VS10_Pin GPIO_PIN_9
#define ADC_VS10_GPIO_Port GPIOE
#define ADC_VS10E10_Pin GPIO_PIN_10
#define ADC_VS10E10_GPIO_Port GPIOE
#define ADC_VS11_Pin GPIO_PIN_11
#define ADC_VS11_GPIO_Port GPIOE
#define ADC_CUR11_Pin GPIO_PIN_12
#define ADC_CUR11_GPIO_Port GPIOE
#define ADC_VS12_Pin GPIO_PIN_13
#define ADC_VS12_GPIO_Port GPIOE
#define ADC_CUR12_Pin GPIO_PIN_14
#define ADC_CUR12_GPIO_Port GPIOE
#define ADC_TEMP2_Pin GPIO_PIN_14
#define ADC_TEMP2_GPIO_Port GPIOB
#define ADC_TEMP1_Pin GPIO_PIN_15
#define ADC_TEMP1_GPIO_Port GPIOB
#define LED1_Pin GPIO_PIN_14
#define LED1_GPIO_Port GPIOD

/* USER CODE BEGIN Private defines */

/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */
