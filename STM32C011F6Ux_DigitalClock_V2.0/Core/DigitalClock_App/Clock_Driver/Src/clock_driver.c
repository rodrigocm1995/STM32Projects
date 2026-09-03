/**
  ******************************************************************************
  * @file           : clock_driver.c
  * @brief          : Digital Clock Application controller implementation.
  ******************************************************************************
  */

#include "clock_driver.h"
#include "TLC5917.h"
#include "internal_temp_app.h"
#include "button_driver.h"
#include "main.h"
#include "stm32c0xx_hal_gpio.h"
#include "stm32c0xx_hal_rtc.h"
#include "stm32c0xx_hal_tim.h"

#define ARRAY_SIZE 10 /**< Number of digit glyphs (0-9) */

/* External system clock restoration routine */
extern void SystemClock_Config(void);

/* Private hardware control handles and state flags */
static TLC5917_HandleTypeDef tlc5917;
static RTC_HandleTypeDef     *h_rtc = NULL;
static TIM_HandleTypeDef     *h_tim = NULL;
static Button_HandleTypeDef   configButton;
static volatile _Bool         powerLostFlag = 0;

/* 7-Segment common-anode digit patterns (0 to 9) */
static const uint8_t digitsArray[ARRAY_SIZE] = {
    0x3F, // Digit 0
    0x06, // Digit 1
    0x5B, // Digit 2
    0x4F, // Digit 3
    0x66, // Digit 4
    0x6D, // Digit 5
    0x7D, // Digit 6
    0x07, // Digit 7
    0x7F, // Digit 8
    0x6F  // Digit 9
};

/* Multiplexing digit segment buffers */
static volatile uint8_t thousand = 0x00; // Tens of hours / Temperature sign
static volatile uint8_t hundred  = 0x00; // Units of hours / Hundreds or tens of temperature
static volatile uint8_t tens     = 0x00; // Tens of minutes / Units of temperature
static volatile uint8_t unit     = 0x00; // Units of minutes / 'C' unit glyph

/* Multiplexing and timing state variables */
static volatile uint8_t digitPin = 0; // Multiplex index (0 to 3), designates active digit
static volatile _Bool colonBlink = 0; // Blink state flag for colon (:)

/* Clock state machine and user settings */
static Clock_Mode_TypeDef currentMode = MODE_NORMAL;
static uint8_t setHours = 12;
static uint8_t setMinutes = 0;
static uint32_t lastCycleStart = 0;

/* Private Function Prototypes */
static void Display_Off(void);
static void Display_Pins_Isolate_LowPower(void);
static void Display_Pins_Restore_Normal(void);
static void Handle_Button_Setting(void);
static void Update_Display_Segments(void);
static void RTC_Set_Time(uint8_t hours, uint8_t minutes, uint8_t seconds);
void Check_Power_Management(void);

/**
  * @brief  Initializes the clock application, binds peripheral handles and drivers.
  * @param  hrtc: Pointer to RTC handle structure.
  * @param  hspi: Pointer to SPI handle structure for TLC5917 sink driver.
  * @param  htim: Pointer to TIM handle structure for display multiplexing.
  * @retval None
  */
void Clock_App_Init(RTC_HandleTypeDef *hrtc, SPI_HandleTypeDef *hspi, TIM_HandleTypeDef *htim)
{
    h_rtc = hrtc;
    h_tim = htim;
    
    // Initialize TLC5917 LED driver and user configuration button B1
    TLC5917_Init(&tlc5917, hspi, LE_GPIO_Port, LE_Pin);
    Button_Init(&configButton, B1_GPIO_Port, B1_Pin);
    
    // Turn off display to prevent initial ghosting
    Display_Off();
    
    lastCycleStart = HAL_GetTick();
}

/**
  * @brief  Main periodic task running state machines, button polling, and display refresh.
  * @retval None
  */
void Clock_App_Task(void)
{
    Check_Power_Management();
    Handle_Button_Setting();
    Update_Display_Segments();
}

/**
  * @brief  Turns off all common-anode PNP transistors by driving their base pins HIGH.
  * @retval None
  */
