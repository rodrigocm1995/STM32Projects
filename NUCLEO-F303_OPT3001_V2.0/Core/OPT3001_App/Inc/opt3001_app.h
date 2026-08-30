#ifndef INC_OPT3001_APP_H_H
#define INC_OPT3001_APP_H_H

#include "main.h"

/* Inicialización del sensor y límites */
void OPT3001_App_Init(I2C_HandleTypeDef *hi2c, volatile uint8_t *ptick);

/* Tarea periódica de lectura del sensor */
void OPT3001_App_Task(void);

#endif