/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2026 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "main.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "TLC5917.h"
#include "cmsis_gcc.h"
#include <stdint.h>
#include <stdio.h>
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */
typedef enum
{
    MODE_NORMAL,
    MODE_SET_HOURS,
    MODE_SET_MINUTES,
    MODE_READ_TEMPERATURE
} ClockMode;
/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
#define ARRAY_SIZE 10

#define TS_CAL1_ADDR        ((uint16_t*)0x1FFFF7B8)
#define TS_CAL2_ADDR        ((uint16_t*)0x1FFFF7C2)
#define VREFINT_CAL_ADDR    ((uint16_t*)0x1FFFF7BA)
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
ADC_HandleTypeDef hadc1;
DMA_HandleTypeDef hdma_adc1;

RTC_HandleTypeDef hrtc;

SPI_HandleTypeDef hspi1;

TIM_HandleTypeDef htim3;

UART_HandleTypeDef huart2;

/* USER CODE BEGIN PV */
TLC5917_HandleTypeDef tlc5917;

const uint8_t digitsArray[ARRAY_SIZE] = {0x3F, 0x06, 0x5B, 0x4F, 0x66, 0x6D, 0x7D, 0x07, 0x7F, 0x6F};

static volatile uint8_t thousand = 0x00; // Dígito 3 (Decenas de hora)
static volatile uint8_t hundred  = 0x00; // Dígito 2 (Unidades de hora)
static volatile uint8_t tens     = 0x00; // Dígito 1 (Decenas de minuto)
static volatile uint8_t unit     = 0x00; // Dígito 0 (Unidades de minuto)

static volatile uint8_t digitPin = 0;   // Índice de multiplexación
static volatile _Bool colonBlink = 0;   // Flag de parpadeo controlado por Alarma A

/* Variables de la máquina de estados del reloj */
static ClockMode currentMode = MODE_NORMAL;
static uint8_t setHours = 12;
static uint8_t setMinutes = 0;

// Arreglo para almacenar las lecturas de los 2 canales configurados (Rank 1 y Rank 2)
static uint16_t adcBuffer[2];

static volatile uint8_t isConversionCompleted = 0;

static double temperature = 0.0;
static uint32_t lastCycleStart = 0;
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_DMA_Init(void);
static void MX_RTC_Init(void);
static void MX_SPI1_Init(void);
static void MX_TIM3_Init(void);
static void MX_USART2_UART_Init(void);
static void MX_ADC1_Init(void);
/* USER CODE BEGIN PFP */
void Display_Off(void);
void RTC_Set_Time(uint8_t hours, uint8_t minutes, uint8_t seconds);
void Update_Display_Segments(void);
void Handle_Button_Setting(void);
void Read_Internal_Temp(void);
/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{

  /* USER CODE BEGIN 1 */

  /* USER CODE END 1 */

  /* MCU Configuration--------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();

  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();

  /* USER CODE BEGIN SysInit */

  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_DMA_Init();
  MX_RTC_Init();
  MX_SPI1_Init();
  MX_TIM3_Init();
  MX_USART2_UART_Init();
  MX_ADC1_Init();
  /* USER CODE BEGIN 2 */
  Display_Off();
  TLC5917_Init(&tlc5917, &hspi1, LE_GPIO_Port, LE_Pin);

  HAL_TIM_Base_Start_IT(&htim3);

  HAL_ADCEx_Calibration_Start(&hadc1, ADC_SINGLE_ENDED);
  HAL_ADC_Start_DMA(&hadc1, (uint32_t*)adcBuffer, 2);
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
    __WFI(); // Entra en modo sleep de bajo consumo, despierta con TIM3 (5ms) o RTC Alarm (1s)
    /* 1. Procesar el botón y los segmentos */
    Handle_Button_Setting();
    Update_Display_Segments();
    Read_Internal_Temp();
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
  }
  /* USER CODE END 3 */
}

