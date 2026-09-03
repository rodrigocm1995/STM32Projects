#include "clock_driver.h"
#include "TLC5917.h"
#include "internal_temp_app.h"
#include "button_driver.h"
#include "main.h"
#include "stm32c0xx_hal_gpio.h"
#include "stm32c0xx_hal_rtc.h"
#include "stm32c0xx_hal_tim.h"

#define ARRAY_SIZE 10 /* Array para almacenar los 10 dígitos (0-9) */

/* Declaraciones externas necesarias del sistema */
extern TIM_HandleTypeDef htim3;
extern void SystemClock_Config(void);

/* Instancias globales de control privado */
static TLC5917_HandleTypeDef tlc5917;
static RTC_HandleTypeDef     *h_rtc =  NULL;
static Button_HandleTypeDef   configButton;
static volatile _Bool powerLostFlag = 0;

/* Arreglo de dígitos del 0 al 9 en representación de 7 segmentos */
static const uint8_t digitsArray[ARRAY_SIZE] = {
    0x3F, //Dígito 0
    0x06, //Dígito 1
    0x5B, //Dígito 2
    0x4F, //Dígito 3
    0x66, //Dígito 4
    0x6D, //Dígito 5
    0x7D, //Dígito 6
    0x07, //Dígito 7
    0x7F, //Dígito 8
    0x6F  //Dígito 9
};

/* Buffers de los dígitos para el multiplexado */
static volatile uint8_t thousand = 0x00; // Decenas de hora / Signo de temperatura
static volatile uint8_t hundred  = 0x00; // Unidades de hora / Centena o Decena de temperatura
static volatile uint8_t tens     = 0x00; // Decenas de minuto / Unidad de temperatura
static volatile uint8_t unit     = 0x00; // Unidades de minuto / Unidad 'C'

/* Variables de multiplexado y temporización */
static volatile uint8_t digitPin = 0; // Indice de multiplexación (0 al 3), especifica qué dígito está activado
static volatile _Bool colonBlink = 0; // Estado de parpadeo de los dos puntos (:)

/* Máquina de estados del reloj */
static Clock_Mode_TypeDef currentMode = MODE_NORMAL;
static uint8_t setHours = 12;
static uint8_t setMinutes = 0;
static uint32_t lastCycleStart = 0;

/* Prototipos de Funciones Privadas */
static void Display_Off(void);
static void Handle_Button_Setting(void);
static void Update_Display_Segments(void);
static void RTC_Set_Time(uint8_t hours, uint8_t minutes, uint8_t seconds);
void Check_Power_Management(void);


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
    Check_Power_Management();
    Handle_Button_Setting();
    Update_Display_Segments();
}


void Display_Off(void)
{
    // Se desactivan los transistores PNP poniendo los pines en HIGH (corta corriente de base a 0 uA)
    HAL_GPIO_WritePin(UNIT_GPIO_Port, UNIT_Pin, GPIO_PIN_SET);
    HAL_GPIO_WritePin(TENS_GPIO_Port, TENS_Pin, GPIO_PIN_SET);
    HAL_GPIO_WritePin(HUND_GPIO_Port, HUND_Pin, GPIO_PIN_SET);
    HAL_GPIO_WritePin(THOU_GPIO_Port, THOU_Pin, GPIO_PIN_SET);
    HAL_GPIO_WritePin(COLON_GPIO_Port, COLON_Pin, GPIO_PIN_SET);
}

static void Display_Pins_Isolate_LowPower(void)
{
    // Desconectar internamente los pines hacia modo analógico (alta impedancia High-Z)
    // Esto elimina cualquier fuga parásita hacia pistas desenergizadas
    GPIO_InitTypeDef GPIO_InitStruct = {0};
    GPIO_InitStruct.Pin = UNIT_Pin | TENS_Pin | HUND_Pin | THOU_Pin | COLON_Pin | LE_Pin | GPIO_PIN_1 | GPIO_PIN_2;
    GPIO_InitStruct.Mode = GPIO_MODE_ANALOG;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);
}

