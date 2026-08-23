#ifndef INC_CONSOLE_UART_H_
#define INC_CONSOLE_UART_H_

#include "main.h"

void Console_Init(UART_HandleTypeDef *huart);

// Modo Normal (Bloqueante)
void Console_Printf(const char *format, ...);

// Modo DMA (No bloqueante)
void Console_Printf_DMA(const char *format, ...);

// Esta función debe ser llamada dentro de HAL_UART_TxCpltCallback en main.c
void Console_TxCpltCallback(UART_HandleTypeDef *huart);

#endif /* INC_CONSOLE_H_ */