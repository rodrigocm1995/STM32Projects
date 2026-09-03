#ifndef BUTTON_DRIVER_H_
#define BUTTON_DRIVER_H_

#include "main.h"
#include "stm32c011xx.h"

/* Tipos de eventos detectables  por el controlador de botones */
typedef enum
{
    BUTTON_EVENT_NONE,
    BUTTON_EVENT_SHORT_PRESS,
    BUTTON_EVENT_LONG_PRESS
} Button_Event_TypeDef;

/* Estructura para instanciar botones individuales */
typedef struct
{
    GPIO_TypeDef *_gpioPort;
    uint16_t     _gpioPin;
    uint32_t     _pressStartTime;
    _Bool        _wasPressed;
    _Bool        _longPressTriggered;
} Button_HandleTypeDef;

/**
  * @brief  Inicializa el manejador de botón asociando su hardware y limpiando estados.
  * @param  hbutton: Puntero al manejador del botón.
  * @param  gpioPort: Puerto GPIO del botón (ej. GPIOC).
  * @param  gpioPin: Pin GPIO del botón (ej. GPIO_PIN_13).
  * @retval None
  */
void Button_Init(Button_HandleTypeDef *hbutton, GPIO_TypeDef *gpioPort, uint16_t gpioPin);

/**
  * @brief  Procesa y evalúa el estado físico del botón para retornar eventos.
  *         Detecta rebotes, pulsaciones cortas al liberar y pulsaciones largas en caliente.
  * @param  hbutton: Puntero al manejador del botón.
  * @retval Button_Event_TypeDef: Evento detectado.
  */
Button_Event_TypeDef Button_Process(Button_HandleTypeDef *hbutton);


#endif /* BUTTON_DRIVER_H_ */