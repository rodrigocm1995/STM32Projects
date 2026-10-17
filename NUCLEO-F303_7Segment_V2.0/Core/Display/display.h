#ifndef DISPLAY_7_SEG_H_
#define DISPLAY_7_SEG_H_

#include "main.h"
#include "stm32f303xe.h"
#include "stm32f3xx_hal_tim.h"
#include <stdint.h>

typedef struct
{
    GPIO_TypeDef    *Segment_Port;
    uint16_t         Segment_Pin;
} GPIO_PortMap_HandleTypeDef;

/**
 * @brief Inicializa el módulo del display de 7 segmentos con su lista de pines y el timer.
 */
void Display_Init(GPIO_PortMap_HandleTypeDef *segmentPinList, TIM_HandleTypeDef *htim);

/**
 * @brief Tarea periódica no bloqueante. Debe llamarse continuamente dentro del while(1).
 */
void Display_Task(void);

/**
 * @brief Muestra un dígito específico (0 a 9) en el display de 7 segmentos.
 * @param digit Número del 0 al 9 a proyectar.
 */
void Display_ShowDigit(uint8_t digit);

/**
 * @brief Callback que sincroniza la actualización del display con el Timer.
 *        Debe llamarse dentro de HAL_TIM_PeriodElapsedCallback().
 */
void Display_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim);

#endif