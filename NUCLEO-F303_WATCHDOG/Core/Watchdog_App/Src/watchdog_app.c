#include "watchdog_app.h"
#include "console_uart.h"
#include "stm32f3xx_hal_iwdg.h"

/* Variables externas del sistema */
extern IWDG_HandleTypeDef hiwdg;

/* Array del estado de salud para cada tarea del sistema */
static volatile uint8_t task_status[TASK_COUNT] = {0};

void Watchdog_Init(void)
{
    // Inicializar el estado de salud de todas las tareas a 0 (no reportado)
    for (uint8_t i = 0; i < TASK_COUNT; i++)
    {
        task_status[i] = 0;
    }
}

void Watchdog_Report_Alive(SystemTask_ID_TypeDef taskId)
{
    if (taskId < TASK_COUNT)
    {
        task_status[taskId] = 1; // Firma guardada para la tarea específica
    }
}

void Watchdog_Monitor_And_Feed(void)
{
    uint8_t all_tasks_healthy = 1;
    
    // Evaluamos dinámicamente si TODAS las tareas firmaron exitosamente
    for (uint8_t i = 0; i < TASK_COUNT; i++)
    {
        if (task_status[i] == 0)
        {
            all_tasks_healthy = 0; // Una tarea no reportó vida o se trabó
            break;
        }
    }
    
    if (all_tasks_healthy)
    {
        // Limpiamos las firmas para el siguiente periodo de monitoreo
        for (uint8_t i = 0; i < TASK_COUNT; i++)
        {
            task_status[i] = 0;
        }
        
        // Alimentamos físicamente al periférico de hardware
        HAL_IWDG_Refresh(&hiwdg);
    }
    // Si alguna tarea falló, no alimentamos al IWDG y dejamos que la placa se reinicie.
}

void Watchdog_Check_Reset_Reason(void)
{
    if (__HAL_RCC_GET_FLAG(RCC_FLAG_IWDGRST) != RESET)
    {
        Console_Printf("ALERTA: El último reinicio fue por Watchdog (IWDG Reset).\r\n");
        __HAL_RCC_CLEAR_RESET_FLAGS(); // Obligatorio limpiar las banderas
    }
    else
    {
        Console_Printf("Arranque normal del sistema.\r\n");
    }
}