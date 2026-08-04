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
#include "stm32f3xx_hal.h"

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
#define INA228_ALRT_Pin GPIO_PIN_0
#define INA228_ALRT_GPIO_Port GPIOC
#define RTC_ALRT_Pin GPIO_PIN_1
#define RTC_ALRT_GPIO_Port GPIOC
#define TMP117_ALRT_Pin GPIO_PIN_2
#define TMP117_ALRT_GPIO_Port GPIOC
#define MCP9808_ALRT_Pin GPIO_PIN_3
#define MCP9808_ALRT_GPIO_Port GPIOC
#define INA260_ALRT_Pin GPIO_PIN_4
#define INA260_ALRT_GPIO_Port GPIOC
#define INA238_ALRT_Pin GPIO_PIN_5
#define INA238_ALRT_GPIO_Port GPIOC
#define TFT_A0_Pin GPIO_PIN_0
#define TFT_A0_GPIO_Port GPIOB
#define TFT_RST_Pin GPIO_PIN_1
#define TFT_RST_GPIO_Port GPIOB
#define TFT_CS_Pin GPIO_PIN_2
#define TFT_CS_GPIO_Port GPIOB
#define OUT_LED1_Pin GPIO_PIN_10
#define OUT_LED1_GPIO_Port GPIOB
#define OUT_LED2_Pin GPIO_PIN_11
#define OUT_LED2_GPIO_Port GPIOB
#define OUT_LED3_Pin GPIO_PIN_12
#define OUT_LED3_GPIO_Port GPIOB
#define OUT_LED4_Pin GPIO_PIN_13
#define OUT_LED4_GPIO_Port GPIOB
#define OUT_LED5_Pin GPIO_PIN_14
#define OUT_LED5_GPIO_Port GPIOB
#define OPT3001_ALRT_Pin GPIO_PIN_6
#define OPT3001_ALRT_GPIO_Port GPIOC
#define ADS1219_ALRT_Pin GPIO_PIN_7
#define ADS1219_ALRT_GPIO_Port GPIOC
#define TMP127_CS_Pin GPIO_PIN_8
#define TMP127_CS_GPIO_Port GPIOC
#define SW_OUT1_Pin GPIO_PIN_3
#define SW_OUT1_GPIO_Port GPIOB
#define SW_OUT2_Pin GPIO_PIN_4
#define SW_OUT2_GPIO_Port GPIOB
#define SW_OUT3_Pin GPIO_PIN_5
#define SW_OUT3_GPIO_Port GPIOB
#define SW_OUT4_Pin GPIO_PIN_6
#define SW_OUT4_GPIO_Port GPIOB
#define SW_OUT5_Pin GPIO_PIN_7
#define SW_OUT5_GPIO_Port GPIOB

/* USER CODE BEGIN Private defines */

/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */
