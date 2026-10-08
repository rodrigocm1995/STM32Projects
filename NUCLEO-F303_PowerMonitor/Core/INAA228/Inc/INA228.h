/**
  *******************************************************************************************
  * @file           : INA228.h
  * @brief          : Header file for INA228 20-bit Ultra-Precise Digital Power Monitor Driver
  *******************************************************************************************
  * @attention
  *
  * The INA228 is an ultra-precise digital power monitor with a 20-bit delta-sigma ADC specifi-
  * cally designed for current-sensing applications. The device can measure a full-scale diffe-
  * rential input of ±163.84 mV or ±40.96 mV across a resistive shunt sense element with commo-
  * n mode voltage support from -0.3 V to +85 V.
  * 
  * The INA228 reports current, bus voltage, temperature, power, energy and charge accumulation
  * while employing a precision ±0.5% integrated oscillator, all while performing the needed c-
  * alculations in the background.
  * 
  *******************************************************************************************
  */

#ifndef INC_INA228_H_
#define INC_INA228_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "main.h"

#define INA228_ADDRESS                            0x40
#define INA228_TRIALS                             5
#define CHECK_BIT(var,pos) ((var) & (1<<(pos)))

/* Registers Definition */
#define INA228_CONFIG_REG                         0x00 /* 16 Bits */
#define INA228_ADC_CONFIG_REG                     0x01 /* 16 Bits */
#define INA228_SHUNT_CAL_REG                      0x02 /* 16 Bits */
#define INA228_SHUNT_TEMPCOEF_REG                 0x03 /* 16 Bits */
#define INA228_VSHUNT_REG                         0x04 /* 24 Bits */
#define INA228_VBUS_REG                           0x05 /* 24 Bits */
#define INA228_DIETEMP_REG                        0x06 /* 16 Bits */
#define INA228_CURRENT_REG                        0x07 /* 24 Bits */
#define INA228_POWER_REG                          0x08 /* 24 Bits */
#define INA228_ENERGY_REG                         0x09 /* 40 Bits */
#define INA228_CHARGE_REG                         0x0A /* 40 Bits */
#define INA228_DIAG_ALERT_REG                     0x0B /* 16 Bits */
#define INA228_SOVL_REG                           0x0C /* 16 Bits */
#define INA228_SUVL_REG                           0x0D /* 16 Bits */
#define INA228_BOVL_REG                           0x0E /* 16 Bits */
#define INA228_BUVL_REG                           0x0F /* 16 Bits */
#define INA228_TEMP_LIMIT_REG                     0x10 /* 16 Bits */
#define INA228_POWER_LIMIT_REG                    0x11 /* 16 Bits */
#define INA228_MANUFACTURER_ID_REG                0x3E /* 16 Bits */
#define INA228_DEVICE_ID_REG                      0x3F /* 16 Bits */

/******************* Bits definition for CONFIGURATION register ******************/
#define INA228_ADCRANGE_Pos                       (4U)
#define INA228_ADCRANGE_Mask                      (0x1U << INA228_ADCRANGE_Pos)
#define INA228_ADCRANGE                           INA228_ADCRANGE_Mask

#define INA228_TEMPCOMP_Pos                       (5U)
#define INA228_TEMPCOMP_Mask                      (0x1U << INA228_TEMPCOMP_Pos)
#define INA228_TEMPCOMP                           INA228_TEMPCOMP_Mask

#define INA228_CONVDLY_Pos                        (6U)
#define INA228_CONVDLY_Mask                       (0xFFU << INA228_CONVDLY_Pos)
#define INA228_CONVDLY                            INA228_CONVDLY_Mask

#define INA228_RSTACC_Pos                         (14U)
#define INA228_RSTACC_Mask                        (0x1U << INA228_RSTACC_Pos)
#define INA228_RSTACC                             INA228_RSTACC_Mask

#define INA228_RST_Pos                            (15U)
#define INA228_RST_Mask                           (0x1U << INA228_RST_Pos)
#define INA228_RST                                INA228_RST_Mask