void Display_Off(void)
{
    // Deactivate PNP transistors by setting base pins HIGH (cuts base current to 0 uA)
    HAL_GPIO_WritePin(UNIT_GPIO_Port, UNIT_Pin, GPIO_PIN_SET);
    HAL_GPIO_WritePin(TENS_GPIO_Port, TENS_Pin, GPIO_PIN_SET);
    HAL_GPIO_WritePin(HUND_GPIO_Port, HUND_Pin, GPIO_PIN_SET);
    HAL_GPIO_WritePin(THOU_GPIO_Port, THOU_Pin, GPIO_PIN_SET);
    HAL_GPIO_WritePin(COLON_GPIO_Port, COLON_Pin, GPIO_PIN_SET);
}

/**
  * @brief  Configures display, SPI, and transistor pins as high-impedance analog inputs.
  *         Eliminates leakage currents through external pull-up resistors during STOP mode.
  * @retval None
  */
static void Display_Pins_Isolate_LowPower(void)
{
    // Internally disconnect pins by setting them to Analog mode (high-impedance High-Z)
    // Eliminates any parasitic leakage into unpowered power rails
    GPIO_InitTypeDef GPIO_InitStruct = {0};
    GPIO_InitStruct.Pin = UNIT_Pin | TENS_Pin | HUND_Pin | THOU_Pin | COLON_Pin | LE_Pin | GPIO_PIN_1 | GPIO_PIN_2;
    GPIO_InitStruct.Mode = GPIO_MODE_ANALOG;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);
}

/**
  * @brief  Restores display and SPI GPIO pins to Push-Pull outputs upon wake-up.
  * @retval None
  */
static void Display_Pins_Restore_Normal(void)
{
    GPIO_InitTypeDef GPIO_InitStruct = {0};

    // Restore transistor base and Latch Enable pins to Push-Pull outputs
    Display_Off(); // Place pins in safe OFF state first
    GPIO_InitStruct.Pin = LE_Pin | COLON_Pin | THOU_Pin | HUND_Pin | TENS_Pin | UNIT_Pin;
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

    // Restore SPI1 pins (PA1 SCK, PA2 MOSI) to alternate function AF0
    GPIO_InitStruct.Pin = GPIO_PIN_1 | GPIO_PIN_2;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
    GPIO_InitStruct.Alternate = GPIO_AF0_SPI1;
    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);
}

/**
  * @brief  Timer ISR multiplexing 7-segment display digits (invoked every 5 ms).
  * @retval None
  */
void Clock_App_Multiplex_ISR(void)
{
    // 1. BLANKING: Turn off all transistors to avoid ghosting between digits
    Display_Off();

    // 2. Load segment pattern into TLC5917 and activate target digit (driving base LOW)
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

    // Advance to next digit index for the subsequent multiplex cycle
    digitPin = (digitPin + 1) % 4;
}

/**
  * @brief  RTC Alarm A callback toggling the colon blink state.
  * @retval None
  */
void Clock_App_Alarm_ISR(void)
{
    colonBlink = !colonBlink;
}

/**
  * @brief  Updates 7-segment digit buffers based on current mode and timing.
  *         Executes 45s Clock / 5s Temperature cycle, colon blinking, and mode rendering.
  * @retval None
  */
