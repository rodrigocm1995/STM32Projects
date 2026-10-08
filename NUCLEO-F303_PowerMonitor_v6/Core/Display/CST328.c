/**
  ******************************************************************************
  * @file    CST328.c (Driver para Waveshare 2.8" V2 - CST3530)
  * @brief   Implementation file for CST3530 Capacitive Touch Driver.
  ******************************************************************************
  */

#include "CST328.h"
#include <string.h>

#define CST3530_DATA_REG        0xD0070000
#define CST3530_END_READ_REG    0xD00002AB

static void CST3530_I2C_ClearBus(void);
static CST328_Config_HandleTypeDef cst_cfg;
static uint32_t s_last_touch_error_tick = 0;

/* Escritura I2C con registro de 32 bits */
static HAL_StatusTypeDef CST3530_I2C_Write_32(uint32_t reg, const uint8_t *val, uint16_t len) 
{
    if (cst_cfg.hi2c == NULL) return HAL_ERROR;
    
    if (cst_cfg.hi2c->State != HAL_I2C_STATE_READY && cst_cfg.hi2c->State != HAL_I2C_STATE_RESET) {
        CST3530_I2C_ClearBus();
        return HAL_ERROR;
    }
    uint8_t buf[16];
    buf[0] = (uint8_t)(reg >> 24);
    buf[1] = (uint8_t)(reg >> 16);
    buf[2] = (uint8_t)(reg >> 8);
    buf[3] = (uint8_t)(reg & 0xFF);
    if (len > 0 && val != NULL) {
        if (len > 12) len = 12;
        memcpy(&buf[4], val, len);
    }
    
    HAL_StatusTypeDef res = HAL_I2C_Master_Transmit(cst_cfg.hi2c, CST328_I2C_ADDR, buf, 4 + len, 15);
    if (res != HAL_OK) {
        CST3530_I2C_ClearBus();
    }
    return res;
}

/* Lectura I2C con registro de 32 bits */
static HAL_StatusTypeDef CST3530_I2C_Read_32(uint32_t reg, uint8_t *val, uint16_t len) 
{
    if (cst_cfg.hi2c == NULL || val == NULL) return HAL_ERROR;
    
    if (cst_cfg.hi2c->State != HAL_I2C_STATE_READY && cst_cfg.hi2c->State != HAL_I2C_STATE_RESET) {
        CST3530_I2C_ClearBus();
        return HAL_ERROR;
    }
    uint8_t reg_buf[4] = {
        (uint8_t)(reg >> 24),
        (uint8_t)(reg >> 16),
        (uint8_t)(reg >> 8),
        (uint8_t)(reg & 0xFF)
    };
    
    if (HAL_I2C_Master_Transmit(cst_cfg.hi2c, CST328_I2C_ADDR, reg_buf, 4, 15) != HAL_OK) {
        CST3530_I2C_ClearBus();
        return HAL_ERROR;
    }
    
    HAL_StatusTypeDef res = HAL_I2C_Master_Receive(cst_cfg.hi2c, CST328_I2C_ADDR, val, len, 15);
    if (res != HAL_OK) {
        CST3530_I2C_ClearBus();
    }
    return res;
}

void CST328_Reset(void) 
{
    if (cst_cfg.RST_Port == NULL) return;

    HAL_GPIO_WritePin(cst_cfg.RST_Port, cst_cfg.RST_Pin, GPIO_PIN_SET);
    HAL_Delay(20);
    HAL_GPIO_WritePin(cst_cfg.RST_Port, cst_cfg.RST_Pin, GPIO_PIN_RESET);
    HAL_Delay(10);
    HAL_GPIO_WritePin(cst_cfg.RST_Port, cst_cfg.RST_Pin, GPIO_PIN_SET);
    HAL_Delay(50);
}

uint16_t CST328_ReadConfig(void) 
{
    return 0x3530; /* CST3530 no usa registro de firma; devuelve su modelo */
}

uint8_t CST328_Init(const CST328_Config_HandleTypeDef *config) 
{
    if (config == NULL || config->hi2c == NULL) return 0;

    cst_cfg = *config;
    CST328_Reset();

    /* El CST3530 queda listo para operar inmediatamente tras el reset */
    return 1;
}

