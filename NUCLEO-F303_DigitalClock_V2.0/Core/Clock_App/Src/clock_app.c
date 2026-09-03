#include "clock_app.h"
#include "TLC5917.h"
#include "internal_temp_app.h"
#include "button_driver.h"

#define ARRAY_SIZE 10

/* Instancias globales de control privado */
static TLC5917_HandleTypeDef tlc5917;
static RTC_HandleTypeDef      *h_rtc = NULL;
static Button_HandleTypeDef   configButton;

/* Arreglo de dígitos del 0 al 9 en representación de 7 segmentos */
static const uint8_t digitsArray[ARRAY_SIZE] = {0x3F, 0x06, 0x5B, 0x4F, 0x66, 0x6D, 0x7D, 0x07, 0x7F, 0x6F};

/* Buffers de los dígitos para multiplexado */
static volatile uint8_t thousand = 0x00; // Decenas de hora / Signo temperatura
static volatile uint8_t hundred  = 0x00; // Unidades de hora / Centena o Decena temperatura
static volatile uint8_t tens     = 0x00; // Decenas de minuto / Unidad temperatura
static volatile uint8_t unit     = 0x00; // Unidades de minuto / Unidad 'C'

/* Variables de multiplexado y temporización */
static volatile uint8_t digitPin = 0;   // Índice de multiplexación (0 a 3)
static volatile _Bool colonBlink = 0;   // Estado de parpadeo de los dos puntos (:)

/* Máquina de estados del reloj */
static Clock_Mode_TypeDef currentMode = MODE_NORMAL;
static uint8_t setHours = 12;
static uint8_t setMinutes = 0;
static uint32_t lastCycleStart = 0;

/* --- Prototipos de Funciones Privadas --- */
static void Display_Off(void);
static void Handle_Button_Setting(void);
static void Update_Display_Segments(void);
static void RTC_Set_Time(uint8_t hours, uint8_t minutes, uint8_t seconds);

/* --- Inicialización de la Aplicación del Reloj --- */
void Clock_App_Init(RTC_HandleTypeDef *hrtc, SPI_HandleTypeDef *hspi)
{
    h_rtc = hrtc;
    
    // Inicializar el controlador TLC5917 y el botón B1
    TLC5917_Init(&tlc5917, hspi, LE_GPIO_Port, LE_Pin);
    Button_Init(&configButton, B1_GPIO_Port, B1_Pin);
    
    // Apagar el display para evitar ghosting inicial
    Display_Off();
    
    lastCycleStart = HAL_GetTick();
}

/* --- Tarea cíclica principal del reloj --- */
void Clock_App_Task(void)
{
    Handle_Button_Setting();
    Update_Display_Segments();
}

/* --- Apaga todos los dígitos deshabilitando los transistores (Pines en HIGH) --- */
static void Display_Off(void)
{
    HAL_GPIO_WritePin(UNIT_GPIO_Port, UNIT_Pin, GPIO_PIN_SET);
    HAL_GPIO_WritePin(TENS_GPIO_Port, TENS_Pin, GPIO_PIN_SET);
    HAL_GPIO_WritePin(HUND_GPIO_Port, HUND_Pin, GPIO_PIN_SET);
    HAL_GPIO_WritePin(THOU_GPIO_Port, THOU_Pin, GPIO_PIN_SET);
}

