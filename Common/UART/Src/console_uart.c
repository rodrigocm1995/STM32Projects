#include "console_uart.h"
#include <stdio.h>
#include <stdarg.h>
#include <string.h>

#define CONSOLE_TX_BUF_SIZE   512    // Tamaño del buffer circular

static UART_HandleTypeDef *h_uart = NULL;

// Variables para el Buffer Circular de DMA
static uint8_t ring_buffer[CONSOLE_TX_BUF_SIZE];
static volatile uint16_t head = 0; // Índice de escritura
static volatile uint16_t tail = 0; // Índice de lectura
static volatile _Bool dma_busy = 0;

// Buffer temporal de formateo
static char temp_buffer[128]; 

void Console_Init(UART_HandleTypeDef *huart)
{
    h_uart = huart;
    dma_busy = 0;
    head = 0;
    tail = 0;
}

/* =======================================================================================
   MODO NORMAL (BLOQUEANTE - POLLING)
   ======================================================================================= */
void Console_Printf(const char *format, ...)
{
    if (h_uart == NULL) return;

    va_list args;
    va_start(args, format);
    
    // Formatear el string en el buffer temporal
    int len = vsnprintf(temp_buffer, sizeof(temp_buffer), format, args);
    va_end(args);

    if (len > 0)
    {
        // Enviar por UART en modo normal (bloqueante)
        HAL_UART_Transmit(h_uart, (uint8_t *)temp_buffer, len, HAL_MAX_DELAY);
    }
}

/* =======================================================================================
   MODO DMA (NO BLOQUEANTE - BUFFER CIRCULAR)
   ======================================================================================= */
void Console_Printf_DMA(const char *format, ...)
{
    if (h_uart == NULL) return;

    va_list args;
    va_start(args, format);
    int len = vsnprintf(temp_buffer, sizeof(temp_buffer), format, args);
    va_end(args);

    if (len <= 0) return;

    // 1. Guardar el mensaje en el buffer circular
    for (int i = 0; i < len; i++)
    {
        uint16_t next_head = (head + 1) % CONSOLE_TX_BUF_SIZE;
        
        // Si el buffer circular se llena, descartamos el resto del mensaje para evitar desbordes
        if (next_head == tail) 
        {
            break; 
        }
        
        ring_buffer[head] = temp_buffer[i];
        head = next_head;
    }

    // 2. Si el DMA no está transmitiendo, iniciar la transmisión del bloque
    if (!dma_busy)
    {
        dma_busy = 1;
        
        uint16_t current_tail = tail;
        uint16_t send_len = 0;
        
        // Calcular cuántos bytes continuos hay listos para enviar
        if (head >= current_tail)
        {
            send_len = head - current_tail;
        }
        else
        {
            send_len = CONSOLE_TX_BUF_SIZE - current_tail; // Enviar hasta el límite del buffer
        }

        if (send_len > 0)
        {
            HAL_UART_Transmit_DMA(h_uart, &ring_buffer[current_tail], send_len);
        }
        else
        {
            dma_busy = 0;
        }
    }
}

// Callback invocado cuando la transmisión del DMA finaliza
void Console_TxCpltCallback(UART_HandleTypeDef *huart)
{
    if (huart->Instance == h_uart->Instance)
    {
        // 1. Actualizar el puntero 'tail' según el tamaño del bloque transmitido
        // En este ejemplo simple, asumimos que se transmitió hasta 'head' o el límite.
        // Calculamos cuántos bytes quedan pendientes.
        uint16_t current_tail = tail;
        uint16_t sent_len = 0;
        
        if (head >= current_tail)
        {
            sent_len = head - current_tail;
            tail = head; // Todo enviado
        }
        else
        {
            sent_len = CONSOLE_TX_BUF_SIZE - current_tail;
            tail = 0; // Dio la vuelta
        }

        // 2. Si quedan más datos encolados en el buffer circular, iniciar otra transferencia DMA
        if (tail != head)
        {
            uint16_t next_tail = tail;
            uint16_t send_len = 0;
            
            if (head >= next_tail)
            {
                send_len = head - next_tail;
            }
            else
            {
                send_len = CONSOLE_TX_BUF_SIZE - next_tail;
            }
            
            HAL_UART_Transmit_DMA(h_uart, &ring_buffer[next_tail], send_len);
        }
        else
        {
            // El buffer circular está vacío, liberamos el estado de ocupado
            dma_busy = 0;
        }
    }
}