#ifndef FSM_H_
#define FSM_H_

#include "main.h"
#include <stdint.h>

typedef enum
{
    FSM_STATE_OFF = 0,
    FSM_STATE_CASCADE_UP,
    FSM_STATE_CASCADE_DOWN,
    FSM_STATE_PATTERN_CUSTOM,
    FSM_STATE_STROBE,
    FSM_STATE_TOTAL_NUM
} FSM_State_TypeDef;


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
