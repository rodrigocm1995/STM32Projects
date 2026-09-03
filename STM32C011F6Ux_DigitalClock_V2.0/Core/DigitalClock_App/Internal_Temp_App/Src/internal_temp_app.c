#include "internal_temp_app.h"
#include "stm32c0xx_hal_adc.h"
#include "stm32c0xx_hal_adc_ex.h"
#include <stdint.h>

#define BUFFER_SIZE             2

#define TS_CAL1_ADDR            ((uint16_t*)0x1FFF7568UL)
#define VREFINT_ADDR            ((uint16_t*)0x1FFF756AUL)

#define AVG_SLOPE_MV_PER_C      (2.53) // pendiente
#define VREF_CAL_MV             (3000.0)
#define ADC_MAX_COUNT           (4095.0) 

static ADC_HandleTypeDef *h_adc = NULL;
static uint16_t adcBuffer[BUFFER_SIZE] = {0,0};
static volatile float currentTemperature = 0.0f;
extern volatile _Bool adcIsConvCompleted;                   /* Variable externa controlado por la interrupción del ADC DMA en main.c*/

void Internal_Temp_App_Init(ADC_HandleTypeDef *hadc)
{
    h_adc = hadc;

    if (h_adc == NULL) return;

    // Perform an ADC automatic self-calibration
    HAL_ADCEx_Calibration_Start(h_adc);

    // Arrancar lectura por DMA de 2 canales (Rango 1: Temp Sensor, Rango 2: VrefInt)
    HAL_ADC_Start_DMA(h_adc, (uint32_t *)adcBuffer, BUFFER_SIZE);
}

void Internal_Temp_App_Task(void)
{
    static uint32_t lastCalcTick = 0;
    uint32_t currentTick = HAL_GetTick();

    if (adcIsConvCompleted)
    {
        adcIsConvCompleted = 0;

        // Actualizar el cálculo de temperatura cada 500 ms para optimizar el CPU
        if (currentTick - lastCalcTick >= 500)
        {
            lastCalcTick = currentTick;

            uint16_t rawTemp = adcBuffer[0];
            uint16_t rawVref = adcBuffer[1];

            uint16_t tsCal1 = *TS_CAL1_ADDR;
            uint16_t vrefIntCal = *VREFINT_ADDR;

            if (rawVref > 0)
            {
                // Compensar la medición analógica por variaciones de la tensión de alimentación (VDDA)
                float adcTempScaled = (float)(rawTemp) * ((float)vrefIntCal / (float)rawVref);

                // Calcular el voltaje en milivoltios en ambas mediciones
                float vsense_mV = adcTempScaled * (VREF_CAL_MV / ADC_MAX_COUNT);
                float v30_mV = (float)tsCal1 * (VREF_CAL_MV / ADC_MAX_COUNT);

                currentTemperature = (float)(((vsense_mV - v30_mV) / AVG_SLOPE_MV_PER_C) + 30.0f);
            }
        }
    }
}

float Internal_Temp_App_GetTemp(void)
{
    return currentTemperature;
}

void Internal_Temp_App_Stop(void)
{
    if (h_adc != NULL)
    {
        HAL_ADC_Stop_DMA(h_adc);
        ADC1_COMMON->CCR &= ~(ADC_CCR_TSEN | ADC_CCR_VREFEN);
        __HAL_RCC_ADC_CLK_DISABLE();
    }
}

void Internal_Temp_App_Resume(void)
{
    if (h_adc != NULL)
    {
        __HAL_RCC_ADC_CLK_ENABLE();
        ADC1_COMMON->CCR |= (ADC_CCR_TSEN | ADC_CCR_VREFEN);
        HAL_ADC_Start_DMA(h_adc, (uint32_t *)adcBuffer, BUFFER_SIZE);
    }
}



