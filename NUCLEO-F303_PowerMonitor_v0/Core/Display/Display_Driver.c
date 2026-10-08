/**
  *******************************************************************************************
  * @file           : Display_Driver.c
  * @brief          : Implementación del Integrador de LVGL v9.5 (100% Dinámico)
  *******************************************************************************************
  */

#include "Display_Driver.h"
#include "screens.h"
#include "CST328.h"

static lv_display_t *disp;
static lv_indev_t *indev_touch;
static ST7789_Config_HandleTypeDef lcd_cfg; /* <-- Copia local para el callback */

/* Buffer de renderizado en RAM para LVGL */
#define DRAW_BUF_SIZE (ST7789_WIDTH * 24 * 2)
static uint8_t buf1[DRAW_BUF_SIZE];

static void Display_Flush(lv_display_t *display, const lv_area_t *area, uint8_t *px_map) 
{
    uint32_t width = (area->x2 - area->x1 + 1);
    uint32_t height = (area->y2 - area->y1 + 1);
    uint32_t size = width * height * 2;

    /* Corrige el Endianness: elimina los puntos de colores en el texto */
    lv_draw_sw_rgb565_swap(px_map, width * height);
    ST7789_SetAddressWindow(area->x1, area->y1, area->x2, area->y2);
    ST7789_WriteBufferDMA(px_map, size);
}

/* Callback del DMA de SPI: 100% desacoplado */
void HAL_SPI_TxCpltCallback(SPI_HandleTypeDef *hspi) 
{
    if (hspi == lcd_cfg.hspi) {
        HAL_GPIO_WritePin(lcd_cfg.CS_Port, lcd_cfg.CS_Pin, GPIO_PIN_SET); // <-- Usa la estructura
        lv_display_flush_ready(disp);
    }
}

static void Touch_Read(lv_indev_t *indev, lv_indev_data_t *data) 
{
    CST328_TouchData_HandleTypeDef touch_data;
    if (CST328_ReadTouch(&touch_data) == 1 && touch_data.pressed) {
        HAL_GPIO_WritePin(GPIOA, GPIO_PIN_5, GPIO_PIN_SET);   /* LED ON */
        data->point.x = touch_data.coords[0].x;
        data->point.y = touch_data.coords[0].y;
        data->state = LV_INDEV_STATE_PRESSED;
        /* Telemetría en vivo: muestra las coordenadas recibidas en el botón */
        if (objects.obj1 != NULL) {
            lv_label_set_text_fmt(objects.obj1, "X: %d | Y: %d", data->point.x, data->point.y);
        }
    } else {
        HAL_GPIO_WritePin(GPIOA, GPIO_PIN_5, GPIO_PIN_RESET); /* LED OFF */
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