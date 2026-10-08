/**
  *******************************************************************************************
  * @file           : Display_Driver.h
  * @brief          : Encabezado del Integrador de Pantalla y Touch para LVGL v9.5
  *******************************************************************************************
  */

#ifndef DISPLAY_DRIVER_H_
#define DISPLAY_DRIVER_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "ST7789.h"
#include "CST328.h"
#include "lvgl.h"


typedef struct {
    ST7789_Config_HandleTypeDef lcd;
    CST328_Config_HandleTypeDef touch;
} Display_Config_HandleTypeDef;

void Display_Init(const Display_Config_HandleTypeDef *config);
void Display_SetBrightness(uint8_t brightness_percent);

#ifdef __cplusplus
}
#endif

#endif /* DISPLAY_DRIVER_H_ */