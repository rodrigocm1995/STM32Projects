#ifndef POWER_READ_H
#define POWER_READ_H

#include "main.h"
#include "stm32f3xx_hal_def.h"
#include <stdbool.h>

#define USE_PHYSICAL_INA228   1

#ifdef __cplusplus
extern "C" {
#endif

/* Inicializa el sensor físico INA228 en I2C2 */
HAL_StatusTypeDef PowerRead_Init(I2C_HandleTypeDef *hi2c);

/* Lee los datos físicos del INA228 y actualiza g_power_sim */
void PowerRead_Update(uint32_t delta_ms);

/* Lee Manufacturer ID, Device ID y el Registro de Calibración */
HAL_StatusTypeDef PowerRead_GetDeviceInfo(uint16_t *manuf_id, uint16_t *device_id, uint16_t *shunt_cal);

HAL_StatusTypeDef PowerRead_SetAdcRange(uint8_t range);
HAL_StatusTypeDef PowerRead_GetAdcRange(uint8_t *range);

HAL_StatusTypeDef PowerRead_SetMode(uint8_t mode);
HAL_StatusTypeDef PowerRead_GetMode(uint8_t *mode);

HAL_StatusTypeDef PowerRead_SetAverage(uint8_t samples);
HAL_StatusTypeDef PowerRead_GetAverage(uint8_t *samples);

HAL_StatusTypeDef PowerRead_SetConversionDelay(uint16_t delay_ms);
HAL_StatusTypeDef PowerRead_GetConversionDelay(uint16_t *delay_ms);

HAL_StatusTypeDef PowerRead_SetBusConvTime(uint8_t conv_time);
HAL_StatusTypeDef PowerRead_GetBusConvTime(uint8_t *conv_time);

HAL_StatusTypeDef PowerRead_SetShuntConvTime(uint8_t conv_time);
HAL_StatusTypeDef PowerRead_GetShuntConvTime(uint8_t *conv_time);

HAL_StatusTypeDef PowerRead_SetTempConvTime(uint8_t conv_time);
HAL_StatusTypeDef PowerRead_GetTempConvTime(uint8_t *conv_time);

/* Recalcula y envía el registro de calibración SHUNT_CAL al INA228 */
HAL_StatusTypeDef PowerRead_SetCalibration(float rshunt_mohms, float max_current_a);

/* Configuración de Alertas INA228 */
HAL_StatusTypeDef PowerRead_SetAlertLatch(uint8_t latch);
HAL_StatusTypeDef PowerRead_GetAlertLatch(uint8_t *latch);

HAL_StatusTypeDef PowerRead_SetAlertPin(uint8_t cnvr);
HAL_StatusTypeDef PowerRead_GetAlertPin(uint8_t *cnvr);

HAL_StatusTypeDef PowerRead_SetAlertPinPolarity(uint8_t pol);
HAL_StatusTypeDef PowerRead_GetAlertPinPolarity(uint8_t *pol);

HAL_StatusTypeDef PowerRead_SetSlowAlert(uint8_t filter);
HAL_StatusTypeDef PowerRead_GetSlowAlert(uint8_t *filter);

/* Lee el registro completo DIAG_ALRT (0x0B) */
HAL_StatusTypeDef PowerRead_GetDiagAlert(uint16_t *diag_alrt);

/* Configuración de Umbrales (Thresholds) INA228 */
HAL_StatusTypeDef PowerRead_SetShuntOverVoltage(float threshold_mV);
HAL_StatusTypeDef PowerRead_GetShuntOverVoltage(float *threshold_mV);

HAL_StatusTypeDef PowerRead_SetShuntUnderVoltage(float threshold_mV);
HAL_StatusTypeDef PowerRead_GetShuntUnderVoltage(float *threshold_mV);

HAL_StatusTypeDef PowerRead_SetBusOverVoltage(float threshold_V);
HAL_StatusTypeDef PowerRead_GetBusOverVoltage(float *threshold_V);

HAL_StatusTypeDef PowerRead_SetBusUnderVoltage(float threshold_V);
HAL_StatusTypeDef PowerRead_GetBusUnderVoltage(float *threshold_V);

HAL_StatusTypeDef PowerRead_SetTempLimit(float threshold_C);
HAL_StatusTypeDef PowerRead_GetTempLimit(float *threshold_C);

HAL_StatusTypeDef PowerRead_SetPowerLimit(float threshold_W);
HAL_StatusTypeDef PowerRead_GetPowerLimit(float *threshold_W);

/* Resetea los acumuladores de Energía y Carga internos del chip INA228 */
HAL_StatusTypeDef PowerRead_ResetAccumulators(void);

#ifdef __cplusplus
}
#endif

#endif /* POWER_READ_H */