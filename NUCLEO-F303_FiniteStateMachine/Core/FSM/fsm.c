#include "fsm.h"
#include "stm32f303xe.h"
#include "stm32f3xx_hal.h"
#include "stm32f3xx_hal_gpio.h"

static FSM_Config_HandleTypeDef pConfig;
static volatile FSM_State_TypeDef currentState = FSM_STATE_OFF;

static void LEDs_Off(void);
static void LEDs_Toggle(void);

typedef struct
{
    GPIO_PinState led1;
    GPIO_PinState led2;
    GPIO_PinState led3;
} LED_Pattern_Step_t;

// Al declarar la tabla como const, el compilador la almacena directamente en la memoria FLASH (ROM) del microcontrolador.
// Al declarar la tabla como static, limitas su visibilidad exclusivamente al archivo fsm.c, evitando colisiones de nombres con otros archivos.
static const LED_Pattern_Step_t sequence3_patterns[] = {
    { GPIO_PIN_SET,   GPIO_PIN_SET,   GPIO_PIN_RESET }, 
    { GPIO_PIN_RESET, GPIO_PIN_RESET, GPIO_PIN_RESET }, 
    { GPIO_PIN_RESET, GPIO_PIN_SET,   GPIO_PIN_SET   }, 
    { GPIO_PIN_RESET, GPIO_PIN_RESET, GPIO_PIN_RESET }, 
    { GPIO_PIN_SET,   GPIO_PIN_RESET, GPIO_PIN_SET   }, 
    { GPIO_PIN_RESET, GPIO_PIN_RESET, GPIO_PIN_RESET }  
};

void FSM_Init(const FSM_Config_HandleTypeDef *config)
{
    if (config == NULL)
    {
        return;
    }

    pConfig = *config;

    LEDs_Off();
}

static void Run_LEDs_Sequence(GPIO_TypeDef *port1, uint16_t pin1,
                              GPIO_TypeDef *port2, uint16_t pin2,
                              GPIO_TypeDef *port3, uint16_t pin3,
                              uint8_t *step)
{
    switch (*step)
    {
        case 0:
            HAL_GPIO_WritePin(port1, pin1, GPIO_PIN_SET);   
            *step = 1;
            break;
        case 1:
            HAL_GPIO_WritePin(port2, pin2, GPIO_PIN_SET);   
            *step = 2;
            break;
        case 2:
            HAL_GPIO_WritePin(port3, pin3, GPIO_PIN_SET);   
            *step = 3;
            break;
        case 3:
            HAL_GPIO_WritePin(port1, pin1, GPIO_PIN_RESET);   
            *step = 4;
            break;
        case 4:
            HAL_GPIO_WritePin(port2, pin2, GPIO_PIN_RESET);
            *step = 5;
            break;
        case 5:
            HAL_GPIO_WritePin(port3, pin3, GPIO_PIN_RESET);
            *step = 0;
            break;
        default:
            *step = 0;
            break;
    }
}

void FSM_Process(void)
{
    static FSM_State_TypeDef previousState = FSM_STATE_OFF;
    static uint32_t lastTick = 0;
    static uint8_t step = 0;

    uint32_t currentTick = HAL_GetTick();

    if (currentState != previousState)
    {
        previousState = currentState;
        LEDs_Off();
        step = 0;
        lastTick = currentTick;
    }

    switch(currentState)
    {
        case FSM_STATE_OFF:
            LEDs_Off();
            step = 0;
            break;
            
        case FSM_STATE_CASCADE_UP:
            if ((currentTick - lastTick) >= 250)
            {
                lastTick = currentTick;

                Run_LEDs_Sequence(pConfig.LED1_Port, pConfig.LED1_Pin,
                                  pConfig.LED2_Port, pConfig.LED2_Pin,
                                  pConfig.LED3_Port, pConfig.LED3_Pin,
                                  &step);
            }
            break;
        case FSM_STATE_CASCADE_DOWN:
            if ((currentTick - lastTick) >= 250)
            {
                lastTick = currentTick;

                Run_LEDs_Sequence(pConfig.LED3_Port, pConfig.LED3_Pin,
                                  pConfig.LED2_Port, pConfig.LED2_Pin,
                                  pConfig.LED1_Port, pConfig.LED1_Pin,
                                  &step);
            }
            break;
        
        case FSM_STATE_PATTERN_CUSTOM:
            if ((currentTick - lastTick) >= 250)
            {
                lastTick = currentTick;

                uint8_t totalSteps = sizeof(sequence3_patterns) / sizeof(sequence3_patterns[0]);
                
                HAL_GPIO_WritePin(pConfig.LED1_Port, pConfig.LED1_Pin, sequence3_patterns[step].led1);
                HAL_GPIO_WritePin(pConfig.LED2_Port, pConfig.LED2_Pin, sequence3_patterns[step].led2);
                HAL_GPIO_WritePin(pConfig.LED3_Port, pConfig.LED3_Pin, sequence3_patterns[step].led3);

                step++;
                if (step >= totalSteps)
                {
                    step = 0;
                }
            }
            break;

        case FSM_STATE_STROBE:
            if ((currentTick - lastTick) >= 100)
            {
                lastTick = currentTick;

                LEDs_Toggle();
            }
            break;
            
        default:
            currentState = FSM_STATE_OFF;
            break;
    }
}

static void LEDs_Off(void)
{
    HAL_GPIO_WritePin(pConfig.LED1_Port, pConfig.LED1_Pin, GPIO_PIN_RESET);
    HAL_GPIO_WritePin(pConfig.LED2_Port, pConfig.LED2_Pin, GPIO_PIN_RESET);
    HAL_GPIO_WritePin(pConfig.LED3_Port, pConfig.LED3_Pin, GPIO_PIN_RESET);
}

static void LEDs_Toggle(void)
{
    HAL_GPIO_TogglePin(pConfig.LED1_Port, pConfig.LED1_Pin);
    HAL_GPIO_TogglePin(pConfig.LED2_Port, pConfig.LED2_Pin);
    HAL_GPIO_TogglePin(pConfig.LED3_Port, pConfig.LED3_Pin);
}

uint16_t FSM_GetButtonPin(void)
{
    return pConfig.Btn_Pin;
}

void Button_Process_EXTI_Callback(uint16_t btnPin)
{
    if (btnPin != pConfig.Btn_Pin)
    {
        return;
    }

    static uint32_t lastButtonTick = 0;
    uint32_t currentTick = HAL_GetTick();

    if ((currentTick - lastButtonTick) >= 200)
    {
        lastButtonTick = currentTick;

        currentState = (FSM_State_TypeDef)((currentState + 1) % FSM_STATE_TOTAL_NUM);
    }
}