static void Update_Display_Segments(void)
{
    uint32_t currentTick = HAL_GetTick();
    static int8_t displayTemperature = 0;
    static _Bool tempSnapshotTaken = 0;

    /* Automatic display cycle: 45s Time, 5s Temperature */
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

    /* Format segment buffers according to active mode */
    if (currentMode == MODE_NORMAL)
    {
        tempSnapshotTaken = 0; // Release snapshot flag for next temperature cycle
        
        RTC_TimeTypeDef sTime = {0};
        RTC_DateTypeDef sDate = {0};

        if (h_rtc != NULL)
        {
            HAL_RTC_GetTime(h_rtc, &sTime, RTC_FORMAT_BIN);
            HAL_RTC_GetDate(h_rtc, &sDate, RTC_FORMAT_BIN);
        }

        // Colon (:) blinking every 500 ms (500 ms ON / 500 ms OFF)
        _Bool colonState = (currentTick / 500) % 2 == 0;

        // Drive physical COLON transistor (PA6): LOW turns PNP ON, HIGH turns OFF
        HAL_GPIO_WritePin(COLON_GPIO_Port, COLON_Pin, colonState ? GPIO_PIN_RESET : GPIO_PIN_SET);

        uint8_t colonMask = colonState ? 0x80 : 0x00;
        
        thousand = digitsArray[sTime.Hours / 10] | colonMask;
        hundred  = digitsArray[sTime.Hours % 10] | colonMask;
        tens     = digitsArray[sTime.Minutes / 10] | colonMask;
        unit     = digitsArray[sTime.Minutes % 10] | colonMask;
    }
    else if (currentMode == MODE_READ_TEMPERATURE)
    {
        // In temperature display mode, turn off colon LEDs
        HAL_GPIO_WritePin(COLON_GPIO_Port, COLON_Pin, GPIO_PIN_SET);

        // Take a single stable temperature snapshot at the start of the 5s window
        if (!tempSnapshotTaken)
        {
            displayTemperature = (int8_t)Internal_Temp_App_GetTemp();
            tempSnapshotTaken = 1;
        }

        int8_t tempInt = displayTemperature;
        
        // Units digit shows letter 'C' (pattern 0x39)
        unit = 0x39;

        if (tempInt < 0)
        {
            int absTemp = -tempInt;
            thousand = 0x40; // Minus sign '-'
            if (absTemp >= 10)
            {
                hundred = digitsArray[(absTemp / 10) % 10];
                tens    = digitsArray[absTemp % 10];
            }
            else
            {
                hundred = 0x00; // Blank
                tens    = digitsArray[absTemp];
            }
        }
        else
        {
            thousand = 0x00; // Blank
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
                hundred = 0x00; // Blank
                tens    = digitsArray[tempInt];
            }
        }
    }
    else if (currentMode == MODE_SET_HOURS)
    {
        // In setting mode, keep colon steady ON
        HAL_GPIO_WritePin(COLON_GPIO_Port, COLON_Pin, GPIO_PIN_RESET);

        // Fast blink (4 Hz / 250 ms) for hours being configured
        _Bool blinkOn = (currentTick / 250) % 2 == 0;
        thousand = blinkOn ? (digitsArray[setHours / 10] | 0x80) : 0x80;
        hundred  = blinkOn ? (digitsArray[setHours % 10] | 0x80) : 0x80;
        tens     = digitsArray[setMinutes / 10] | 0x80;
        unit     = digitsArray[setMinutes % 10] | 0x80;
    }
    else if (currentMode == MODE_SET_MINUTES)
    {
        // In setting mode, keep colon steady ON
        HAL_GPIO_WritePin(COLON_GPIO_Port, COLON_Pin, GPIO_PIN_RESET);

        // Fast blink (4 Hz / 250 ms) for minutes being configured
        _Bool blinkOn = (currentTick / 250) % 2 == 0;
        thousand = digitsArray[setHours / 10] | 0x80;
        hundred  = digitsArray[setHours % 10] | 0x80;
        tens     = blinkOn ? (digitsArray[setMinutes / 10] | 0x80) : 0x80;
        unit     = blinkOn ? (digitsArray[setMinutes % 10] | 0x80) : 0x80;
    }
}

/**
  * @brief  Handles button B1 interactions for entering settings and adjusting time.
  * @retval None
  */
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
            // Enter hours setting mode
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
            // Advance to minutes setting mode
            currentMode = MODE_SET_MINUTES;
        }
        else if (currentMode == MODE_SET_MINUTES)
        {
            // Save new time to RTC hardware and return to normal operation
            RTC_Set_Time(setHours, setMinutes, 0);
            currentMode = MODE_NORMAL;
            lastCycleStart = HAL_GetTick();
        }
    }
}

/**
  * @brief  Configures the RTC peripheral with new hours, minutes, and seconds.
  * @param  hours:   Target hour value (0-23).
  * @param  minutes: Target minute value (0-59).
  * @param  seconds: Target second value (0-59).
  * @retval None
  */
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

/**
  * @brief  Evaluates main power presence and manages deep low-power STOP mode.
  *         When PB7 falls LOW (power outage), this routine blanks the display, stops
  *         timers/ADC/RTC alarm, isolates GPIOs into analog mode, and executes
  *         STOP mode in a closed loop until 5V external power is fully restored.
  * @retval None
  */
