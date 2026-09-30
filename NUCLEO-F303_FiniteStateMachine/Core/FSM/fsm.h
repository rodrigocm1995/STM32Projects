#ifndef FSM_H_
#define FSM_H_

#include "main.h"
#include <stdint.h>

typedef enum
{
    STATE_NO_LED_SEQUENCE = 0,
    STATE_LED_SEQUENCE_1,
    STATE_LED_SEQUENCE_2,
    STATE_LED_SEQUENCE_3,
    STATE_LED_SEQUENCE_4,
    NUMBER_OF_SEQUENCES
} FSM_Event_TypeDef;


typedef struct
{
    GPIO_TypeDef *LED1_Port;
    uint16_t      LED1_Pin;
    GPIO_TypeDef *LED2_Port;
    uint16_t      LED2_Pin;
    GPIO_TypeDef *LED3_Port;
    uint16_t      LED3_Pin;
    GPIO_TypeDef *Btn_Port;
    uint16_t      Btn_Pin;
} FSM_Config_HandleTypeDef;

void FSM_Init(const FSM_Config_HandleTypeDef *config);

void FSM_Process(void);

void Button_Process_EXTI_Callback(uint16_t btn_pin);

uint16_t FSM_GetButtonPin(void);

#endif