/* --- Multiplexa los dígitos en el display de 7 segmentos (TIM3 - cada 5ms) --- */
void Clock_App_Multiplex_ISR(void)
{
    // 1. BLANKING: Apagar transistores
    Display_Off();

    // 2. Cargar patrón en el registro y encender el dígito correspondiente (Pin en LOW)
    switch (digitPin)
    {
        case 0:
            TLC5917_WriteRegister(&tlc5917, unit);
            HAL_GPIO_WritePin(UNIT_GPIO_Port, UNIT_Pin, GPIO_PIN_RESET);
            break;
        case 1:
            TLC5917_WriteRegister(&tlc5917, tens);
            HAL_GPIO_WritePin(TENS_GPIO_Port, TENS_Pin, GPIO_PIN_RESET);
            break;
        case 2:
            TLC5917_WriteRegister(&tlc5917, hundred);
            HAL_GPIO_WritePin(HUND_GPIO_Port, HUND_Pin, GPIO_PIN_RESET);
            break;
        case 3:
            TLC5917_WriteRegister(&tlc5917, thousand);
            HAL_GPIO_WritePin(THOU_GPIO_Port, THOU_Pin, GPIO_PIN_RESET);
            break;
    }

    // Indexar el siguiente dígito
    digitPin = (digitPin + 1) % 4;
}

/* --- Evento de Alarma RTC para conmutar parpadeo de dos puntos (:) --- */
void Clock_App_Alarm_ISR(void)
{
    colonBlink = !colonBlink;
}

/* --- Actualización de los datos de los segmentos --- */
static void Update_Display_Segments(void)
{
    uint32_t currentTick = HAL_GetTick();
    static uint8_t displayTemperature = 0;
    static _Bool tempSnapshotTaken = 0;

    /* Ciclo automático de visualización: 45s Hora, 5s Temperatura */
    if (currentMode == MODE_NORMAL || currentMode == MODE_READ_TEMPERATURE)
    {
        uint32_t elapsed = currentTick - lastCycleStart;
        if (elapsed >= 50000)
        {
            lastCycleStart = currentTick;
            currentMode = MODE_NORMAL;
        }
        else if (elapsed >= 45000)
        {
            currentMode = MODE_READ_TEMPERATURE;
        }
        else
        {
            currentMode = MODE_NORMAL;
        }
    }

    /* Formatear segmentos según el estado actual */
    if (currentMode == MODE_NORMAL)
    {
        tempSnapshotTaken = 0; // Liberar bandera
        
        RTC_TimeTypeDef sTime = {0};
        RTC_DateTypeDef sDate = {0};

        if (h_rtc != NULL)
        {
            HAL_RTC_GetTime(h_rtc, &sTime, RTC_FORMAT_BIN);
            HAL_RTC_GetDate(h_rtc, &sDate, RTC_FORMAT_BIN);
        }

        uint8_t colonMask = colonBlink ? 0x80 : 0x00;
        
        thousand = digitsArray[sTime.Hours / 10] | colonMask;
        hundred  = digitsArray[sTime.Hours % 10] | colonMask;
        tens     = digitsArray[sTime.Minutes / 10] | colonMask;
        unit     = digitsArray[sTime.Minutes % 10] | colonMask;
    }
    else if (currentMode == MODE_READ_TEMPERATURE)
    {
        // Tomar una sola captura de temperatura al inicio de los 5s para evitar saltos visuales
        if (!tempSnapshotTaken)
        {
            displayTemperature = (uint8_t)Internal_Temp_App_GetTemp();
            tempSnapshotTaken = 1;
        }

        uint8_t tempInt = displayTemperature;
        
        // Dígito de unidades muestra la letra 'C' (patrón 0x39)
        unit = 0x39;

        if (tempInt < 0)
        {
            int absTemp = -tempInt;
            thousand = 0x40; // Signo '-'
            if (absTemp >= 10)
            {
                hundred = digitsArray[(absTemp / 10) % 10];
                tens    = digitsArray[absTemp % 10];
            }
            else
            {
                hundred = 0x00; // Vacío
                tens    = digitsArray[absTemp];
            }
        }
        else
        {
            thousand = 0x00; // Vacío
            if (tempInt >= 100)
            {
                thousand = digitsArray[(tempInt / 100) % 10];
                hundred  = digitsArray[(tempInt / 10) % 10];
                tens     = digitsArray[tempInt % 10];
            }
            else if (tempInt >= 10)
            {
                hundred = digitsArray[tempInt / 10];
                tens    = digitsArray[tempInt % 10];
            }
            else
            {
                hundred = 0x00; // Vacío
                tens    = digitsArray[tempInt];
            }
        }
    }
    else if (currentMode == MODE_SET_HOURS)
    {
        // Parpadeo rápido (4Hz / 250ms) de las horas en edición
        _Bool blinkOn = (currentTick / 250) % 2 == 0;
        thousand = blinkOn ? (digitsArray[setHours / 10] | 0x80) : 0x80;
        hundred  = blinkOn ? (digitsArray[setHours % 10] | 0x80) : 0x80;
        tens     = digitsArray[setMinutes / 10] | 0x80;
        unit     = digitsArray[setMinutes % 10] | 0x80;
    }
    else if (currentMode == MODE_SET_MINUTES)
    {
        // Parpadeo rápido (4Hz / 250ms) de los minutos en edición
        _Bool blinkOn = (currentTick / 250) % 2 == 0;
        thousand = digitsArray[setHours / 10] | 0x80;
        hundred  = digitsArray[setHours % 10] | 0x80;
        tens     = blinkOn ? (digitsArray[setMinutes / 10] | 0x80) : 0x80;
        unit     = blinkOn ? (digitsArray[setMinutes % 10] | 0x80) : 0x80;
    }
}

