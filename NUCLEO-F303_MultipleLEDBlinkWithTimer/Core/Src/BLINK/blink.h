#ifndef BLINK_H_
#define BLINK_H_

#include "main.h"
#include <stdint.h>
#include <stddef.h>

/**
 * @brief Estructura descriptora de configuración para cada LED
 */
typedef struct
{
    GPIO_TypeDef        *LED_Port;      // Puerto GPIO (ej. GPIOB, GPIOA)
    uint16_t            LED_Pin;        // Pin GPIO (ej. LED_1_Pin)
    uint32_t            periodMs;       // Período de ciclo completo en ms
    uint32_t            periodTicks;    // Ticks calculados automáticamente por el módulo
    volatile uint32_t   currentTicks;   // Contador de ticks en ejecución
} LED_ConfigTypeDef;

/**
 * @brief Inicializa el módulo Blink, registra la tabla de LEDs, calibra
 *        los tiempos automáticamente según el Timer y arranca el Timer por IT.
 * @param configList Arreglo con la lista de LEDs configurados
 * @param count      Número de LEDs en el arreglo
 * @param htim       Puntero al timer configurado (ej. &htim2, &htim3)
 */
void Blink_Init(LED_ConfigTypeDef *configList, uint8_t count, TIM_HandleTypeDef *htim);

/**
 * @brief Tarea periódica de parpadeo. Debe llamarse continuamente dentro del bucle while(1).
 */
void Blink_Process(void);

/**
 * @brief Manejador de la interrupción del Timer. Debe llamarse desde HAL_TIM_PeriodElapsedCallback().
 * @param htim Puntero al timer que generó la interrupción
 */
void Blink_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim);

#endif /* BLINK_H_ */