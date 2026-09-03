#include "button_driver.h"

void Button_Init(Button_HandleTypeDef *hbutton, GPIO_TypeDef *gpioPort, uint16_t gpioPin)
{
    if (hbutton == NULL)
    {
        return;
    }
    hbutton->gpioPort = gpioPort;
    hbutton->gpioPin = gpioPin;
    hbutton->pressStartTime = 0;
    hbutton->wasPressed = 0;
    hbutton->longPressTriggered = 0;
}

Button_Event_TypeDef Button_Process(Button_HandleTypeDef *hbutton)
{
    if (hbutton == NULL || hbutton->gpioPort == NULL)
    {
        return BUTTON_EVENT_NONE;
    }

    // Los botones en placas Nucleo son activos en bajo (Pull-Up interno/externo)
    _Bool buttonIsPressed = (HAL_GPIO_ReadPin(hbutton->gpioPort, hbutton->gpioPin) == GPIO_PIN_RESET);
    uint32_t currentTick = HAL_GetTick();
    Button_Event_TypeDef event = BUTTON_EVENT_NONE;

    if (buttonIsPressed && !hbutton->wasPressed)
    {
        // Flanco de bajada: se inicia la pulsación
        hbutton->pressStartTime = currentTick;
        hbutton->wasPressed = 1;
        hbutton->longPressTriggered = 0;
    }
    else if (!buttonIsPressed && hbutton->wasPressed)
    {
        // Flanco de subida: se libera el botón
        uint32_t pressDuration = currentTick - hbutton->pressStartTime;
        hbutton->wasPressed = 0;

        // Si no se llegó a disparar la pulsación larga "en caliente", evaluar la corta al soltar
        if (!hbutton->longPressTriggered)
        {
            if (pressDuration >= 50 && pressDuration < 1500)
            {
                event = BUTTON_EVENT_SHORT_PRESS;
            }
        }
    }
    else if (buttonIsPressed && hbutton->wasPressed)
    {
        // Mantenido presionado: Evaluar pulsación larga "en caliente" (>= 2 segundos)
        if (!hbutton->longPressTriggered)
        {
            uint32_t pressDuration = currentTick - hbutton->pressStartTime;
            if (pressDuration >= 2000)
            {
                event = BUTTON_EVENT_LONG_PRESS;
                hbutton->longPressTriggered = 1; // Evita disparar múltiples eventos en el mismo ciclo
            }
        }
    }

    return event;
}
