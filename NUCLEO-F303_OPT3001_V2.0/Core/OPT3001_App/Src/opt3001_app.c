#include "opt3001_app.h"
#include "OPT3001.h"
#include "console_uart.h"
#include "stm32f3xx_hal_i2c.h"

/* Variables externas definidas en main.c */
extern I2C_HandleTypeDef hi2c1;
extern volatile uint8_t counterTick;

/* Variables privadas de la aplicación de luz */
static OPT3001_HandleTypeDef hopt3001;
static float lowLimit = 50000.0f;
static float highLimit = 80000.0f;
static uint8_t counterOverflow = 8;    // Valor por defecto seguro (800 ms)

void OPT3001_App_Init(void)
{
    if (OPT3001_Init(&hopt3001, &hi2c1, OPT3001_ADDRESS) != HAL_OK)
    {
        Console_Printf("Error. Could not communicate with the OPT3001 via I2C.\r\n");
        Error_Handler();
    }
    Console_Printf("OPT3001 detected and successfully configured.\r\n");
    if (OPT3001_SetLowLimit(&hopt3001, lowLimit) != HAL_OK)
    {
        Console_Printf("Error. Could not set the low limit lux.\r\n");
        Error_Handler();
    }
    Console_Printf("OPT3001 set the low limit lux to %.2f successfully.\r\n", lowLimit);
    if (OPT3001_SetHighLimit(&hopt3001, highLimit) != HAL_OK)
    {
        Console_Printf("Error. Could not set the high limit lux.\r\n");
        Error_Handler();
    }
    Console_Printf("OPT3001 set the high limit lux to %.2f successfully.\r\n", highLimit);
    if (OPT3001_SetConvTime(&hopt3001, OPT3001_800_MS) != HAL_OK)
    {
        Console_Printf("Error. Could not set the conversion time.\r\n");
        Error_Handler();
    }
    Console_Printf("The conversion time was successfully established.\r\n");
    OPT3001_ConvTime_TypeDef currentConvTime;
    if (OPT3001_GetConvTime(&hopt3001, &currentConvTime) == HAL_OK)
    {
        if (currentConvTime == OPT3001_100_MS)
        {
            counterOverflow = 1; // 100 ms
            Console_Printf("Tiempo de integración activo: 100 ms\r\n");
        }
        else if (currentConvTime == OPT3001_800_MS)
        {
            counterOverflow = 8; // 800 ms
            Console_Printf("Tiempo de integración activo: 800 ms\r\n");
        }
    }
}

void OPT3001_App_Task(void)
{
    if (counterTick >= counterOverflow)
    {
        counterTick = 0;
        if (OPT3001_IsConversionReady(&hopt3001))
        {
            float lux = 0.0f;
            if (OPT3001_GetLux(&hopt3001, &lux) == HAL_OK)
            {
                Console_Printf("Lux = %.2f\r\n", lux);
            }
            else 
            {
                Console_Printf("ERROR: Error al realizar la lectura I2C.\r\n");
            }
        }
    }
}