/* --- Gestión de la máquina de estados con el Botón B1 (PC13) --- */
static void Handle_Button_Setting(void)
{
    Button_Event_TypeDef event = Button_Process(&configButton);

    if (event == BUTTON_EVENT_SHORT_PRESS)
    {
        if (currentMode == MODE_SET_HOURS)
        {
            setHours = (setHours + 1) % 24;
        }
        else if (currentMode == MODE_SET_MINUTES)
        {
            setMinutes = (setMinutes + 1) % 60;
        }
    }
    else if (event == BUTTON_EVENT_LONG_PRESS)
    {
        if (currentMode == MODE_NORMAL)
        {
            // Entrar a ajustar horas
            RTC_TimeTypeDef sTime = {0};
            if (h_rtc != NULL)
            {
                HAL_RTC_GetTime(h_rtc, &sTime, RTC_FORMAT_BIN);
            }
            setHours = sTime.Hours;
            setMinutes = sTime.Minutes;
            currentMode = MODE_SET_HOURS;
        }
        else if (currentMode == MODE_SET_HOURS)
        {
            // Pasar a ajustar minutos
            currentMode = MODE_SET_MINUTES;
        }
        else if (currentMode == MODE_SET_MINUTES)
        {
            // Guardar y retornar
            RTC_Set_Time(setHours, setMinutes, 0);
            currentMode = MODE_NORMAL;
            lastCycleStart = HAL_GetTick();
        }
    }
}

/* --- Ajustar la hora en el RTC físico --- */
static void RTC_Set_Time(uint8_t hours, uint8_t minutes, uint8_t seconds)
{
    RTC_TimeTypeDef sTime = {0};
    sTime.Hours = hours;
    sTime.Minutes = minutes;
    sTime.Seconds = seconds;
    sTime.DayLightSaving = RTC_DAYLIGHTSAVING_NONE;
    sTime.StoreOperation = RTC_STOREOPERATION_RESET;
    
    if (h_rtc != NULL)
    {
        HAL_RTC_SetTime(h_rtc, &sTime, RTC_FORMAT_BIN);
    }
}

/* ========================================================================== */
/*                SOBREESCRITURA DE CALLBACKS DÉBILES DE LA HAL               */
/* ========================================================================== */

void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
    if (htim->Instance == TIM3)
    {
        Clock_App_Multiplex_ISR();
    }
}

void HAL_RTC_AlarmAEventCallback(RTC_HandleTypeDef *hrtc)
{
    Clock_App_Alarm_ISR();
}