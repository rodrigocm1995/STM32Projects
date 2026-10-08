/**
  *******************************************************************************************
  * @file           : ST7789.c
  * @brief          : Implementación Dinámica del Driver ST7789 con PWM genérico
  *******************************************************************************************
  */

#include "ST7789.h"

static ST7789_Config_t lcd_cfg;

/* Funciones inline privadas de control GPIO dinámico */
static inline void LCD_CS_LOW(void) 
{
    HAL_GPIO_WritePin(lcd_cfg.CS_Port, lcd_cfg.CS_Pin, GPIO_PIN_RESET);
}

static inline void LCD_CS_HIGH(void) 
{
    HAL_GPIO_WritePin(lcd_cfg.CS_Port, lcd_cfg.CS_Pin, GPIO_PIN_SET);
}

static inline void LCD_DC_LOW(void) 
{
    HAL_GPIO_WritePin(lcd_cfg.DC_Port, lcd_cfg.DC_Pin, GPIO_PIN_RESET);
}

static inline void LCD_DC_HIGH(void) 
{
    HAL_GPIO_WritePin(lcd_cfg.DC_Port, lcd_cfg.DC_Pin, GPIO_PIN_SET);
}

static inline void LCD_RST_LOW(void) 
{
    HAL_GPIO_WritePin(lcd_cfg.RST_Port, lcd_cfg.RST_Pin, GPIO_PIN_RESET);
}

static inline void LCD_RST_HIGH(void) 
{
    HAL_GPIO_WritePin(lcd_cfg.RST_Port, lcd_cfg.RST_Pin, GPIO_PIN_SET);
}

void ST7789_WriteCommand(uint8_t cmd) 
{
    LCD_DC_LOW();
    LCD_CS_LOW();
    HAL_SPI_Transmit(lcd_cfg.hspi, &cmd, 1, HAL_MAX_DELAY);
    LCD_CS_HIGH();
}

void ST7789_WriteData(uint8_t data) 
{
    LCD_DC_HIGH();
    LCD_CS_LOW();
    HAL_SPI_Transmit(lcd_cfg.hspi, &data, 1, HAL_MAX_DELAY);
    LCD_CS_HIGH();
}

void ST7789_Reset(void) 
{
    LCD_CS_HIGH();      /* CS debe estar inactivo durante el reset */
    HAL_Delay(10);
    LCD_RST_LOW();      /* Activar reset */
    HAL_Delay(20);
    LCD_RST_HIGH();     /* Liberar reset */
    HAL_Delay(120);     /* El oscilador interno del ST7789 tarda 120ms en estabilizarse */
}

void ST7789_SetAddressWindow(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2) 
{
    ST7789_WriteCommand(ST7789_CASET);
    ST7789_WriteData(x1 >> 8);
    ST7789_WriteData(x1 & 0xFF);
    ST7789_WriteData(x2 >> 8);
    ST7789_WriteData(x2 & 0xFF);
    ST7789_WriteCommand(ST7789_RASET);
    ST7789_WriteData(y1 >> 8);
    ST7789_WriteData(y1 & 0xFF);
    ST7789_WriteData(y2 >> 8);
    ST7789_WriteData(y2 & 0xFF);
    /* Enviar comando RAMWR SIN soltar el pin CS */
    LCD_DC_LOW();
    LCD_CS_LOW();
    uint8_t cmd = ST7789_RAMWR;
    HAL_SPI_Transmit(lcd_cfg.hspi, &cmd, 1, HAL_MAX_DELAY);
    /* Dejamos DC en HIGH y CS en LOW listos para recibir los píxeles */
    LCD_DC_HIGH();
}

void ST7789_WriteBufferDMA(uint8_t *data, uint32_t size) 
{
    /* CS y DC ya fueron posicionados por ST7789_SetAddressWindow */
    HAL_SPI_Transmit_DMA(lcd_cfg.hspi, data, size);
}

void ST7789_SetBacklight(uint8_t duty) 
{
    if (lcd_cfg.htim_bl == NULL) return; /* Si no hay timer configurado, ignorar */

    if (duty > 100) duty = 100;

    /* Leer dinámicamente el valor máximo del contador (ARR) del Timer asignado */
    uint32_t period = __HAL_TIM_GET_AUTORELOAD(lcd_cfg.htim_bl);
    uint32_t pulse = (duty * period) / 100;

    /* Asignar el pulso al canal asignado */
    __HAL_TIM_SET_COMPARE(lcd_cfg.htim_bl, lcd_cfg.Channel_bl, pulse);
}

void ST7789_Init(const ST7789_Config_t *config) 
{
    if (config == NULL) return;
    lcd_cfg = *config;
    /* Iniciar PWM de Backlight */
    if (lcd_cfg.htim_bl != NULL) 
    {
        HAL_TIM_PWM_Start(lcd_cfg.htim_bl, lcd_cfg.Channel_bl);
        ST7789_SetBacklight(100);
    }
    ST7789_Reset();
    /* 1. Salir de Sleep Mode PRIMERO */
    ST7789_WriteCommand(ST7789_SLPOUT);
    HAL_Delay(120);     /* Esperar a que despierte el circuito de potencia */
    /* 2. Configurar formato de color: 16-bit RGB565 */
    ST7789_WriteCommand(ST7789_COLMOD);
    ST7789_WriteData(0x55);
    /* 3. Configurar orientación apaisada */
    ST7789_WriteCommand(ST7789_MADCTL);
    ST7789_WriteData(ST7789_MADCTL_MV | ST7789_MADCTL_MX);
    /* 4. Modo normal e inversión */
    ST7789_WriteCommand(ST7789_INVON);
    HAL_Delay(10);
    ST7789_WriteCommand(ST7789_NORON);
    HAL_Delay(10);
    /* 5. Encender el panel */
    ST7789_WriteCommand(ST7789_DISPON);
    HAL_Delay(20);
}

void ST7789_TestFill(uint16_t color)
{
    ST7789_SetAddressWindow(0, 0, ST7789_WIDTH - 1, ST7789_HEIGHT - 1);
    /* DC ya está en HIGH y CS ya está en LOW gracias a SetAddressWindow */
    uint8_t data[2] = { (uint8_t)(color >> 8), (uint8_t)(color & 0xFF) };
    for (uint32_t i = 0; i < (ST7789_WIDTH * ST7789_HEIGHT); i++)
    {
        HAL_SPI_Transmit(lcd_cfg.hspi, data, 2, HAL_MAX_DELAY);
    }
    /* Soltamos CS únicamente al terminar de pintar toda la pantalla */
    LCD_CS_HIGH();
}