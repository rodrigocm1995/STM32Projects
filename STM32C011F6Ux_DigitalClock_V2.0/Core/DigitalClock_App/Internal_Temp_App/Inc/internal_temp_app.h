/**
  ******************************************************************************
  * @file           : internal_temp_app.h
  * @brief          : Header for STM32C0 internal temperature acquisition application.
  ******************************************************************************
  */

#ifndef INTERNAL_TEMP_APP_H_
#define INTERNAL_TEMP_APP_H_

#include "main.h"
#include "stm32c0xx_hal_adc.h"

/**
  * @brief  Initializes the internal temperature application, calibrating and starting ADC DMA.
  * @param  hadc: Pointer to ADC handle structure.
  * @retval None
  */
void Internal_Temp_App_Init(ADC_HandleTypeDef *hadc);

/**
  * @brief  Periodic background task that processes ADC samples and calculates temperature.
  * @retval None
  */
void Internal_Temp_App_Task(void);

/**
  * @brief  Retrieves the latest calculated temperature in degrees Celsius.
  * @retval float: Current temperature in °C.
  */
float Internal_Temp_App_GetTemp(void);

/**
  * @brief  Disables ADC conversions, DMA, internal sensor buffers and clock for low power mode.
  * @retval None
  */
void Internal_Temp_App_Stop(void);

/**
  * @brief  Restores ADC clock, internal sensor paths, and resumes circular DMA conversions.
  * @retval None
  */
void Internal_Temp_App_Resume(void);

#endif /* INTERNAL_TEMP_APP_H_ */