static void Display_Pins_Restore_Normal(void)
{
    GPIO_InitTypeDef GPIO_InitStruct = {0};

    // Restaurar pines de transistores y Latch Enable a salidas Push-Pull
    Display_Off(); // Ponerlos en nivel seguro primero
    GPIO_InitStruct.Pin = LE_Pin | COLON_Pin | THOU_Pin | HUND_Pin | TENS_Pin | UNIT_Pin;
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

    // Restaurar pines SPI1 (PA1 SCK, PA2 MOSI) a función alternativa AF0
    GPIO_InitStruct.Pin = GPIO_PIN_1 | GPIO_PIN_2;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
    GPIO_InitStruct.Alternate = GPIO_AF0_SPI1;
    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);
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
    static int8_t displayTemperature = 0;
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

        // Parpadeo de dos puntos (:) cada 500 ms (500 ms encendido / 500 ms apagado)
        _Bool colonState = (currentTick / 500) % 2 == 0;

        // Conmutar el transistor físico del COLON (PA6): LOW enciende PNP, HIGH apaga
        HAL_GPIO_WritePin(COLON_GPIO_Port, COLON_Pin, colonState ? GPIO_PIN_RESET : GPIO_PIN_SET);

        uint8_t colonMask = colonState ? 0x80 : 0x00;
        
        thousand = digitsArray[sTime.Hours / 10] | colonMask;
        hundred  = digitsArray[sTime.Hours % 10] | colonMask;
        tens     = digitsArray[sTime.Minutes / 10] | colonMask;
        unit     = digitsArray[sTime.Minutes % 10] | colonMask;
    }
    else if (currentMode == MODE_READ_TEMPERATURE)
    {
        // En modo temperatura apagamos el COLON
        HAL_GPIO_WritePin(COLON_GPIO_Port, COLON_Pin, GPIO_PIN_SET);

        // Tomar una sola captura de temperatura al inicio de los 5s para evitar saltos visuales
        if (!tempSnapshotTaken)
        {
            displayTemperature = (int8_t)Internal_Temp_App_GetTemp();
            tempSnapshotTaken = 1;
        }

        int8_t tempInt = displayTemperature;
        
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
        // En modo ajuste, dejamos los dos puntos fijos encendidos
        HAL_GPIO_WritePin(COLON_GPIO_Port, COLON_Pin, GPIO_PIN_RESET);

        // Parpadeo rápido (4Hz / 250ms) de las horas en edición
        _Bool blinkOn = (currentTick / 250) % 2 == 0;
        thousand = blinkOn ? (digitsArray[setHours / 10] | 0x80) : 0x80;
        hundred  = blinkOn ? (digitsArray[setHours % 10] | 0x80) : 0x80;
        tens     = digitsArray[setMinutes / 10] | 0x80;
        unit     = digitsArray[setMinutes % 10] | 0x80;
    }
    else if (currentMode == MODE_SET_MINUTES)
    {
        // En modo ajuste, dejamos los dos puntos fijos encendidos
        HAL_GPIO_WritePin(COLON_GPIO_Port, COLON_Pin, GPIO_PIN_RESET);

        // Parpadeo rápido (4Hz / 250ms) de los minutos en edición
        _Bool blinkOn = (currentTick / 250) % 2 == 0;
        thousand = digitsArray[setHours / 10] | 0x80;
        hundred  = digitsArray[setHours % 10] | 0x80;
        tens     = blinkOn ? (digitsArray[setMinutes / 10] | 0x80) : 0x80;
        unit     = blinkOn ? (digitsArray[setMinutes % 10] | 0x80) : 0x80;
    }
}

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
            // Entrar al ajuste de horas
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
            //Guardar y retornar
            RTC_Set_Time(setHours, setMinutes, 0);
            currentMode = MODE_NORMAL;
            lastCycleStart = HAL_GetTick();
        }
    }
}

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

