#ifndef INTERNAL_TEMP_APP_H_
#define INTERNAL_TEMP_APP_H_

#include "main.h"

/**
  * @brief  Inicializa la aplicación de medición de temperatura interna.
  *         Configura la calibración del ADC y arranca la conversión continua vía DMA.
  * @param  hadc: Puntero al manejador de ADC1.
  * @retval None
  */
void Internal_Temp_App_Init(ADC_HandleTypeDef *hadc);

/**
  * @brief  Tarea cíclica que evalúa si la conversión de DMA ha terminado,
  *         calcula la temperatura compensando las variaciones de VDDA 
  *         y actualiza la lectura almacenada.
  * @retval None
  */
void Internal_Temp_App_Task(void);

/**
  * @brief  Retorna el último valor de temperatura calculado.
  * @retval double: Temperatura en grados Celsius (°C).
  */
double Internal_Temp_App_GetTemp(void);

#endif /* INTERNAL_TEMP_APP_H_ */