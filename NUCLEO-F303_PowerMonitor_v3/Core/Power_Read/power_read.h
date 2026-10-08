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

#ifdef __cplusplus
}
#endif

#endif /* POWER_READ_H */