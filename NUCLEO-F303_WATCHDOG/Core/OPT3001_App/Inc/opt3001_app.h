#ifndef INC_OPT3001_APP_H_
#define INC_OPT3001_APP_H_

#include "main.h"

/* Inicialización del sensor y límites */
void OPT3001_App_Init(void);

/* Tarea periódica de lectura del sensor */
void OPT3001_App_Task(void);

#endif