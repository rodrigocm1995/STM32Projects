/**
  ******************************************************************************
  * @file    CST328.h
  * @brief   Header file for CST328 Capacitive Touch Controller (STM32 HAL).
  ******************************************************************************
  */

#ifndef __CST328_H
#define __CST328_H

#ifdef __cplusplus
extern "C" {
#endif

#include "main.h"

/* Dirección I2C del CST328 (0x1A desplazado 1 bit a la izquierda para HAL = 0x34) */
#define CST328_I2C_ADDR                     (0x58 << 1)

/* Registros del controlador */
#define CST328_REG_DEBUG_INFO_MODE          0xD101
#define CST328_REG_DEBUG_INFO_TP_NTX        0xD1F4
#define CST328_REG_NORMAL_MODE              0xD109
#define CST328_REG_READ_NUMBER              0xD005
#define CST328_REG_READ_XY                  0xD000

#define CST328_MAX_TOUCH_POINTS             5

/* Estructura de configuración dinámica del CST328 */
typedef struct {
    I2C_HandleTypeDef   *hi2c;
    GPIO_TypeDef        *RST_Port;  /* Puerto del Pin RST (puede ser NULL si no se usa) */
    uint16_t            RST_Pin;    /* Pin de RST */
    GPIO_TypeDef        *INT_Port;  /* Opcional: Puerto de interrupción (NULL si no se usa) */
    uint16_t            INT_Pin;    /* Pin de interrupción */
} CST328_Config_HandleTypeDef;

typedef struct {
    uint16_t x;
    uint16_t y;
    uint16_t strength;
} CST328_TouchPoint_TypeDef;

typedef struct {
    uint8_t points;
    CST328_TouchPoint_TypeDef coords[CST328_MAX_TOUCH_POINTS];
    uint8_t pressed;
} CST328_TouchData_HandleTypeDef;

/* Alias de compatibilidad */
typedef CST328_Config_HandleTypeDef CST328_Config_t;
typedef CST328_TouchData_HandleTypeDef CST328_TouchData_t;

/* Prototipos de funciones públicas */
uint8_t CST328_Init(const CST328_Config_HandleTypeDef *config);
void CST328_Reset(void);
uint16_t CST328_ReadConfig(void);
uint8_t CST328_ReadTouch(CST328_TouchData_HandleTypeDef *touch_data);

#ifdef __cplusplus
}
#endif

#endif /* __CST328_H */