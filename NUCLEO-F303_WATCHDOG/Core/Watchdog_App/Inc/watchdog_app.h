#ifndef INC_WATCHDOG_APP_H_
#define INC_WATCHDOG_APP_H_

#include "main.h"

/* Enum para registrar e identificar todas las tareas activas del sistema */
typedef enum
{
    TASK_OPT3001 = 0,
    TASK_TMP117,
    TASK_COUNT
} SystemTask_ID_TypeDef;

/* Se Inicializa las variables de control de software */
void Watchdog_Init(void);

/* Se revisa el origen del último reset y limpia las banderas */
void Watchdog_Check_Reset_Reason(void);

/* Las tareas individuales se comunican periodicamente para dcir "estoy bien" */
void Watchdog_Report_Alive(SystemTask_ID_TypeDef taskId);

/* Tarea de supervisión central que revisa las firmas y alimenta al hardware */
void Watchdog_Monitor_And_Feed(void);

#endif