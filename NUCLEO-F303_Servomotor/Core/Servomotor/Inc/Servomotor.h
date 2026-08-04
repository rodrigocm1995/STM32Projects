#ifndef INC_SERVOMOTOR_H_
#define INC_SERVOMOTOR_H_



typedef struct
{
    TIM_HandleTypeDef *htim; // Pointer to HAL timer (e.g.: &htim2)
    uint32_t channel;        // Timer Channel (e.g.: TIM_CHANNEL_1)
    uint16_t minPulseUs;     // Minumim pulse in us to 0° (e.g.: 500)
    uint16_t maxPulseUs;     // Maximum pulse in us to 180° (e.g.: 2500)
    uint8_t currentAngle;    // Current servomotor angle (0° to 180°)      
} Servo_HandleTypeDef;

HAL_StatusTypeDef Servo_Init(Servo_HandleTypeDef *hservo, TIM_HandleTypeDef *htim, uint32_t channel, uint16_t minPulseUs, uint16_t maxPulseUs);

void Servo_WriteAngle(Servo_HandleTypeDef *hservo, uint8_t angle);

uint8_t Servo_ReadAngle(Servo_HandleTypeDef *hservo);

void Servo_Stop(Servo_HandleTypeDef *hservo);

#endif
