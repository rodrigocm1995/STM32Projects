#ifndef INC_TMP117_APP_H_
#define INC_TMP117_APP_H_

#include "main.h"

/* Inicialización del sensor y límites */
void TMP117_App_Init(void);

/* Tarea periódica de lectura del sensor */
void TMP117_App_Task(void);

#endif