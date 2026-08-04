#include "main.h"
#include "stm32f3xx_hal_tim.h"
#include <stdint.h>
#include "Servomotor.h"

HAL_StatusTypeDef Servo_Init(Servo_HandleTypeDef *hservo, TIM_HandleTypeDef *htim, uint32_t channel, uint16_t minPulseUs, uint16_t maxPulseUs)
{
    if (hservo == NULL || htim == NULL)
    {
        return HAL_ERROR;
    }

    hservo->htim = htim;
    hservo->channel = channel;
    hservo->minPulseUs = minPulseUs;
    hservo->maxPulseUs = maxPulseUs;
    hservo->currentAngle = 90; // Ángulo inicial de seguridad (centro)

    if (HAL_TIM_PWM_Start(hservo->htim, hservo->channel) != HAL_OK)
    {
        return HAL_ERROR;
    }

    Servo_WriteAngle(hservo, hservo->currentAngle);

    return HAL_OK;
}

void Servo_WriteAngle(Servo_HandleTypeDef *hservo, uint8_t angle)
{
    if (hservo == NULL) return;

    // Limitar el ángulo al rango físico de 0 a 180 grados
    if (angle > 180)
    {
        angle = 180;
    }

    hservo->currentAngle = angle;

    // Obtener la frecuencia de conteo del Timer leyendo su registro de Prescaler
    // (Fórmula: ticks por microsegundo)
    // Asumiendo que el timer se configuró a 1 MHz (1 tick = 1 us) mediante PSC = 71
    // Mapeo lineal: Ancho de pulso = Min + (Rango_Pulso * angulo) / 180
    uint32_t pulseWidthUs = hservo->minPulseUs + (((uint32_t)angle * (hservo->maxPulseUs - hservo->minPulseUs)) / 180);

    __HAL_TIM_SET_COMPARE(hservo->htim, hservo->channel, pulseWidthUs);
}

uint8_t Servo_ReadAngle(Servo_HandleTypeDef *hservo)
{
    if (hservo == NULL)
    {
        return 0;
    }

    return hservo->currentAngle;
}

void Servo_Stop(Servo_HandleTypeDef *hservo)
{
    if (hservo == NULL) return;

    // Detiene el temporizador PWM. Al no haber pulsos, el servo se queda libre mecánicamente.
    HAL_TIM_PWM_Stop(hservo->htim, hservo->channel);
}