uint8_t CST328_ReadTouch(CST328_TouchData_HandleTypeDef *touch_data) 
{
    uint8_t buf[12] = {0};

    if (!touch_data || !cst_cfg.hi2c) return 0;

    touch_data->pressed = 0;
    touch_data->points = 0;

    /* Si hubo un error reciente en I2C1, esperar 500ms antes de reintentar para no saturar la CPU */
    if (HAL_GetTick() - s_last_touch_error_tick < 500) {
        return 0;
    }

    /* 1. Leer 9 bytes desde el registro de datos CST3530 */
    if (CST3530_I2C_Read_32(CST3530_DATA_REG, buf, 9) != HAL_OK) {
        s_last_touch_error_tick = HAL_GetTick();
        return 0;
    }

    /* 2. Comprobar si hay toque real: buf[3] indica dedos y buf[8] valida el contacto */
    if ((buf[3] & 0x0F) == 0 || (buf[8] & 0xF0) == 0) {
        CST3530_I2C_Write_32(CST3530_END_READ_REG, NULL, 0); /* Fin de lectura */
        return 0;
    }

    uint8_t touch_cnt = buf[3] & 0x0F;
    if (touch_cnt > CST328_MAX_TOUCH_POINTS || touch_cnt == 0) {
        CST3530_I2C_Write_32(CST3530_END_READ_REG, NULL, 0);
        return 0;
    }

    /* 3. Enviar confirmación de lectura al controlador */
    CST3530_I2C_Write_32(CST3530_END_READ_REG, NULL, 0);

    /* 4. Decodificar coordenadas nativas del Dedo 1 (según Waveshare V2) */
    uint16_t raw_x = (uint16_t)(((buf[7] & 0x0F) << 8) + buf[4]);
    uint16_t raw_y = (uint16_t)(((buf[7] & 0xF0) << 4) + buf[5]);

    touch_data->pressed = 1;
    touch_data->points = touch_cnt;

    /* 5. Mapeo seguro al display horizontal 320x240 */
    uint16_t screen_x = (raw_y < 320) ? raw_y : 319;
    uint16_t screen_y = (raw_x < 240) ? (239 - raw_x) : 0;

    touch_data->coords[0].x = screen_x;
    touch_data->coords[0].y = screen_y;
    touch_data->coords[0].strength = buf[6];

    return 1;
}

/* Función de Desbloqueo Físico del Bus I2C1 (9 impulsos de reloj en SCL: PB8 y SDA: PB9) */
static void CST3530_I2C_ClearBus(void)
{
    GPIO_InitTypeDef GPIO_InitStruct = {0};

    /* 1. Deshabilitar I2C1 */
    __HAL_RCC_I2C1_CLK_DISABLE();

    /* 2. Configurar PB8 (SCL) y PB9 (SDA) como GPIO Output Open-Drain con Pull-Up */
    GPIO_InitStruct.Pin = GPIO_PIN_8 | GPIO_PIN_9;
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_OD;
    GPIO_InitStruct.Pull = GPIO_PULLUP;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
    HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

    /* 3. Generar 9 pulsos en SCL (PB8) para forzar al esclavo a soltar SDA (PB9) */
    for (int i = 0; i < 9; i++) {
        HAL_GPIO_WritePin(GPIOB, GPIO_PIN_8, GPIO_PIN_RESET);
        for (volatile int d = 0; d < 100; d++);
        HAL_GPIO_WritePin(GPIOB, GPIO_PIN_8, GPIO_PIN_SET);
        for (volatile int d = 0; d < 100; d++);
    }

    /* 4. Condición STOP manual */
    HAL_GPIO_WritePin(GPIOB, GPIO_PIN_9, GPIO_PIN_RESET);
    for (volatile int d = 0; d < 100; d++);
    HAL_GPIO_WritePin(GPIOB, GPIO_PIN_8, GPIO_PIN_SET);
    for (volatile int d = 0; d < 100; d++);
    HAL_GPIO_WritePin(GPIOB, GPIO_PIN_9, GPIO_PIN_SET);
    for (volatile int d = 0; d < 100; d++);

    /* 5. Re-habilitar I2C1 e inicializar */
    __HAL_RCC_I2C1_CLK_ENABLE();
    if (cst_cfg.hi2c != NULL) {
        cst_cfg.hi2c->State = HAL_I2C_STATE_RESET;
        HAL_I2C_Init(cst_cfg.hi2c);
    }
}