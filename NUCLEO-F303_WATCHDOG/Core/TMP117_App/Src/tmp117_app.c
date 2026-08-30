#include "tmp117_app.h"
#include "TMP117.h"
#include "console_uart.h"
#include "main.h"
#include "watchdog_app.h"


/* Variables privadas */
static TMP117_HandleTypeDef htmp117;
static float highLimitTemp = 55.0f;
static float lowLimitTemp = 35.0f;
static volatile uint8_t *pTmp117CounterTick = NULL;

void TMP117_App_Init(I2C_HandleTypeDef *hi2c, volatile uint8_t *pTemptick)
{
    if (hi2c == NULL || pTemptick == NULL)
    {
        return;
    }

    pTmp117CounterTick = pTemptick;

    if (TMP117_Init(&htmp117, hi2c, TMP117_ADDRESS) != HAL_OK)
    {
        Console_Printf("TMP117 Error. Could not communicate with the TMP117 via I2C.\r\n");
        Error_Handler();
    }
    Console_Printf("TMP117 detected and successfully configured.\r\n");

    if (TMP117_SetLowLimit_C(&htmp117, lowLimitTemp) != HAL_OK)
    {
        Console_Printf("Error. Could not set the low limit temperature.\r\n");
        Error_Handler();
    }
    Console_Printf("TMP117 - The low limit temp was successfully established.\r\n");

    if (TMP117_SetHighLimit_C(&htmp117, highLimitTemp) != HAL_OK)
    {
        Console_Printf("Error. Could not set the high limit temperature.\r\n");
        Error_Handler();
    }
    Console_Printf("TMP117 - The high limit temp was successfully established.\r\n");

    if (TMP117_SetConvTime(&htmp117, TMP117_CONV_500_MS) != HAL_OK)
    {
        Console_Printf("TMP117 Error. Requested time (%.1f ms) is less than active time (%.1f ms)\r\n", htmp117._requestedTime, htmp117._activeTime);
        Error_Handler();
    }
    Console_Printf("TMP117 - The conversion time was successfully stablisehd.\r\n");
}

void TMP117_App_Task(void)
{
    if (*pTmp117CounterTick >= 4)
    {
        *pTmp117CounterTick = 0;
        if (TMP117_IsDataReady(&htmp117))
        {
            float temp = 0.0f;
            if (TMP117_GetTemperature_C(&htmp117, &temp) == HAL_OK)
            {
                Console_Printf("Temperature: %.2f C\r\n", temp);
            }
            else
            {
                Console_Printf("TMP117 ERROR: Error al realizar la lectura I2C.\r\n");
            }
        }
    }
    // Al final del ciclo de la tarea, reportamos que el hilo no está bloqueado
    Watchdog_Report_Alive(TASK_TMP117);
}