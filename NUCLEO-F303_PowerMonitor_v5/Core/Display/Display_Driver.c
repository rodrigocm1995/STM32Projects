/**
  *******************************************************************************************
  * @file           : Display_Driver.c
  * @brief          : Implementación del Integrador de LVGL v9.5 (100% Dinámico)
  *******************************************************************************************
  */

#include "Display_Driver.h"
#include "CST328.h"

static lv_display_t *disp;
static lv_indev_t *indev_touch;
static ST7789_Config_HandleTypeDef lcd_cfg; /* Copia local para los callbacks */

/* Buffer de renderizado en RAM para LVGL */
#define DRAW_BUF_SIZE (ST7789_WIDTH * 8 * 2)
static uint8_t buf1[DRAW_BUF_SIZE];

static void Display_Flush(lv_display_t *display, const lv_area_t *area, uint8_t *px_map) 
{
    uint32_t width = (area->x2 - area->x1 + 1);
    uint32_t height = (area->y2 - area->y1 + 1);
    uint32_t size = width * height * 2;
    
    /* 1. ESPERA DE SEGURIDAD: Esperar a que la transferencia DMA previa finalice */
    uint32_t timeout = 100000;
    while (lcd_cfg.hspi->State != HAL_SPI_STATE_READY && --timeout > 0);

    /* Corrige el Endianness para RGB565 */
    lv_draw_sw_rgb565_swap(px_map, width * height);
    
    /* Configurar la ventana de direcciones en la pantalla */
    ST7789_SetAddressWindow(area->x1, area->y1, area->x2, area->y2);
    
    /* Limpiar bandera de Overrun previa por seguridad */
    __HAL_SPI_CLEAR_OVRFLAG(lcd_cfg.hspi);

    /* 
     * PROTECCIÓN CONTRA CONGELAMIENTO DE DMA:
     * Si la transferencia por DMA falla o el canal está ocupado (HAL_BUSY),
     * liberamos el pin CS y notificamos a LVGL para evitar que quede esperando eternamente.
     */
    if (HAL_SPI_Transmit_DMA(lcd_cfg.hspi, px_map, size) != HAL_OK) 
    {
        HAL_GPIO_WritePin(lcd_cfg.CS_Port, lcd_cfg.CS_Pin, GPIO_PIN_SET);
        lv_display_flush_ready(display);
    }
}

/* Callback del DMA de SPI: Transferencia Completada */
void HAL_SPI_TxCpltCallback(SPI_HandleTypeDef *hspi) 
{
    if (hspi == lcd_cfg.hspi) {
        HAL_GPIO_WritePin(lcd_cfg.CS_Port, lcd_cfg.CS_Pin, GPIO_PIN_SET);
        
        /* ¡CRÍTICO! Liberar manualmente el estado del HAL antes de llamar a LVGL */
        hspi->State = HAL_SPI_STATE_READY;
        
        lv_display_flush_ready(disp);
    }
}

/* Callback del DMA de SPI: Error en Transferencia (¡NUEVO: EVITA EL CONGELAMIENTO!) */
void HAL_SPI_ErrorCallback(SPI_HandleTypeDef *hspi)
{
    if (hspi == lcd_cfg.hspi) {
        HAL_GPIO_WritePin(lcd_cfg.CS_Port, lcd_cfg.CS_Pin, GPIO_PIN_SET);
        
        /* ¡CRÍTICO! Liberar manualmente el estado del HAL */
        hspi->State = HAL_SPI_STATE_READY;
        
        lv_display_flush_ready(disp);
    }
}
static void Touch_Read(lv_indev_t *indev, lv_indev_data_t *data) 
{
    CST328_TouchData_HandleTypeDef touch_data;
    if (CST328_ReadTouch(&touch_data) == 1 && touch_data.pressed) {
        data->point.x = touch_data.coords[0].x;
        data->point.y = touch_data.coords[0].y;
        data->state = LV_INDEV_STATE_PRESSED;
    } else {
        data->state = LV_INDEV_STATE_RELEASED;
    }
}

void Display_Init(const Display_Config_HandleTypeDef *config) 
{
    if (config == NULL) return;
    lcd_cfg = config->lcd;
    ST7789_Init(&lcd_cfg);
    CST328_Init(&config->touch);
    
    /* 1. Crear pantalla en LVGL v9.5 */
    disp = lv_display_create(ST7789_WIDTH, ST7789_HEIGHT);
    
    /* 2. Asignar buffer de pintado y callback de flush */
    lv_display_set_buffers(disp, buf1, NULL, sizeof(buf1), LV_DISPLAY_RENDER_MODE_PARTIAL);
    lv_display_set_flush_cb(disp, Display_Flush);
    
    /* 3. Registrar panel táctil en LVGL */
    indev_touch = lv_indev_create();
    lv_indev_set_type(indev_touch, LV_INDEV_TYPE_POINTER);
    lv_indev_set_read_cb(indev_touch, Touch_Read);
}

void Display_SetBrightness(uint8_t brightness_percent) 
{
    ST7789_SetBacklight(brightness_percent);
}