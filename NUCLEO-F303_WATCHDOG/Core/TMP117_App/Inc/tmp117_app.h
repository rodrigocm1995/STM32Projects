#ifndef INC_TMP117_APP_H_
#define INC_TMP117_APP_H_

#include "main.h"

/* Inicialización del sensor y límites */
void TMP117_App_Init(I2C_HandleTypeDef *hi2c, volatile uint8_t *pTemptick);

/* Tarea periódica de lectura del sensor */
void TMP117_App_Task(void);

#endif