void Check_Power_Management(void)
{
    // Check if main power is absent (PB7 at LOW level) or if EXTI outage flag was set
    if (HAL_GPIO_ReadPin(POWER_OUTAGE_GPIO_Port, POWER_OUTAGE_Pin) == GPIO_PIN_RESET || powerLostFlag)
    {
        // 15 ms debounce filter to confirm genuine outage rather than transient noise
        HAL_Delay(15);
        if (HAL_GPIO_ReadPin(POWER_OUTAGE_GPIO_Port, POWER_OUTAGE_Pin) != GPIO_PIN_RESET)
        {
            powerLostFlag = 0;
            return; // Main power is still present, ignore transient
        }

        powerLostFlag = 0;

        /* =====================================================================
         *         PREPARATION FOR STOP MODE (SYSTEM POWER SHUTDOWN)
         * ===================================================================== */

        // 1. Turn off display transistors to eliminate PNP base currents
        Display_Off();

        // 2. Stop multiplexing timer interrupts
        if (h_tim != NULL)
        {
            HAL_TIM_Base_Stop_IT(h_tim);
        }

        // 3. Stop ADC DMA and power off analog references (TSEN and VREFINT)
        Internal_Temp_App_Stop();

        // 4. Deactivate RTC Alarm A to prevent periodic wake-ups every second
        // (The hardware RTC calendar continues ticking accurately on LSE 32.768 kHz)
        if (h_rtc != NULL)
        {
            HAL_RTC_DeactivateAlarm(h_rtc, RTC_ALARM_A);
        }

        // 5. Isolate all display and SPI GPIO pins into Analog mode (High-Z)
        // to prevent parasitic leakage currents through base pull-up resistors
        Display_Pins_Isolate_LowPower();

        // 6. Suspend SysTick timer to avoid waking CPU every 1 ms
        HAL_SuspendTick();

        // 7. Ultra-low-power loop: as long as external power remains absent,
        // keep CPU in STOP mode. If spurious wake-up occurs, immediately re-enter STOP.
        while (HAL_GPIO_ReadPin(POWER_OUTAGE_GPIO_Port, POWER_OUTAGE_Pin) == GPIO_PIN_RESET)
        {
            HAL_PWR_EnterSTOPMode(PWR_MAINREGULATOR_ON, PWR_STOPENTRY_WFI);
        }

        /* =====================================================================
         * -- CPU RESUMES HERE IMMEDIATELY WHEN MAIN EXTERNAL POWER IS RESTORED --
         * ===================================================================== */

        // 8. Re-configure oscillator and system PLL / clock back to 48 MHz
        SystemClock_Config();

        // 9. Resume SysTick interrupt
        HAL_ResumeTick();

        // 10. Restore display and SPI GPIO pins to Push-Pull outputs
        Display_Pins_Restore_Normal();

        // 11. Reactivate ADC peripheral clock, analog paths, and DMA conversions
        Internal_Temp_App_Resume();

        // 12. Reactivate RTC Alarm A
        if (h_rtc != NULL)
        {
            RTC_AlarmTypeDef sAlarm = {0};
            sAlarm.AlarmMask = RTC_ALARMMASK_ALL;
            sAlarm.Alarm = RTC_ALARM_A;
            HAL_RTC_SetAlarm_IT(h_rtc, &sAlarm, RTC_FORMAT_BCD);
        }

        // 13. Resume display multiplexing timer
        if (h_tim != NULL)
        {
            HAL_TIM_Base_Start_IT(h_tim);
        }
    }
}

/* ========================================================================== */
/*                      HAL CALLBACK OVERRIDES                                */
/* ========================================================================== */

/**
  * @brief  Period elapsed callback in non-blocking mode.
  * @param  htim: Pointer to TIM handle that triggered the interrupt.
  * @retval None
  */
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
    if ((h_tim != NULL) && (htim->Instance == h_tim->Instance))
    {
        Clock_App_Multiplex_ISR();
    }
}

/**
  * @brief  RTC Alarm A event callback.
  * @param  hrtc: Pointer to RTC handle.
  * @retval None
  */
void HAL_RTC_AlarmAEventCallback(RTC_HandleTypeDef *hrtc)
{
    Clock_App_Alarm_ISR();
}

/**
  * @brief  EXTI line detection callback.
  * @param  GPIO_Pin: Specifies the pins connected to the EXTI line.
  * @retval None
  */
void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
{
    return;
}

/**
  * @brief  EXTI falling edge detection callback.
  * @param  GPIO_Pin: Specifies the pins connected to the EXTI line.
  * @retval None
  */
void HAL_GPIO_EXTI_Falling_Callback(uint16_t GPIO_Pin)
{
    if (GPIO_Pin == POWER_OUTAGE_Pin)
    {
        Display_Off();
        powerLostFlag = 1;
    }
}