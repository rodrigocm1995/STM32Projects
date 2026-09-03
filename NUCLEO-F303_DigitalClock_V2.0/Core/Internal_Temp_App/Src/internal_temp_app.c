#include "internal_temp_app.h"

/* Direcciones de calibración física en la memoria de fábrica de ST para STM32F303 */
#define TS_CAL1_ADDR        ((uint16_t*)0x1FFFF7B8)
#define TS_CAL2_ADDR        ((uint16_t*)0x1FFFF7C2)
#define VREFINT_CAL_ADDR    ((uint16_t*)0x1FFFF7BA)

static ADC_HandleTypeDef *h_adc = NULL;
static uint16_t adcBuffer[2] = {0, 0};
static volatile double current_temperature = 0.0;

/* Variable externa controlada por la interrupción de ADC DMA en main.c */
extern volatile _Bool adcIsConversionCompleted;

void Internal_Temp_App_Init(ADC_HandleTypeDef *hadc)
{
    h_adc = hadc;
    if (h_adc == NULL)
    {
        return;
    }
    
    // Iniciar calibración del ADC en modo Single Ended
    HAL_ADCEx_Calibration_Start(h_adc, ADC_SINGLE_ENDED);
    
    // Arrancar lectura por DMA de 2 canales (Rango 1: Temp Sensor, Rango 2: VrefInt)
    HAL_ADC_Start_DMA(h_adc, (uint32_t*)adcBuffer, 2);
}

void Internal_Temp_App_Task(void)
{
    if (adcIsConversionCompleted)
    {
        adcIsConversionCompleted = 0;

        uint16_t rawTemp = adcBuffer[0];
        uint16_t rawVref = adcBuffer[1];

        uint16_t tsCal1 = *TS_CAL1_ADDR;
        uint16_t tsCal2 = *TS_CAL2_ADDR;
        uint16_t VrefintCal = *VREFINT_CAL_ADDR;

        if (rawVref > 0)
        {
            // Compensar la medición analógica por variaciones de la tensión de alimentación (VDDA)
            double adcTempScaled = (double)rawTemp * ((double)VrefintCal / (double)rawVref);

            // Calcular temperatura por interpolación lineal (30 °C a 110 °C)
            current_temperature = ((110.0 - 30.0) / (double)(tsCal2 - tsCal1)) * (adcTempScaled - (double)tsCal1) + 30.0;
        }
    }
}

double Internal_Temp_App_GetTemp(void)
{
    return current_temperature;
}