#ifndef INTERNAL_TEMP_APP_H_
#define INTERNAL_TEMP_APP_H_

#include "main.h"
#include "stm32c0xx_hal_adc.h"

void Internal_Temp_App_Init(ADC_HandleTypeDef *hadc);

void Internal_Temp_App_Task(void);

float Internal_Temp_App_GetTemp(void);

void Internal_Temp_App_Stop(void);

void Internal_Temp_App_Resume(void);

#endif /* INTERNAL_TEMP_APP_H_ */