/******************* Bits definition for ADC_CONFIG register ******************/
#define INA228_AVG_Pos                            (0U)
#define INA228_AVG_Mask                           (0x7U << INA228_AVG_Pos)
#define INA228_AVG                                INA228_AVG_Mask

#define INA228_VTCT_Pos                           (3U)
#define INA228_VTCT_Mask                          (0x7U << INA228_VTCT_Pos)
#define INA228_VTCT                               INA228_VTCT_Mask

#define INA228_VSHCT_Pos                          (6U)
#define INA228_VSHCT_Mask                         (0x7U << INA228_VSHCT_Pos)
#define INA228_VSHCT                              INA228_VSHCT_Mask

#define INA228_VBUSCT_Pos                         (9U)
#define INA228_VBUSCT_Mask                        (0x7U << INA228_VBUSCT_Pos)
#define INA228_VBUSCT                             INA228_VBUSCT_Mask

#define INA228_MODE_Pos                           (12U)
#define INA228_MODE_Mask                          (0xFU << INA228_MODE_Pos)
#define INA228_MODE                               INA228_MODE_Mask

/******************* Bits definition for DIAG_ALRT register ******************/
#define INA228_MEMSTAT_Pos                        (0U)
#define INA228_MEMSTAT_Mask                       (0x1U << INA228_MEMSTAT_Pos)
#define INA228_MEMSTAT                            INA228_MEMSTAT_Mask

#define INA228_CNVRF_Pos                          (1U)
#define INA228_CNVRF_Mask                         (0x1U << INA228_CNVRF_Pos)
#define INA228_CNVRF                              INA228_CNVRF_Mask

#define INA228_POL_Pos                            (2U)
#define INA228_POL_Mask                           (0x1U << INA228_POL_Pos)
#define INA228_POL                                INA228_POL_Mask

#define INA228_BUSUL_Pos                          (3U)
#define INA228_BUSUL_Mask                         (0x1U << INA228_BUSUL_Pos)
#define INA228_BUSUL                              INA228_BUSUL_Mask

#define INA228_BUSOL_Pos                          (4U)
#define INA228_BUSOL_Mask                         (0x1U << INA228_BUSOL_Pos)
#define INA228_BUSOL                              INA228_BUSOL_Mask

#define INA228_SHUNTUL_Pos                        (5U)
#define INA228_SHUNTUL_Mask                       (0x1U << INA228_SHUNTUL_Pos)
#define INA228_SHUNTUL                            INA228_SHUNTUL_Mask

#define INA228_SHUNTOL_Pos                        (6U)
#define INA228_SHUNTOL_Mask                       (0x1U << INA228_SHUNTOL_Pos)
#define INA228_SHUNTOL                            INA228_SHUNTOL_Mask

#define INA228_TMPOL_Pos                          (7U)
#define INA228_TMPOL_Mask                         (0x1U << INA228_TMPOL_Pos)
#define INA228_TMPOL                              INA228_TMPOL_Mask

#define INA228_MATHOF_Pos                         (9U)
#define INA228_MATHOF_Mask                        (0x1U << INA228_MATHOF_Pos)
#define INA228_MATHOF                             INA228_MATHOF_Mask

#define INA228_CHARGEOF_Pos                       (10U)
#define INA228_CHARGEOF_Mask                      (0x1U << INA228_CHARGEOF_Pos)
#define INA228_CHARGEOF                           INA228_CHARGEOF_Mask

#define INA228_ENERGYOF_Pos                       (11U)
#define INA228_ENERGYOF_Mask                      (0x1U << INA228_ENERGYOF_Pos)
#define INA228_ENERGYOF                           INA228_ENERGYOF_Mask

#define INA228_APOL_Pos                           (12U)
#define INA228_APOL_Mask                          (0x1U << INA228_APOL_Pos)
#define INA228_APOL                               INA228_APOL_Mask

#define INA228_SLOWALERT_Pos                      (13U)
#define INA228_SLOWALERT_Mask                     (0x1U << INA228_SLOWALERT_Pos)
#define INA228_SLOWALERT                          INA228_SLOWALERT_Mask

#define INA228_CNVR_Pos                           (14U)
#define INA228_CNVR_Mask                          (0x1U << INA228_CNVR_Pos)
#define INA228_CNVR                               INA228_CNVR_Mask

#define INA228_ALATCH_Pos                         (15U)
#define INA228_ALATCH_Mask                        (0x1U << INA228_ALATCH_Pos)
#define INA228_ALATCH                             INA228_ALATCH_Mask

/* Enums Definitions */
typedef enum
{
    INA228_ADC_RANGE_163_84_MV                  = 0x0U,
    INA228_ADC_RANGE_40_96_MV                   = 0x1U
} INA228_AdcRange_TypeDef;

typedef enum
{
    INA228_TEMP_COMP_DISABLED                   = 0x0U,
    INA228_TEMPERATURE_COMP_ENABLED             = 0x1U
} INA228_TempComp_TypeDef;

typedef enum
{
    INA228_DELAY_0_MS                           = 0x0U,
    INA228_DELAY_2_MS                           = 0x1U,
    INA228_DELAY_510_MS                         = 0xFFU
} INA228_ConvDelay_TypeDef;

typedef enum
{
    INA228_1_SAMPLE                             = 0x0U,
    INA228_4_SAMPLES                            = 0x1U,
    INA228_16_SAMPLES                           = 0x2U,
    INA228_64_SAMPLES                           = 0x3U,
    INA228_128_SAMPLES                          = 0x4U,
    INA228_256_SAMPLES                          = 0x5U,
    INA228_512_SAMPLES                          = 0x6U,
    INA228_1024_SAMPLES                         = 0x7U
} INA228_Average_TypeDef;

typedef enum
{
    INA228_50_US                                = 0x0U,
    INA228_84_US                                = 0x1U,
    INA228_150_US                               = 0x2U,
    INA228_280_US                               = 0x3U,
    INA228_540_US                               = 0x4U,
    INA228_1052_US                              = 0x5U,
    INA228_2074_US                              = 0x6U,
    INA228_4120_US                              = 0x7U
} INA228_ConvTime_TypeDef;

typedef enum
{
    INA228_SHUTDOWN_MODE                        = 0x0U,
    INA228_BUS_ONE_SHOT                         = 0x1U,
    INA228_SHUNT_ONE_SHOT                       = 0x2U,
    INA228_SHUNT_BUS_ONE_SHOT                   = 0x3U,
    INA228_TEMP_ONE_SHOT                        = 0x4U,
    INA228_TEMP_BUS_ONE_SHOT                    = 0x5U,
    INA228_TEMP_SHUNT_ONE_SHOT                  = 0x6U,
    INA228_TEMP_SHUNT_BUS_ONE_SHOT              = 0x7U,
    INA228_BUS_CONTINUOUS                       = 0x9U,
    INA228_SHUNT_CONTINUOUS                     = 0xAU,
    INA228_SHUNT_BUS_CONTINUOUS                 = 0xBU,
    INA228_TEMP_CONTINUOUS                      = 0xCU,
    INA228_TEMP_BUS_CONTINUOUS                  = 0xDU,
    INA228_TEMP_SHUNT_CONTINUOUS                = 0xEU,
    INA228_TEMP_SHUNT_BUS_CONTINUOUS            = 0xFU
} INA228_Mode_TypeDef;

typedef enum
{
    INA228_DISABLE_CNVR_FLAG_ON_ALERT_PIN       = 0x0U,
    INA228_ENABLE_CNVR_FLAG_ON_ALERT_PIN        = 0x1U
} INA228_CNVR_TypeDef;

typedef enum
{
    INA228_ALERT_ACTIVE_LOW                     = 0x0U,
    INA228_ALERT_ACTIVE_HIGH                    = 0x1U
} INA228_AlertPinPol_TypeDef;

typedef enum
{
    INA228_ALERT_COMPARISON_NON_AVERAGED        = 0x0U,
    INA228_ALERT_COMPARISON_AVERAGED            = 0x1U
} INA228_SlowAlert_TypeDef;

typedef enum
{
    INA228_ALERT_LATCH_TRANSPARENT              = 0x0U,
    INA228_ALERT_LATCH_ENABLED                  = 0x1U
} INA228_AlertLatch_TypeDef;

typedef enum
{
    INA228_ENERGY_REGISTER_NORMAL               = 0x0U,
    INA228_ENERGY_REGISTER_OVERFLOW             = 0x1U
} INA228_EnergyOverflow_TypeDef;

typedef enum
{
    INA228_CHARGE_REGISTER_NORMAL               = 0x0U,
    INA228_CHARGE_REGISTER_OVERFLOW             = 0x1U
} INA228_ChargeOverflow_TypeDef;

typedef enum
{
    INA228_MATH_NORMAL                          = 0x0U,
    INA228_MATH_OVERFLOW                        = 0x1U
} INA228_MathOverflow_TypeDef;

typedef enum
{
    INA228_TEMP_LIMIT_NORMAL                    = 0x0U,
    INA228_TEMP_OVER_LIMIT_EVENT                = 0x1U
} INA228_TempOverLimit_TypeDef;

typedef enum
{
    INA228_SHUNT_LIMIT_NORMAL                   = 0x0U,
    INA228_SHUNT_OVER_LIMIT_EVENT               = 0x1U
} INA228_ShuntOverLimit_TypeDef;

typedef enum
{
    INA228_SHUNT_UNDER_LIMIT_NORMAL             = 0x0U,
    INA228_SHUNT_UNDER_LIMIT_EVENT              = 0x1U
} INA228_ShuntUnderLimit_TypeDef;

typedef enum
{
    INA228_BUS_LIMIT_NORMAL                     = 0x0U,
    INA228_BUS_OVER_LIMIT_EVENT                 = 0x1U
} INA228_BusOverLimit_TypeDef;

typedef enum
{
    INA228_BUS_UNDER_LIMIT_NORMAL               = 0x0U,
    INA228_BUS_UNDER_LIMIT_EVENT                = 0x1U
} INA228_BusUnderLimit_TypeDef;

typedef enum
{
    INA228_POWER_LIMIT_NORMAL                   = 0x0U,
    INA228_POWER_OVER_LIMIT_EVENT               = 0x1U
} INA228_PowerOverLimit_TypeDef;

typedef enum
{
    INA228_CONVERSION_NOT_READY                 = 0x0U,
    INA228_CONVERSION_READY                     = 0x1U
} INA228_ConvReadyFlag_TypeDef;

typedef enum
{
    INA228_MEMORY_CHECKSUM_ERROR                = 0x0U,
    INA228_MEMORY_NORMAL_OPERATION              = 0x1U
} INA228_MemStatus_TypeDef;

/* Handle Structure */
typedef struct
{
    I2C_HandleTypeDef   *hi2c;
    uint8_t             _devAddress;
    uint8_t             _adcRange;
    double              _shuntAdcRange;
    double              _resolution;
    double              _shuntResistor;
    double              _maximumCurrent;
    double              _currentLsbMin;
    double              _currentLsb;
} INA228_HandleTypeDef;

/* Application Functions Prototypes */
HAL_StatusTypeDef INA228_Init(INA228_HandleTypeDef *ina228, I2C_HandleTypeDef *i2c, uint8_t devAddress);

/* Register Access Functions */
HAL_StatusTypeDef INA228_GetConfigurationReg(INA228_HandleTypeDef *ina228, uint16_t *value);
HAL_StatusTypeDef INA228_GetAdcConfigurationReg(INA228_HandleTypeDef *ina228, uint16_t *value);
HAL_StatusTypeDef INA228_GetShuntCalibrationReg(INA228_HandleTypeDef *ina228, uint16_t *value);
HAL_StatusTypeDef INA228_GetTempCoefficientReg(INA228_HandleTypeDef *ina228, uint16_t *value);
HAL_StatusTypeDef INA228_GetShuntVoltageReg(INA228_HandleTypeDef *ina228, uint32_t *value);
HAL_StatusTypeDef INA228_GetBusVoltageReg(INA228_HandleTypeDef *ina228, uint32_t *value);
HAL_StatusTypeDef INA228_GetDieTempReg(INA228_HandleTypeDef *ina228, uint16_t *value);
HAL_StatusTypeDef INA228_GetCurrentReg(INA228_HandleTypeDef *ina228, uint32_t *value);
HAL_StatusTypeDef INA228_GetPowerReg(INA228_HandleTypeDef *ina228, uint32_t *value);
HAL_StatusTypeDef INA228_GetEnergyReg(INA228_HandleTypeDef *ina228, uint64_t *value);
HAL_StatusTypeDef INA228_GetChargeReg(INA228_HandleTypeDef *ina228, uint64_t *value);
HAL_StatusTypeDef INA228_GetDiagAlertReg(INA228_HandleTypeDef *ina228, uint16_t *value);
HAL_StatusTypeDef INA228_SetSOVLReg(INA228_HandleTypeDef *ina228, uint16_t value);
HAL_StatusTypeDef INA228_GetSOVLReg(INA228_HandleTypeDef *ina228, uint16_t *value);
HAL_StatusTypeDef INA228_SetSUVLReg(INA228_HandleTypeDef *ina228, uint16_t value);
HAL_StatusTypeDef INA228_GetSUVLReg(INA228_HandleTypeDef *ina228, uint16_t *value);
HAL_StatusTypeDef INA228_SetBOVLReg(INA228_HandleTypeDef *ina228, uint16_t value);
HAL_StatusTypeDef INA228_GetBOVLReg(INA228_HandleTypeDef *ina228, uint16_t *value);
HAL_StatusTypeDef INA228_SetBUVLReg(INA228_HandleTypeDef *ina228, uint16_t value);
HAL_StatusTypeDef INA228_GetBUVLReg(INA228_HandleTypeDef *ina228, uint16_t *value);
HAL_StatusTypeDef INA228_SetTempLimitReg(INA228_HandleTypeDef *ina228, uint16_t value);
HAL_StatusTypeDef INA228_GetTempLimitReg(INA228_HandleTypeDef *ina228, uint16_t *value);
HAL_StatusTypeDef INA228_SetPowerLimitReg(INA228_HandleTypeDef *ina228, uint16_t value);
HAL_StatusTypeDef INA228_GetPowerLimitReg(INA228_HandleTypeDef *ina228, uint16_t *value);
HAL_StatusTypeDef INA228_GetManufacturerID(INA228_HandleTypeDef *ina228, uint16_t *value);
HAL_StatusTypeDef INA228_GetDeviceID(INA228_HandleTypeDef *ina228, uint16_t *value);

/* Configuration Setters */
HAL_StatusTypeDef INA228_SetAdcRange(INA228_HandleTypeDef *ina228, INA228_AdcRange_TypeDef adcRange);
HAL_StatusTypeDef INA228_SetTempComp(INA228_HandleTypeDef *ina228, INA228_TempComp_TypeDef tempComp);
HAL_StatusTypeDef INA228_SetConversionDelay_ms(INA228_HandleTypeDef *ina228, uint16_t delay_ms);
HAL_StatusTypeDef INA228_SetAverage(INA228_HandleTypeDef *ina228, INA228_Average_TypeDef avg);
HAL_StatusTypeDef INA228_SetTempConvTime(INA228_HandleTypeDef *ina228, INA228_ConvTime_TypeDef convTime);
HAL_StatusTypeDef INA228_SetShuntConvTime(INA228_HandleTypeDef *ina228, INA228_ConvTime_TypeDef convTime);
HAL_StatusTypeDef INA228_SetBusConvTime(INA228_HandleTypeDef *ina228, INA228_ConvTime_TypeDef convTime);
HAL_StatusTypeDef INA228_SetMode(INA228_HandleTypeDef *ina228, INA228_Mode_TypeDef mode);

/* Configuration Getters */
HAL_StatusTypeDef INA228_GetAdcRange(INA228_HandleTypeDef *ina228, INA228_AdcRange_TypeDef *adcRange);
HAL_StatusTypeDef INA228_GetTempComp(INA228_HandleTypeDef *ina228, INA228_TempComp_TypeDef *tempComp);
HAL_StatusTypeDef INA228_GetConversionDelay(INA228_HandleTypeDef *ina228, INA228_ConvDelay_TypeDef *convDelay);
HAL_StatusTypeDef INA228_GetAverage(INA228_HandleTypeDef *ina228, INA228_Average_TypeDef *avg);
HAL_StatusTypeDef INA228_GetTempConvTime(INA228_HandleTypeDef *ina228, INA228_ConvTime_TypeDef *convTime);
HAL_StatusTypeDef INA228_GetShuntConvTime(INA228_HandleTypeDef *ina228, INA228_ConvTime_TypeDef *convTime);
HAL_StatusTypeDef INA228_GetBusConvTime(INA228_HandleTypeDef *ina228, INA228_ConvTime_TypeDef *convTime);
HAL_StatusTypeDef INA228_GetMode(INA228_HandleTypeDef *ina228, INA228_Mode_TypeDef *mode);

/* Shunt Calibration & Reset */
HAL_StatusTypeDef INA228_SetShuntCalibration(INA228_HandleTypeDef *ina228, double rShuntValue, double maxCurrent);
HAL_StatusTypeDef INA228_ResetEnergyAndCharge(INA228_HandleTypeDef *ina228);
HAL_StatusTypeDef INA228_ResetDevice(INA228_HandleTypeDef *ina228);

/* Physical Quantity Measurement Functions */
HAL_StatusTypeDef INA228_GetDieTemp_C(INA228_HandleTypeDef *ina228, double *temp);
HAL_StatusTypeDef INA228_GetShuntVoltage_mV(INA228_HandleTypeDef *ina228, double *vShunt);
HAL_StatusTypeDef INA228_GetBusVoltage_V(INA228_HandleTypeDef *ina228, double *vBus);
HAL_StatusTypeDef INA228_GetPower_W(INA228_HandleTypeDef *ina228, double *power);
HAL_StatusTypeDef INA228_GetCurrent_A(INA228_HandleTypeDef *ina228, double *current);
HAL_StatusTypeDef INA228_GetEnergy_J(INA228_HandleTypeDef *ina228, double *energy);
HAL_StatusTypeDef INA228_GetCharge_Coulomb(INA228_HandleTypeDef *ina228, double *charge);
HAL_StatusTypeDef INA228_GetCharge_mAh(INA228_HandleTypeDef *ina228, double *charge_mAh);

/* Alert & Diagnostic Functions */
HAL_StatusTypeDef INA228_SetAlertLatch(INA228_HandleTypeDef *ina228, INA228_AlertLatch_TypeDef latch);
HAL_StatusTypeDef INA228_GetAlertLatch(INA228_HandleTypeDef *ina228, INA228_AlertLatch_TypeDef *latch);

HAL_StatusTypeDef INA228_SetAlertPin(INA228_HandleTypeDef *ina228, INA228_CNVR_TypeDef cnvr);
HAL_StatusTypeDef INA228_GetAlertPin(INA228_HandleTypeDef *ina228, INA228_CNVR_TypeDef *cnvr);

HAL_StatusTypeDef INA228_SetAlertPinPolarity(INA228_HandleTypeDef *ina228, INA228_AlertPinPol_TypeDef pol);
HAL_StatusTypeDef INA228_GetAlertPinPolarity(INA228_HandleTypeDef *ina228, INA228_AlertPinPol_TypeDef *pol);

HAL_StatusTypeDef INA228_SetSlowAlert(INA228_HandleTypeDef *ina228, INA228_SlowAlert_TypeDef slowAlert);
HAL_StatusTypeDef INA228_GetSlowAlert(INA228_HandleTypeDef *ina228, INA228_SlowAlert_TypeDef *slowAlert);

HAL_StatusTypeDef INA228_GetEnergyOverflowFlag(INA228_HandleTypeDef *ina228, INA228_EnergyOverflow_TypeDef *overflow);
HAL_StatusTypeDef INA228_GetChargeOverflowFlag(INA228_HandleTypeDef *ina228, INA228_ChargeOverflow_TypeDef *overflow);
HAL_StatusTypeDef INA228_GetMathOverflowFlag(INA228_HandleTypeDef *ina228, INA228_MathOverflow_TypeDef *overflow);

HAL_StatusTypeDef INA228_GetTempOverLimitFlag(INA228_HandleTypeDef *ina228, INA228_TempOverLimit_TypeDef *flag);
HAL_StatusTypeDef INA228_GetShuntOverLimitFlag(INA228_HandleTypeDef *ina228, INA228_ShuntOverLimit_TypeDef *flag);
HAL_StatusTypeDef INA228_GetShuntUnderLimitFlag(INA228_HandleTypeDef *ina228, INA228_ShuntUnderLimit_TypeDef *flag);
HAL_StatusTypeDef INA228_GetBusOverLimitFlag(INA228_HandleTypeDef *ina228, INA228_BusOverLimit_TypeDef *flag);
HAL_StatusTypeDef INA228_GetBusUnderLimitFlag(INA228_HandleTypeDef *ina228, INA228_BusUnderLimit_TypeDef *flag);
HAL_StatusTypeDef INA228_GetPowerOverLimitFlag(INA228_HandleTypeDef *ina228, INA228_PowerOverLimit_TypeDef *flag);
HAL_StatusTypeDef INA228_GetConversionReadyFlag(INA228_HandleTypeDef *ina228, INA228_ConvReadyFlag_TypeDef *flag);
HAL_StatusTypeDef INA228_GetMemoryStatus(INA228_HandleTypeDef *ina228, INA228_MemStatus_TypeDef *status);

HAL_StatusTypeDef INA228_GetDiagAlert(INA228_HandleTypeDef *ina228, uint16_t *diagAlert);

/* Threshold Limit Functions */
HAL_StatusTypeDef INA228_SetShuntOverVoltageThreshold_mV(INA228_HandleTypeDef *ina228, double threshold_mV);
HAL_StatusTypeDef INA228_GetShuntOverVoltageThreshold_mV(INA228_HandleTypeDef *ina228, double *threshold_mV);

HAL_StatusTypeDef INA228_SetShuntUnderVoltageThreshold_mV(INA228_HandleTypeDef *ina228, double threshold_mV);
HAL_StatusTypeDef INA228_GetShuntUnderVoltageThreshold_mV(INA228_HandleTypeDef *ina228, double *threshold_mV);

HAL_StatusTypeDef INA228_SetBusOverVoltageThreshold_V(INA228_HandleTypeDef *ina228, double threshold_V);
HAL_StatusTypeDef INA228_GetBusOverVoltageThreshold_V(INA228_HandleTypeDef *ina228, double *threshold_V);

HAL_StatusTypeDef INA228_SetBusUnderVoltageThreshold_V(INA228_HandleTypeDef *ina228, double threshold_V);
HAL_StatusTypeDef INA228_GetBusUnderVoltageThreshold_V(INA228_HandleTypeDef *ina228, double *threshold_V);

HAL_StatusTypeDef INA228_SetTempOverLimitThreshold_C(INA228_HandleTypeDef *ina228, double threshold_C);
HAL_StatusTypeDef INA228_GetTempOverLimitThreshold_C(INA228_HandleTypeDef *ina228, double *threshold_C);

HAL_StatusTypeDef INA228_SetPowerOverLimitThreshold_W(INA228_HandleTypeDef *ina228, double threshold_W);
HAL_StatusTypeDef INA228_GetPowerOverLimitThreshold_W(INA228_HandleTypeDef *ina228, double *threshold_W);

#ifdef __cplusplus
}
#endif

#endif /* INC_INA228_H_ */
