/**
  ******************************************************************************
  * @file           : button_driver.c
  * @brief          : Implementation for push button debounce and event detection.
  ******************************************************************************
  */

#include "button_driver.h"
#include "stm32c0xx_hal.h"
#include "stm32c0xx_hal_gpio.h"
#include <stdint.h>

/**
  * @brief  Initializes a button handle instance with target GPIO port and pin.
  * @param  hbutton:  Pointer to button handle structure.
  * @param  gpioPort: GPIO port connected to the button (e.g. GPIOA).
  * @param  gpioPin:  GPIO pin connected to the button (e.g. GPIO_PIN_0).
  * @retval None
  */
void Button_Init(Button_HandleTypeDef *hbutton, GPIO_TypeDef *gpioPort, uint16_t gpioPin)
{
    if (hbutton == NULL) return;

    hbutton->_gpioPort = gpioPort;
    hbutton->_gpioPin = gpioPin;
    hbutton->_pressStartTime = 0;
    hbutton->_wasPressed = 0;
    hbutton->_longPressTriggered = 0;
}

/**
  * @brief  Polls and evaluates button state transitions to detect short/long presses.
  * @param  hbutton: Pointer to button handle structure.
  * @retval Button_Event_TypeDef: Detected event (NONE, SHORT_PRESS, or LONG_PRESS).
  */
Button_Event_TypeDef Button_Process(Button_HandleTypeDef *hbutton)
{
    if (hbutton == NULL || hbutton->_gpioPort == NULL)
    {
        return BUTTON_EVENT_NONE;
    }

    _Bool buttonIsPressed = (HAL_GPIO_ReadPin(hbutton->_gpioPort, hbutton->_gpioPin) == GPIO_PIN_RESET);
    uint32_t currentTick = HAL_GetTick();
    Button_Event_TypeDef event = BUTTON_EVENT_NONE;

    if (buttonIsPressed && !hbutton->_wasPressed)
    {
        // Falling edge: button press initiated
        hbutton->_pressStartTime = currentTick;
        hbutton->_wasPressed = 1;
        hbutton->_longPressTriggered = 0;
    }
    else if (!buttonIsPressed && hbutton->_wasPressed)
    {
        // Rising edge: button released
        uint32_t pressDuration = currentTick - hbutton->_pressStartTime;
        hbutton->_wasPressed = 0;

        // If long press was not already triggered, evaluate short press upon release
        if (!hbutton->_longPressTriggered)
        {
            if (pressDuration >= 50U && pressDuration < 1500U)
            {
                event = BUTTON_EVENT_SHORT_PRESS;
            }
        }
    }
    else if (buttonIsPressed && hbutton->_wasPressed)
    {
        // Held pressed: evaluate long press threshold (>= 2000 ms)
        if (!hbutton->_longPressTriggered)
        {
            uint32_t pressDuration = currentTick - hbutton->_pressStartTime;
            if (pressDuration >= 2000U)
            {
                event = BUTTON_EVENT_LONG_PRESS;
                hbutton->_longPressTriggered = 1; // Prevent repeated triggers in same press cycle
            }
        }
    }

    return event;
}