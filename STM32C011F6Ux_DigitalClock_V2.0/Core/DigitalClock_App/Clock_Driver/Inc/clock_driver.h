#ifndef INC_CLOCK_DRIVER_H_
#define INC_CLOCK_DRIVER_H_

#include "main.h"
#include "stm32c0xx_hal_rtc.h"
#include "stm32c0xx_hal_spi.h"

/* Enum para registrar e identificar los distintos modos de operación del reloj */
typedef enum
{
    MODE_NORMAL,
    MODE_SET_HOURS,
    MODE_SET_MINUTES,
    MODE_READ_TEMPERATURE
} Clock_Mode_TypeDef;

/**
  * @brief  Inicializa la aplicación de reloj digital, asociando los buses periféricos.
  * @param  hrtc: Puntero al manejador del RTC.
  * @param  hspi: Puntero al manejador de SPI del TLC5917.
  * @retval None
  */
void Clock_App_Init(RTC_HandleTypeDef *hrtc, SPI_HandleTypeDef *hspi);

/**
  * @brief  Tarea de fondo que ejecuta la lógica principal del reloj, 
  *         la máquina de estados, el botón B1 y el refresco de buffers.
  * @retval None
  */
void Clock_App_Task(void);

/**
  * @brief  Función de interrupción periódica para multiplexar los dígitos (5 ms).
  * @retval None
  */
void Clock_App_Multiplex_ISR(void);

/**
  * @brief  Función de interrupción de Alarma RTC para alternar dos puntos (:).
  * @retval None
  */
void Clock_App_Alarm_ISR(void);

#endif /* INC_CLOCK_DRIVER_H_ */