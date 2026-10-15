#include "blink.h"

// Variables privadas del módulo (encapsuladas)
static LED_ConfigTypeDef *pLedList   = NULL;
static uint8_t           ledCount   = 0;
static TIM_HandleTypeDef *pTimHandle = NULL;

/**
 * @brief Obtiene automáticamente la frecuencia de reloj del Timer configurado en STM32F3
 */
static uint32_t Get_Timer_Clock_Freq(TIM_HandleTypeDef *htim)
{
    if (htim->Instance == TIM2)
    {
        return HAL_RCCEx_GetPeriphCLKFreq(RCC_PERIPHCLK_TIM2);
    }
#if defined(RCC_PERIPHCLK_TIM34)
    else if (htim->Instance == TIM3 || htim->Instance == TIM4)
    {
        return HAL_RCCEx_GetPeriphCLKFreq(RCC_PERIPHCLK_TIM34);
    }
#endif
#if defined(RCC_PERIPHCLK_TIM1)
    else if (htim->Instance == TIM1)
    {
        return HAL_RCCEx_GetPeriphCLKFreq(RCC_PERIPHCLK_TIM1);
    }
#endif
    else
    {
        uint32_t pclk1 = HAL_RCC_GetPCLK1Freq();
        if ((RCC->CFGR & RCC_CFGR_PPRE1) != RCC_CFGR_PPRE1_DIV1)
        {
            return pclk1 * 2;
        }
        return pclk1;
    }
}

/**
 * @brief Inicializa el módulo, autocalibra los ticks e inicia el timer por IT
 */
void Blink_Init(LED_ConfigTypeDef *configList, uint8_t count, TIM_HandleTypeDef *htim)
{
    if (configList == NULL || htim == NULL || count == 0)
    {
        return;
    }

    pLedList   = configList;
    ledCount   = count;
    pTimHandle = htim;

    // 1. Obtener frecuencia del reloj del timer y registros de tiempo
    uint32_t timClock = Get_Timer_Clock_Freq(htim);
    uint32_t psc = htim->Init.Prescaler + 1;
    uint32_t arr = __HAL_TIM_GET_AUTORELOAD(htim) + 1;

    // Frecuencia real en Hz con la que interrumpe el Timer
    float timerInterruptFreqHz = (float)timClock / (float)(psc * arr);

    // 2. Calcular automáticamente los ticks para cada LED
    for (uint8_t i = 0; i < ledCount; i++)
    {
        uint32_t calculatedTicks = (uint32_t)((pLedList[i].periodMs * timerInterruptFreqHz) / 2000.0f);
        pLedList[i].periodTicks = (calculatedTicks > 0) ? calculatedTicks : 1;
        pLedList[i].currentTicks = 0;
    }

    // 3. Iniciar el Timer con interrupción
    HAL_TIM_Base_Start_IT(htim);
}

/**
 * @brief Tarea periódica de conmutación de pines
 */
void Blink_Process(void)
{
    if (pLedList == NULL)
    {
        return;
    }

    for (uint8_t i = 0; i < ledCount; i++)
    {
        if (pLedList[i].currentTicks >= pLedList[i].periodTicks)
        {
            pLedList[i].currentTicks = 0;
            HAL_GPIO_TogglePin(pLedList[i].LED_Port, pLedList[i].LED_Pin);
        }
    }
}

/**
 * @brief Callback de interrupción de timer desacoplado
 */
void Blink_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
    if (htim == pTimHandle && pLedList != NULL)
    {
        for (uint8_t i = 0; i < ledCount; i++)
        {
            pLedList[i].currentTicks++;
        }
    }
}