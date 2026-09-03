/**
  ******************************************************************************
  * @file           : clock_driver.h
  * @brief          : Header for Digital Clock application controller.
  ******************************************************************************
  */

#ifndef INC_CLOCK_DRIVER_H_
#define INC_CLOCK_DRIVER_H_

#include "main.h"
#include "stm32c0xx_hal_rtc.h"
#include "stm32c0xx_hal_spi.h"
#include "stm32c0xx_hal_tim.h"

/**
  * @brief Clock operating modes enumeration.
  */
typedef enum
{
    MODE_NORMAL,           /**< Normal mode: Displays current time (HH:MM) */
    MODE_SET_HOURS,        /**< Setting mode: Edits hours with fast blinking */
    MODE_SET_MINUTES,      /**< Setting mode: Edits minutes with fast blinking */
    MODE_READ_TEMPERATURE /**< Temperature display mode (e.g. 24 C) */
} Clock_Mode_TypeDef;

/**
  * @brief  Initializes the digital clock application and binds peripheral handles.
  * @param  hrtc: Pointer to RTC handle structure.
  * @param  hspi: Pointer to SPI handle structure used for TLC5917.
  * @param  htim: Pointer to TIM handle structure used for display multiplexing.
  * @retval None
  */
void Clock_App_Init(RTC_HandleTypeDef *hrtc, SPI_HandleTypeDef *hspi, TIM_HandleTypeDef *htim);

/**
  * @brief  Background cyclic task handling state machines, buttons, segments and power monitoring.
  * @retval None
  */
void Clock_App_Task(void);

/**
  * @brief  Periodic timer interrupt service routine for digit multiplexing (invoked every 5 ms).
  * @retval None
  */
void Clock_App_Multiplex_ISR(void);

/**
  * @brief  RTC alarm interrupt service routine (reserved for periodic RTC events).
  * @retval None
  */
void Clock_App_Alarm_ISR(void);

#endif /* INC_CLOCK_DRIVER_H_ */