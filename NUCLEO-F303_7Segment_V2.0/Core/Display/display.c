#include "display.h"
#include "stm32f3xx_hal_gpio.h"
#include "stm32f3xx_hal_rcc.h"
#include "stm32f3xx_hal_rcc_ex.h"
#include "stm32f3xx_hal_tim.h"

#define SEGMENTS_SIZE   7   // 7 segmentos (A, B, C, D, E, F, G)
#define DIGITS_SIZE     10  // Digits from 0 to 9

static GPIO_PortMap_HandleTypeDef  *pSegmentPins = NULL;
static TIM_HandleTypeDef *pTimHandle   = NULL;
static volatile uint8_t digitUpdateFlag = 0; // Bandera volátil para sincronizar la interrupción con la tarea principal
static uint8_t currentDigit = 0; // Variable estática a nivel de archivo para recordar el dígito actual

static const uint8_t digitsArray[] = {0x3F, 0x06, 0x5B, 0x4F, 0x66, 0x6D, 0x7D, 0x07, 0x7F, 0x6F};

void Display_Init(GPIO_PortMap_HandleTypeDef *segmentPinList, TIM_HandleTypeDef *htim)
{
    if (segmentPinList == NULL || htim == NULL)
    {
        return;
    }
    pSegmentPins = segmentPinList;
    pTimHandle   = htim;
    currentDigit = 0;

    Display_ShowDigit(currentDigit);

    // Arrancar el Timer con interrupciones
    HAL_TIM_Base_Start_IT(pTimHandle);
}

void Display_ShowDigit(uint8_t digit)
{
    if (pSegmentPins == NULL || digit >= DIGITS_SIZE)
    {
        return;
    }

    uint8_t bytePattern = digitsArray[digit];

    for (uint8_t i = 0; i < SEGMENTS_SIZE; i++)
    {
        GPIO_PinState state = (bytePattern & (1 << i)) ? GPIO_PIN_SET : GPIO_PIN_RESET;
        HAL_GPIO_WritePin(pSegmentPins[i].Segment_Port, pSegmentPins[i].Segment_Pin, state);
    }
}


void Display_Task(void)
{
    if (pSegmentPins == NULL)
    {
        return;
    }

    if (digitUpdateFlag)
    {
        digitUpdateFlag = 0;

        currentDigit++;
        if (currentDigit >= DIGITS_SIZE)
        {
            currentDigit = 0;
        }

        Display_ShowDigit(currentDigit);
    }
}

void Display_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
    // Verifica si la interrupción proviene del timer registrado (desacoplado de TIM3)
    if (htim == pTimHandle)
    {
        digitUpdateFlag = 1;
    }
}