void Check_Power_Management(void)
{
    // Verificar si la energía externa está ausente (PB7 en nivel bajo) o si hubo bandera de corte
    if (HAL_GPIO_ReadPin(POWER_OUTAGE_GPIO_Port, POWER_OUTAGE_Pin) == GPIO_PIN_RESET || powerLostFlag)
    {
        // Pequeño filtro antirruido de 15 ms para confirmar que no sea un transitorio
        HAL_Delay(15);
        if (HAL_GPIO_ReadPin(POWER_OUTAGE_GPIO_Port, POWER_OUTAGE_Pin) != GPIO_PIN_RESET)
        {
            powerLostFlag = 0;
            return; // La energía sigue presente, fue un transitorio
        }

        powerLostFlag = 0;

        /* =====================================================================
         *        PREPARACIÓN PARA MODO STOP (CORTE TOTAL DE CONSUMO)
         * ===================================================================== */

        // 1. Apagar display y transistores (corta corriente de base a 0 uA)
        Display_Off();

        // 2. Detener temporizador de multiplexado
        HAL_TIM_Base_Stop_IT(&htim3);

        // 3. Detener y apagar totalmente el ADC y sus referencias analógicas internas (TSEN y VREFINT)
        Internal_Temp_App_Stop();

        // 4. Desactivar la Alarma A del RTC para que no despierte a la CPU cada segundo.
        // (El RTC sigue contando las horas y minutos internamente por hardware con el LSE).
        if (h_rtc != NULL)
        {
            HAL_RTC_DeactivateAlarm(h_rtc, RTC_ALARM_A);
        }

        // 5. Aislar todos los pines del display y SPI a modo analógico (High-Z)
        // para cortar cualquier fuga hacia pistas externas desenergizadas
        Display_Pins_Isolate_LowPower();

        // 6. Suspender SysTick para que no despierte a la CPU cada 1 ms
        HAL_SuspendTick();

        // 7. Bucle de ultra bajo consumo: mientras la energía externa siga ausente,
        // la CPU permanecerá congelada en STOP. Si cualquier evento parásito la despierta,
        // el bucle la vuelve a dormir inmediatamente.
        while (HAL_GPIO_ReadPin(POWER_OUTAGE_GPIO_Port, POWER_OUTAGE_Pin) == GPIO_PIN_RESET)
        {
            HAL_PWR_EnterSTOPMode(PWR_MAINREGULATOR_ON, PWR_STOPENTRY_WFI);
        }

        /* =====================================================================
         * -- EL MICROCONTROLADOR SALE DE AQUÍ CUANDO LA ENERGÍA EXTERNA REGRESA --
         * ===================================================================== */

        // 8. Restablecer el oscilador y el reloj del sistema a 48 MHz
        SystemClock_Config();

        // 9. Reanudar SysTick
        HAL_ResumeTick();

        // 10. Restaurar los pines del display y del bus SPI a sus modos normales
        Display_Pins_Restore_Normal();

        // 11. Reactivar el ADC y su adquisición DMA
        Internal_Temp_App_Resume();

        // 12. Reactivar la Alarma A del RTC
        if (h_rtc != NULL)
        {
            RTC_AlarmTypeDef sAlarm = {0};
            sAlarm.AlarmMask = RTC_ALARMMASK_ALL;
            sAlarm.Alarm = RTC_ALARM_A;
            HAL_RTC_SetAlarm_IT(h_rtc, &sAlarm, RTC_FORMAT_BCD);
        }

        // 13. Reanudar el multiplexado del display
        HAL_TIM_Base_Start_IT(&htim3);
    }
}

/* ========================================================================== */
/*                SOBREESCRITURA DE CALLBACKS DE LA HAL                       */
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

void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
{
    return;
}


void HAL_GPIO_EXTI_Falling_Callback(uint16_t GPIO_Pin)
{
    if (GPIO_Pin == POWER_OUTAGE_Pin)
    {
        Display_Off();

        powerLostFlag = 1;
    }
}