/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};
  RCC_PeriphCLKInitTypeDef PeriphClkInit = {0};

  /** Configure LSE Drive Capability
  */
  HAL_PWR_EnableBkUpAccess();
  __HAL_RCC_LSEDRIVE_CONFIG(RCC_LSEDRIVE_LOW);

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI|RCC_OSCILLATORTYPE_LSE;
  RCC_OscInitStruct.LSEState = RCC_LSE_ON;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSI;
  RCC_OscInitStruct.PLL.PLLMUL = RCC_PLL_MUL9;
  RCC_OscInitStruct.PLL.PREDIV = RCC_PREDIV_DIV1;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV2;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_2) != HAL_OK)
  {
    Error_Handler();
  }
  PeriphClkInit.PeriphClockSelection = RCC_PERIPHCLK_USART2|RCC_PERIPHCLK_RTC
                              |RCC_PERIPHCLK_ADC12|RCC_PERIPHCLK_TIM34;
  PeriphClkInit.Usart2ClockSelection = RCC_USART2CLKSOURCE_PCLK1;
  PeriphClkInit.Adc12ClockSelection = RCC_ADC12PLLCLK_DIV1;
  PeriphClkInit.RTCClockSelection = RCC_RTCCLKSOURCE_LSE;
  PeriphClkInit.Tim34ClockSelection = RCC_TIM34CLK_HCLK;
  if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInit) != HAL_OK)
  {
    Error_Handler();
  }
}

/**
  * @brief ADC1 Initialization Function
  * @param None
  * @retval None
  */
