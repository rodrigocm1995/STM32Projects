/**
  ******************************************************************************
  * @file           : internal_temp_app.c
  * @brief          : Implementation for STM32C0 internal temperature sensor acquisition.
  ******************************************************************************
  */

#include "internal_temp_app.h"
#include "stm32c011xx.h"
#include "stm32c0xx.h"
#include "stm32c0xx_hal_adc.h"
#include "stm32c0xx_hal_adc_ex.h"
#include <stdint.h>

#define BUFFER_SIZE             2

#define TS_CAL1_ADDR            ((uint16_t*)0x1FFF7568UL)
#define VREFINT_ADDR            ((uint16_t*)0x1FFF756AUL)

#define AVG_SLOPE_MV_PER_C      (2.53)   /**< Average slope in mV/°C from datasheet */
#define VREF_CAL_MV             (3000.0) /**< Calibration reference voltage in mV */
#define ADC_MAX_COUNT           (4095.0) /**< 12-bit ADC full scale resolution */

static ADC_HandleTypeDef *h_adc = NULL;
static uint16_t adcBuffer[BUFFER_SIZE] = {0, 0};
static volatile float currentTemperature = 0.0f;
extern volatile _Bool adcIsConvCompleted; /**< Flag set by ADC DMA conversion complete callback in main.c */

/**
  * @brief  Initializes the internal temperature application, performs calibration and starts DMA.
  * @param  hadc: Pointer to ADC handle structure.
  * @retval None
  */
void Internal_Temp_App_Init(ADC_HandleTypeDef *hadc)
{
    h_adc = hadc;

    if (h_adc == NULL) return;

    // Perform an ADC automatic self-calibration
    HAL_ADCEx_Calibration_Start(h_adc);

    // Start 2-channel circular DMA acquisition (Rank 1: Temp Sensor, Rank 2: VrefInt)
    HAL_ADC_Start_DMA(h_adc, (uint32_t *)adcBuffer, BUFFER_SIZE);
}

/**
  * @brief  Processes acquired raw ADC samples at 500 ms intervals and computes temperature.
  * @retval None
  */
void Internal_Temp_App_Task(void)
{
    static uint32_t lastCalcTick = 0;
    uint32_t currentTick = HAL_GetTick();

    if (adcIsConvCompleted)
    {
        adcIsConvCompleted = 0;

        // Rate-limit temperature calculation to every 500 ms to optimize CPU load
        if (currentTick - lastCalcTick >= 500)
        {
            lastCalcTick = currentTick;

            uint16_t rawTemp = adcBuffer[0];
            uint16_t rawVref = adcBuffer[1];

            uint16_t tsCal1 = *TS_CAL1_ADDR;
            uint16_t vrefIntCal = *VREFINT_ADDR;

            if (rawVref > 0)
            {
                // Compensate raw analog reading for supply voltage (VDDA) variations using VREFINT
                float adcTempScaled = (float)(rawTemp) * ((float)vrefIntCal / (float)rawVref);

                // Calculate sensor voltages in millivolts
                float vsense_mV = adcTempScaled * (VREF_CAL_MV / ADC_MAX_COUNT);
                float v30_mV = (float)tsCal1 * (VREF_CAL_MV / ADC_MAX_COUNT);

                // Compute real temperature using point-slope formula based on 30 °C calibration byte
                currentTemperature = (float)(((vsense_mV - v30_mV) / AVG_SLOPE_MV_PER_C) + 30.0f);
            }
        }
    }
}

/**
  * @brief  Retrieves the latest computed temperature in degrees Celsius.
  * @retval float: Temperature in °C.
  */
float Internal_Temp_App_GetTemp(void)
{
    return currentTemperature;
}

/**
  * @brief  Disables ADC conversions, DMA, internal sensor buffers and clock for low-power mode.
  * @retval None
  */
void Internal_Temp_App_Stop(void)
{
    if (h_adc != NULL)
    {
        HAL_ADC_Stop_DMA(h_adc);

        // Clear internal path enable bits (TSEN and VREFEN) in ADC_CCR before entering STOP mode
        // Reference: RM0490 section 16.12.16 p.343
        CLEAR_BIT(ADC1_COMMON->CCR, ADC_CCR_TSEN | ADC_CCR_VREFEN);

        // Gate off the peripheral clock to eliminate dynamic power consumption
        __HAL_RCC_ADC_CLK_DISABLE();
    }
}

/**
  * @brief  Restores ADC clock, internal sensor paths, and resumes circular DMA conversions.
  * @retval None
  */
void Internal_Temp_App_Resume(void)
{
    if (h_adc != NULL)
    {
        // Re-enable peripheral clock
        __HAL_RCC_ADC_CLK_ENABLE();

        // Restore internal analog paths for Temperature Sensor and VREFINT
        SET_BIT(ADC1_COMMON->CCR, ADC_CCR_TSEN | ADC_CCR_VREFEN);

        // Resume continuous circular DMA acquisition
        HAL_ADC_Start_DMA(h_adc, (uint32_t *)adcBuffer, BUFFER_SIZE);
    }
}



