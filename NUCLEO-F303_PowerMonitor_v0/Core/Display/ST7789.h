/**
  *******************************************************************************************
  * @file           : ST7789.h
  * @brief          : Encabezado del Driver de Pantalla TFT ST7789 (SPI + DMA + PWM)
  *******************************************************************************************
  */

#ifndef DISPLAY_ST7789_H_
#define DISPLAY_ST7789_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "main.h"
#include <stdint.h>

/* Definiciones de resolución de la pantalla (Landscape 320x240) */
#define ST7789_WIDTH   320
#define ST7789_HEIGHT  240

/* Comandos Principales del ST7789 */
#define ST7789_NOP     0x00
#define ST7789_SWRESET 0x01
#define ST7789_SLPIN   0x10
#define ST7789_SLPOUT  0x11
#define ST7789_NORON   0x13
#define ST7789_INVOFF  0x20
#define ST7789_INVON   0x21
#define ST7789_DISPOFF 0x28
#define ST7789_DISPON  0x29
#define ST7789_CASET   0x2A
#define ST7789_RASET   0x2B
#define ST7789_RAMWR   0x2C
#define ST7789_MADCTL  0x36
#define ST7789_COLMOD  0x3A

/* Bits de control para orientación de pantalla (MADCTL) */
#define ST7789_MADCTL_MY  0x80
#define ST7789_MADCTL_MX  0x40
#define ST7789_MADCTL_MV  0x20
#define ST7789_MADCTL_RGB 0x00

/* Estructura de configuración dinámica del ST7789 */
typedef struct
{
    SPI_HandleTypeDef   *hspi;
    GPIO_TypeDef        *CS_Port;
    uint16_t            CS_Pin;
    GPIO_TypeDef        *DC_Port;
    uint16_t            DC_Pin;
    GPIO_TypeDef        *RST_Port;
    uint16_t            RST_Pin;    
    TIM_HandleTypeDef   *htim_bl;   /* Manejador del Timer PWM para el backlight */
    uint32_t            Channel_bl; /* Canal del Timer (ej. TIM_CHANNEL_2) */
} ST7789_Config_t;

/* Alias de compatibilidad por si se utiliza la nomenclatura previa */
typedef ST7789_Config_t ST7789_Config_HandleTypeDef;

/* Prototipos de Funciones Públicas */
void ST7789_Init(const ST7789_Config_t *config);
void ST7789_Reset(void);
void ST7789_WriteCommand(uint8_t cmd);
void ST7789_WriteData(uint8_t data);
void ST7789_SetAddressWindow(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2);
void ST7789_WriteBufferDMA(uint8_t *data, uint32_t size);
void ST7789_SetBacklight(uint8_t duty);
void ST7789_TestFill(uint16_t color);

#ifdef __cplusplus
}
#endif

#endif /* DISPLAY_ST7789_H_ */