static void MX_ADC1_Init(void)
{

  /* USER CODE BEGIN ADC1_Init 0 */

  /* USER CODE END ADC1_Init 0 */

  ADC_MultiModeTypeDef multimode = {0};
  ADC_ChannelConfTypeDef sConfig = {0};

  /* USER CODE BEGIN ADC1_Init 1 */

  /* USER CODE END ADC1_Init 1 */

  /** Common config
  */
  hadc1.Instance = ADC1;
  hadc1.Init.ClockPrescaler = ADC_CLOCK_ASYNC_DIV1;
  hadc1.Init.Resolution = ADC_RESOLUTION_12B;
  hadc1.Init.ScanConvMode = ADC_SCAN_ENABLE;
  hadc1.Init.ContinuousConvMode = ENABLE;
  hadc1.Init.DiscontinuousConvMode = DISABLE;
  hadc1.Init.ExternalTrigConvEdge = ADC_EXTERNALTRIGCONVEDGE_NONE;
  hadc1.Init.ExternalTrigConv = ADC_SOFTWARE_START;
  hadc1.Init.DataAlign = ADC_DATAALIGN_RIGHT;
  hadc1.Init.NbrOfConversion = 2;
  hadc1.Init.DMAContinuousRequests = ENABLE;
  hadc1.Init.EOCSelection = ADC_EOC_SINGLE_CONV;
  hadc1.Init.LowPowerAutoWait = DISABLE;
  hadc1.Init.Overrun = ADC_OVR_DATA_OVERWRITTEN;
  if (HAL_ADC_Init(&hadc1) != HAL_OK)
  {
    Error_Handler();
  }

  /** Configure the ADC multi-mode
  */
  multimode.Mode = ADC_MODE_INDEPENDENT;
  if (HAL_ADCEx_MultiModeConfigChannel(&hadc1, &multimode) != HAL_OK)
  {
    Error_Handler();
  }

  /** Configure Regular Channel
  */
  sConfig.Channel = ADC_CHANNEL_TEMPSENSOR;
  sConfig.Rank = ADC_REGULAR_RANK_1;
  sConfig.SingleDiff = ADC_SINGLE_ENDED;
  sConfig.SamplingTime = ADC_SAMPLETIME_601CYCLES_5;
  sConfig.OffsetNumber = ADC_OFFSET_NONE;
  sConfig.Offset = 0;
  if (HAL_ADC_ConfigChannel(&hadc1, &sConfig) != HAL_OK)
  {
    Error_Handler();
  }

  /** Configure Regular Channel
  */
  sConfig.Channel = ADC_CHANNEL_VREFINT;
  sConfig.Rank = ADC_REGULAR_RANK_2;
  if (HAL_ADC_ConfigChannel(&hadc1, &sConfig) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN ADC1_Init 2 */

  /* USER CODE END ADC1_Init 2 */

}

/**
  * @brief RTC Initialization Function
  * @param None
  * @retval None
  */
static void MX_RTC_Init(void)
{

  /* USER CODE BEGIN RTC_Init 0 */

  /* USER CODE END RTC_Init 0 */

  RTC_TimeTypeDef sTime = {0};
  RTC_DateTypeDef sDate = {0};
  RTC_AlarmTypeDef sAlarm = {0};

  /* USER CODE BEGIN RTC_Init 1 */

  /* USER CODE END RTC_Init 1 */

  /** Initialize RTC Only
  */
  hrtc.Instance = RTC;
  hrtc.Init.HourFormat = RTC_HOURFORMAT_24;
  hrtc.Init.AsynchPrediv = 127;
  hrtc.Init.SynchPrediv = 255;
  hrtc.Init.OutPut = RTC_OUTPUT_DISABLE;
  hrtc.Init.OutPutPolarity = RTC_OUTPUT_POLARITY_HIGH;
  hrtc.Init.OutPutType = RTC_OUTPUT_TYPE_OPENDRAIN;
  if (HAL_RTC_Init(&hrtc) != HAL_OK)
  {
    Error_Handler();
  }

  /* USER CODE BEGIN Check_RTC_BKUP */

  /* USER CODE END Check_RTC_BKUP */

  /** Initialize RTC and set the Time and Date
  */
  sTime.Hours = 0x0;
  sTime.Minutes = 0x0;
  sTime.Seconds = 0x0;
  sTime.DayLightSaving = RTC_DAYLIGHTSAVING_NONE;
  sTime.StoreOperation = RTC_STOREOPERATION_RESET;
  if (HAL_RTC_SetTime(&hrtc, &sTime, RTC_FORMAT_BCD) != HAL_OK)
  {
    Error_Handler();
  }
  sDate.WeekDay = RTC_WEEKDAY_MONDAY;
  sDate.Month = RTC_MONTH_JANUARY;
  sDate.Date = 0x1;
  sDate.Year = 0x0;

  if (HAL_RTC_SetDate(&hrtc, &sDate, RTC_FORMAT_BCD) != HAL_OK)
  {
    Error_Handler();
  }

  /** Enable the Alarm A
  */
  sAlarm.AlarmTime.Hours = 0x0;
  sAlarm.AlarmTime.Minutes = 0x0;
  sAlarm.AlarmTime.Seconds = 0x0;
  sAlarm.AlarmTime.SubSeconds = 0x0;
  sAlarm.AlarmTime.DayLightSaving = RTC_DAYLIGHTSAVING_NONE;
  sAlarm.AlarmTime.StoreOperation = RTC_STOREOPERATION_RESET;
  sAlarm.AlarmMask = RTC_ALARMMASK_ALL;
  sAlarm.AlarmSubSecondMask = RTC_ALARMSUBSECONDMASK_ALL;
  sAlarm.AlarmDateWeekDaySel = RTC_ALARMDATEWEEKDAYSEL_DATE;
  sAlarm.AlarmDateWeekDay = 0x1;
  sAlarm.Alarm = RTC_ALARM_A;
  if (HAL_RTC_SetAlarm_IT(&hrtc, &sAlarm, RTC_FORMAT_BCD) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN RTC_Init 2 */

  /* USER CODE END RTC_Init 2 */

}

/**
  * @brief SPI1 Initialization Function
  * @param None
  * @retval None
  */
static void MX_SPI1_Init(void)
{

  /* USER CODE BEGIN SPI1_Init 0 */

  /* USER CODE END SPI1_Init 0 */

  /* USER CODE BEGIN SPI1_Init 1 */

  /* USER CODE END SPI1_Init 1 */
  /* SPI1 parameter configuration*/
  hspi1.Instance = SPI1;
  hspi1.Init.Mode = SPI_MODE_MASTER;
  hspi1.Init.Direction = SPI_DIRECTION_1LINE;
  hspi1.Init.DataSize = SPI_DATASIZE_8BIT;
  hspi1.Init.CLKPolarity = SPI_POLARITY_LOW;
  hspi1.Init.CLKPhase = SPI_PHASE_1EDGE;
  hspi1.Init.NSS = SPI_NSS_SOFT;
  hspi1.Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_16;
  hspi1.Init.FirstBit = SPI_FIRSTBIT_MSB;
  hspi1.Init.TIMode = SPI_TIMODE_DISABLE;
  hspi1.Init.CRCCalculation = SPI_CRCCALCULATION_DISABLE;
  hspi1.Init.CRCPolynomial = 7;
  hspi1.Init.CRCLength = SPI_CRC_LENGTH_DATASIZE;
  hspi1.Init.NSSPMode = SPI_NSS_PULSE_ENABLE;
  if (HAL_SPI_Init(&hspi1) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN SPI1_Init 2 */

  /* USER CODE END SPI1_Init 2 */

}

/**
  * @brief TIM3 Initialization Function
  * @param None
  * @retval None
  */
static void MX_TIM3_Init(void)
{

  /* USER CODE BEGIN TIM3_Init 0 */

  /* USER CODE END TIM3_Init 0 */

  TIM_ClockConfigTypeDef sClockSourceConfig = {0};
  TIM_MasterConfigTypeDef sMasterConfig = {0};

  /* USER CODE BEGIN TIM3_Init 1 */

  /* USER CODE END TIM3_Init 1 */
  htim3.Instance = TIM3;
  htim3.Init.Prescaler = 1000-1;
  htim3.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim3.Init.Period = 360-1;
  htim3.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
  htim3.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
  if (HAL_TIM_Base_Init(&htim3) != HAL_OK)
  {
    Error_Handler();
  }
  sClockSourceConfig.ClockSource = TIM_CLOCKSOURCE_INTERNAL;
  if (HAL_TIM_ConfigClockSource(&htim3, &sClockSourceConfig) != HAL_OK)
  {
    Error_Handler();
  }
  sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
  sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
  if (HAL_TIMEx_MasterConfigSynchronization(&htim3, &sMasterConfig) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN TIM3_Init 2 */

  /* USER CODE END TIM3_Init 2 */

}

/**
  * @brief USART2 Initialization Function
  * @param None
  * @retval None
  */
static void MX_USART2_UART_Init(void)
{

  /* USER CODE BEGIN USART2_Init 0 */

  /* USER CODE END USART2_Init 0 */

  /* USER CODE BEGIN USART2_Init 1 */

  /* USER CODE END USART2_Init 1 */
  huart2.Instance = USART2;
  huart2.Init.BaudRate = 38400;
  huart2.Init.WordLength = UART_WORDLENGTH_8B;
  huart2.Init.StopBits = UART_STOPBITS_1;
  huart2.Init.Parity = UART_PARITY_NONE;
  huart2.Init.Mode = UART_MODE_TX_RX;
  huart2.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart2.Init.OverSampling = UART_OVERSAMPLING_16;
  huart2.Init.OneBitSampling = UART_ONE_BIT_SAMPLE_DISABLE;
  huart2.AdvancedInit.AdvFeatureInit = UART_ADVFEATURE_NO_INIT;
  if (HAL_UART_Init(&huart2) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN USART2_Init 2 */

  /* USER CODE END USART2_Init 2 */

}

/**
  * Enable DMA controller clock
  */
static void MX_DMA_Init(void)
{

  /* DMA controller clock enable */
  __HAL_RCC_DMA1_CLK_ENABLE();

  /* DMA interrupt init */
  /* DMA1_Channel1_IRQn interrupt configuration */
  HAL_NVIC_SetPriority(DMA1_Channel1_IRQn, 0, 0);
  HAL_NVIC_EnableIRQ(DMA1_Channel1_IRQn);

}

/**
  * @brief GPIO Initialization Function
  * @param None
  * @retval None
  */
static void MX_GPIO_Init(void)
{
  GPIO_InitTypeDef GPIO_InitStruct = {0};
  /* USER CODE BEGIN MX_GPIO_Init_1 */

  /* USER CODE END MX_GPIO_Init_1 */

  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOC_CLK_ENABLE();
  __HAL_RCC_GPIOF_CLK_ENABLE();
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOC, TENS_Pin|LE_Pin|UNIT_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOB, THOU_Pin|COLON_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(HUND_GPIO_Port, HUND_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin : B1_Pin */
  GPIO_InitStruct.Pin = B1_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_IT_FALLING;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(B1_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pins : TENS_Pin LE_Pin UNIT_Pin */
  GPIO_InitStruct.Pin = TENS_Pin|LE_Pin|UNIT_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);

  /*Configure GPIO pins : THOU_Pin COLON_Pin */
  GPIO_InitStruct.Pin = THOU_Pin|COLON_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

  /*Configure GPIO pin : HUND_Pin */
  GPIO_InitStruct.Pin = HUND_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(HUND_GPIO_Port, &GPIO_InitStruct);

  /* USER CODE BEGIN MX_GPIO_Init_2 */

  /* USER CODE END MX_GPIO_Init_2 */
}

/* USER CODE BEGIN 4 */
void Display_Off(void)
{
  HAL_GPIO_WritePin(UNIT_GPIO_Port, UNIT_Pin, GPIO_PIN_SET);
  HAL_GPIO_WritePin(TENS_GPIO_Port, TENS_Pin, GPIO_PIN_SET);
  HAL_GPIO_WritePin(HUND_GPIO_Port, HUND_Pin, GPIO_PIN_SET);
  HAL_GPIO_WritePin(THOU_GPIO_Port, THOU_Pin, GPIO_PIN_SET);
}

/* --- Actualización lógica de los segmentos --- */
void Update_Display_Segments(void)
{
  uint32_t currentTick = HAL_GetTick();
  static uint8_t displayTemperature = 0;    // Almacena la medición congelada
  static _Bool tempSnapshotTaken = 0;   // Bandera para tomar una sola muestra

  /* Alternar automáticamente entre Hora y Temperatura (Ciclo de 50s total: 45s hora, 5s temp) */
  if (currentMode == MODE_NORMAL || currentMode == MODE_READ_TEMPERATURE)
  {
    uint32_t elapsed = currentTick - lastCycleStart;
    if (elapsed >= 50000) // (1ms * 50000 = 50s)
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

  if (currentMode == MODE_NORMAL)
  {
    tempSnapshotTaken = 0; // Liberar bandera para la siguiente medición
    
    RTC_TimeTypeDef sTime = {0};
    RTC_DateTypeDef sDate = {0};
    
    // Lectura del RTC (GetTime debe llamarse antes de GetDate)
    HAL_RTC_GetTime(&hrtc, &sTime, RTC_FORMAT_BIN);
    HAL_RTC_GetDate(&hrtc, &sDate, RTC_FORMAT_BIN);
    // Los dos puntos parpadean según el flag de la alarma
    uint8_t colonMask = colonBlink ? 0x80 : 0x00;
    thousand = digitsArray[sTime.Hours / 10] | colonMask;
    hundred  = digitsArray[sTime.Hours % 10] | colonMask;
    tens     = digitsArray[sTime.Minutes / 10] | colonMask;
    unit     = digitsArray[sTime.Minutes % 10] | colonMask;
  }
  else if (currentMode == MODE_READ_TEMPERATURE)
  {
    // Capturar la temperatura una sola vez al inicio de los 5 segundos (Sample and Hold)
    if (!tempSnapshotTaken)
    {
      displayTemperature = (uint8_t)temperature;
      tempSnapshotTaken = 1;
    }

    uint8_t tempInt = displayTemperature;
    
    // El dígito 0 (unidad) siempre muestra la letra 'C' (patrón 0x39)
    unit = 0x39;
    
    if (tempInt < 0)
    {
      int absTemp = -tempInt;
      thousand = 0x40; // Signo menos '-' en el dígito de millares
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
    // Parpadeo rápido (4Hz / 250ms) en la sección de horas, minutos estables, ":" encendidos
    _Bool blinkOn = (HAL_GetTick() / 250) % 2 == 0;
    thousand = blinkOn ? (digitsArray[setHours / 10] | 0x80) : (0x00 | 0x80);
    hundred  = blinkOn ? (digitsArray[setHours % 10] | 0x80) : (0x00 | 0x80);
    tens     = digitsArray[setMinutes / 10] | 0x80;
    unit     = digitsArray[setMinutes % 10] | 0x80;
  }
  else if (currentMode == MODE_SET_MINUTES)
  {
    // Parpadeo rápido (4Hz / 250ms) en la sección de minutos, horas estables, ":" encendidos
    _Bool blinkOn = (HAL_GetTick() / 250) % 2 == 0;
    thousand = digitsArray[setHours / 10] | 0x80;
    hundred  = digitsArray[setHours % 10] | 0x80;
    tens     = blinkOn ? (digitsArray[setMinutes / 10] | 0x80) : (0x00 | 0x80);
    unit     = blinkOn ? (digitsArray[setMinutes % 10] | 0x80) : (0x00 | 0x80);
  }
}
/* --- Máquina de Estados para Configuración con Un Botón (B1) --- */
void Handle_Button_Setting(void)
{
  static uint32_t buttonPressStart = 0;
  static _Bool buttonWasPressed = 0;
  // Botón B1 en tarjeta Nucleo es activo en bajo (Pull-Up externo)
  _Bool buttonIsPressed = (HAL_GPIO_ReadPin(B1_GPIO_Port, B1_Pin) == GPIO_PIN_RESET);
  if (buttonIsPressed && !buttonWasPressed)
  {
    // Transición de reposo a presionado (flanco de bajada)
    buttonPressStart = HAL_GetTick();
    buttonWasPressed = 1;
  }
  else if (!buttonIsPressed && buttonWasPressed)
  {
    // Transición de presionado a liberado (flanco de subida)
    uint32_t pressDuration = HAL_GetTick() - buttonPressStart;
    buttonWasPressed = 0;
    // Pulsación corta (entre 50 ms y 1.5 segundos)
    if (pressDuration >= 50 && pressDuration < 1500)
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
  }
  else if (buttonIsPressed && buttonWasPressed)
  {
    // El botón se mantiene presionado. Evaluar pulsación larga (2 segundos o más)
    uint32_t pressDuration = HAL_GetTick() - buttonPressStart;
    if (pressDuration >= 2000)
    {
      // Limpiar bandera para requerir soltar el botón antes de la próxima acción
      buttonWasPressed = 0; 
      if (currentMode == MODE_NORMAL)
      {
        // Entrar a configuración de Horas y cargar la hora actual
        RTC_TimeTypeDef sTime = {0};
        HAL_RTC_GetTime(&hrtc, &sTime, RTC_FORMAT_BIN);
        setHours = sTime.Hours;
        setMinutes = sTime.Minutes;
        currentMode = MODE_SET_HOURS;
      }
      else if (currentMode == MODE_SET_HOURS)
      {
        // Pasar a configuración de Minutos
        currentMode = MODE_SET_MINUTES;
      }
      else if (currentMode == MODE_SET_MINUTES)
      {
        // Guardar valores modificados en el RTC y volver a Modo Normal
        RTC_Set_Time(setHours, setMinutes, 0);
        currentMode = MODE_NORMAL;
        lastCycleStart = HAL_GetTick(); // Reiniciar el ciclo del display
      }
    }
  }
}
/* --- Callback de Interrupción de Alarma A (Cada 1 segundo) --- */
void HAL_RTC_AlarmAEventCallback(RTC_HandleTypeDef *hrtc)
{
  // Conmuta el estado de los dos puntos del reloj
  colonBlink = !colonBlink;
}
/* --- Rutina de Interrupción del Temporizador de Multiplexado (TIM3 - 5 ms) --- */
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
  if (htim->Instance == TIM3)
  {
    // 1. BLANKING: Apaga todos los transistores (Pines en HIGH)
    Display_Off();
    // 2. Cargar datos en TLC5917 y habilitar el transistor correspondiente (Pin en LOW)
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
    // Incrementar e indexar el multiplexor
    digitPin++;
    if (digitPin >= 4) {
      digitPin = 0;
    }
  }
}

/* --- Configurar hora en el RTC --- */
void RTC_Set_Time(uint8_t hours, uint8_t minutes, uint8_t seconds)
{
  RTC_TimeTypeDef sTime = {0};
  sTime.Hours = hours;
  sTime.Minutes = minutes;
  sTime.Seconds = seconds;
  sTime.DayLightSaving = RTC_DAYLIGHTSAVING_NONE;
  sTime.StoreOperation = RTC_STOREOPERATION_RESET;
  
  if (HAL_RTC_SetTime(&hrtc, &sTime, RTC_FORMAT_BIN) != HAL_OK)
  {
    Error_Handler();
  }
}

void Read_Internal_Temp(void)
{
    if (isConversionCompleted)
    {
        isConversionCompleted = 0;

        uint16_t rawTemp = adcBuffer[0];
        uint16_t rawVref = adcBuffer[1];

        uint16_t tsCal1 = *TS_CAL1_ADDR;
        uint16_t tsCal2 = *TS_CAL2_ADDR;
        uint16_t VrefintCal = *VREFINT_CAL_ADDR;

        if (rawVref > 0)
        {
            // Compensar el valor analógico leído del sensor de temperatura por variaciones de VDDA
            double adcTempScaled = (double)rawTemp * ((double)VrefintCal / (double)rawVref);

            // Calcular la temperatura real mediante la interpolación lineal (30 °C a 110 °C)
            temperature = ((110.0 - 30.0) / (double)(tsCal2 - tsCal1)) * (adcTempScaled - (double)tsCal1) + 30.0;

        }
    }
}

void HAL_ADC_ConvCpltCallback(ADC_HandleTypeDef *hadc)
{
    if (hadc->Instance == ADC1)
    {
        isConversionCompleted = 1;
    }
}
/* USER CODE END 4 */

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */
  __disable_irq();
  while (1)
  {
  }
  /* USER CODE END Error_Handler_Debug */
}
#ifdef USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t *file, uint32_t line)
{
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */
