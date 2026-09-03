/**
  ******************************************************************************
  * @file           : button_driver.h
  * @brief          : Header for generic GPIO push button debounce and event driver.
  ******************************************************************************
  */

#ifndef BUTTON_DRIVER_H_
#define BUTTON_DRIVER_H_

#include "main.h"
#include "stm32c011xx.h"

/**
  * @brief Event types detectable by the push button driver.
  */
typedef enum
{
    BUTTON_EVENT_NONE,        /**< No event detected */
    BUTTON_EVENT_SHORT_PRESS, /**< Short press detected upon release (50ms - 1500ms) */
    BUTTON_EVENT_LONG_PRESS   /**< Long press triggered immediately upon reaching 2000ms */
} Button_Event_TypeDef;

/**
  * @brief Handle structure for an individual push button instance.
  */
typedef struct
{
    GPIO_TypeDef *_gpioPort;          /**< GPIO port associated with the button (e.g. GPIOA) */
    uint16_t     _gpioPin;           /**< GPIO pin associated with the button (e.g. GPIO_PIN_0) */
    uint32_t     _pressStartTime;     /**< System tick timestamp when button press started */
    _Bool        _wasPressed;         /**< Previous physical press state flag */
    _Bool        _longPressTriggered; /**< Flag indicating whether long press has already fired */
} Button_HandleTypeDef;

/**
  * @brief  Initializes a button instance, binding its GPIO hardware and clearing internal states.
  * @param  hbutton:  Pointer to button handle structure.
  * @param  gpioPort: GPIO port connected to the button (e.g. GPIOA).
  * @param  gpioPin:  GPIO pin connected to the button (e.g. GPIO_PIN_0).
  * @retval None
  */
void Button_Init(Button_HandleTypeDef *hbutton, GPIO_TypeDef *gpioPort, uint16_t gpioPin);

/**
  * @brief  Processes the button physical state, handling debounce, short press and long press.
  * @param  hbutton: Pointer to button handle structure.
  * @retval Button_Event_TypeDef: Detected button event (NONE, SHORT_PRESS, or LONG_PRESS).
  */
Button_Event_TypeDef Button_Process(Button_HandleTypeDef *hbutton);

#endif /* BUTTON_DRIVER_H_ */