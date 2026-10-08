/**
  *******************************************************************************************
  * @file           : INA228.c
  * @brief          : INA228 20-bit Ultra-Precise Digital Power Monitor Driver Implementation
  *******************************************************************************************
  */

#include "main.h"
#include "i2c_bus.h"
#include "math.h"
#include "INA228.h"

/* =======================================================================================
   CAPA DE ADAPTACIÓN PRIVADA (WRAPPERS INTERNOS)
   ======================================================================================= */

/**
  * @brief  Write a 16-bit register value to the INA228 sensor via I2C.
  * @note   Internal helper function. Transmits data in Big-Endian format.
  * @param  ina228 Pointer to a INA228_HandleTypeDef structure that contains
  *         the configuration information for connecting to the sensor.
  * @param  registerAddress Target register address on the INA228.
  * @param  value 16-bit value to be written.
  * @return HAL status
  */
static inline HAL_StatusTypeDef INA228_WriteRegister16(INA228_HandleTypeDef *ina228, uint8_t registerAddress, uint16_t value)
{
    if (ina228 == NULL || ina228->hi2c == NULL) return HAL_ERROR;
    return I2C_Bus_WriteRegister16_BE(ina228->hi2c, ina228->_devAddress, registerAddress, value);
}

/**
  * @brief  Read a 16-bit register value from the INA228 sensor via I2C.
  * @note   Internal helper function. Reads data in Big-Endian format.
  * @param  ina228 Pointer to a INA228_HandleTypeDef structure that contains
  *         the configuration information for connecting to the sensor.
  * @param  registerAddress Target register address on the INA228.
  * @param  value Pointer to store the read 16-bit value.
  * @return HAL status
  */
static inline HAL_StatusTypeDef INA228_ReadRegister16(INA228_HandleTypeDef *ina228, uint8_t registerAddress, uint16_t *value)
{
    if (ina228 == NULL || ina228->hi2c == NULL || value == NULL) return HAL_ERROR;
    return I2C_Bus_ReadRegister16_BE(ina228->hi2c, ina228->_devAddress, registerAddress, value);
}

/**
  * @brief  Read a 24-bit register value from the INA228 sensor via I2C (MSB aligned in 32-bit).
  * @note   Internal helper function. Reads data in Big-Endian format.
  * @param  ina228 Pointer to a INA228_HandleTypeDef structure that contains
  *         the configuration information for connecting to the sensor.
  * @param  registerAddress Target register address on the INA228.
  * @param  value Pointer to store the read 24-bit value.
  * @return HAL status
  */
static inline HAL_StatusTypeDef INA228_ReadRegister32(INA228_HandleTypeDef *ina228, uint8_t registerAddress, uint32_t *value)
{
    if (ina228 == NULL || ina228->hi2c == NULL || value == NULL) return HAL_ERROR;
    /* Los registros de 24 bits (VBUS, VSHUNT, CURRENT, POWER) ocupan 3 bytes en el INA228 */
    return I2C_Bus_ReadRegister24_Unsigned_BE(ina228->hi2c, ina228->_devAddress, registerAddress, value);
}

/**
  * @brief  Read a 40-bit register value from the INA228 sensor via I2C (MSB aligned in 64-bit).
  * @note   Internal helper function. Reads data in Big-Endian format.
  * @param  ina228 Pointer to a INA228_HandleTypeDef structure that contains
  *         the configuration information for connecting to the sensor.
  * @param  registerAddress Target register address on the INA228.
  * @param  value Pointer to store the read 40-bit value.
  * @return HAL status
  */
static inline HAL_StatusTypeDef INA228_ReadRegister64(INA228_HandleTypeDef *ina228, uint8_t registerAddress, uint64_t *value)
{
    if (ina228 == NULL || ina228->hi2c == NULL || value == NULL) return HAL_ERROR;
    /* Los registros de 40 bits (ENERGY, CHARGE) ocupan 5 bytes en el INA228 */
    uint8_t buf[5] = {0};
    if (I2C_Bus_ReadRegister(ina228->hi2c, ina228->_devAddress, registerAddress, buf, 5) == HAL_OK)
    {
        *value = ((uint64_t)buf[0] << 32) | ((uint64_t)buf[1] << 24) |
                 ((uint64_t)buf[2] << 16) | ((uint64_t)buf[3] << 8)  | buf[4];
        return HAL_OK;
    }
    return HAL_ERROR;
}

/**
  * @brief  Round the minimum Current LSB to the next clean 1-2-5 step of a power of 10.
  * @note   Internal helper function implementing a standard 1-2-5 rounding rule (e.g. 10uA, 20uA, 50uA).
  * @param  lsbMin The calculated absolute minimum Current LSB (in Amperes/LSB).
  * @return The rounded, user-friendly Current LSB value (in Amperes).
  */
static double INA228_RoundCurrentLsb(double lsbMin)
{
    double logLsb = log10(lsbMin);
    double powerOf10 = pow(10, floor(logLsb));
    double normalized = lsbMin / powerOf10;
    double roundedLsb;
    
    if (normalized <= 1.0)
    {
        roundedLsb = 1.0 * powerOf10;
    }
    else if (normalized <= 2.0)
    {
        roundedLsb = 2.0 * powerOf10;
    }
    else if (normalized <= 3.0)
    {
        roundedLsb = 3.0 * powerOf10;
    }
    else if (normalized <= 4.0)
    {
        roundedLsb = 4.0 * powerOf10;
    }
    else if (normalized <= 5.0)
    {
        roundedLsb = 5.0 * powerOf10;
    }
    else if (normalized <= 6.0)
    {
        roundedLsb = 6.0 * powerOf10;
    }
    else
    {
        roundedLsb = 10.0 * powerOf10;
    }
    
    return roundedLsb;
}

/* =======================================================================================
   CAPA DE APLICACIÓN (FUNCIONES PÚBLICAS)
   ======================================================================================= */

/**
  * @brief  Initialize the INA228 sensor handle and assign default parameters.
  * @param  ina228 Pointer to a INA228_HandleTypeDef structure that contains
  *         the configuration information for connecting to the sensor.
  * @param  i2c Pointer to a HAL I2C_HandleTypeDef structure.
  * @param  devAddress 7-bit I2C device address (default 0x40).
  * @retval HAL status
  */
HAL_StatusTypeDef INA228_Init(INA228_HandleTypeDef *ina228, I2C_HandleTypeDef *i2c, uint8_t devAddress)
{
    if (ina228 == NULL || i2c == NULL)
    {
        return HAL_ERROR;
    }

    ina228->hi2c = i2c;
    ina228->_devAddress = devAddress;
    ina228->_adcRange = 0; // Default ±163.84 mV
    ina228->_shuntAdcRange = 0.16384;
    ina228->_resolution = 0.0000003125; // 312.5 nV/LSB

    HAL_StatusTypeDef status;

    uint16_t config = 0;
    uint16_t adcConfig = 0;
    
    config |= (INA228_DELAY_0_MS          << INA228_CONVDLY_Pos)  & INA228_CONVDLY; 
    config |= (INA228_TEMP_COMP_DISABLED  << INA228_TEMPCOMP_Pos) & INA228_TEMPCOMP;
    config |= (INA228_ADC_RANGE_163_84_MV << INA228_ADCRANGE_Pos) & INA228_ADCRANGE;

    adcConfig |= (INA228_1_SAMPLE                  << INA228_AVG_Pos)    & INA228_AVG;
    adcConfig |= (INA228_1052_US                   << INA228_VTCT_Pos)   & INA228_VTCT;
    adcConfig |= (INA228_1052_US                   << INA228_VSHCT_Pos)  & INA228_VSHCT;
    adcConfig |= (INA228_1052_US                   << INA228_VBUSCT_Pos) & INA228_VBUSCT;
    adcConfig |= (INA228_TEMP_SHUNT_BUS_CONTINUOUS << INA228_MODE_Pos)   & INA228_MODE;

    status = INA228_WriteRegister16(ina228, INA228_CONFIG_REG, config);
    if (status != HAL_OK)
    {
        return status;
    }

    return INA228_WriteRegister16(ina228, INA228_ADC_CONFIG_REG, adcConfig);
}

/**
  * @brief  Read the 16-bit Configuration Register (0x00).
  * @param  ina228 Pointer to a INA228_HandleTypeDef structure that contains
  *         the configuration information for connecting to the sensor.
  * @param  value Pointer to store the configuration register value.
  * @return HAL status
  */
HAL_StatusTypeDef INA228_GetConfigurationReg(INA228_HandleTypeDef *ina228, uint16_t *value)
{
    return INA228_ReadRegister16(ina228, INA228_CONFIG_REG, value);
}

/**
  * @brief  Read the 16-bit ADC Configuration Register (0x01).
  * @param  ina228 Pointer to a INA228_HandleTypeDef structure that contains
  *         the configuration information for connecting to the sensor.
  * @param  value Pointer to store the ADC configuration register value.
  * @return HAL status
  */
HAL_StatusTypeDef INA228_GetAdcConfigurationReg(INA228_HandleTypeDef *ina228, uint16_t *value)
{
    return INA228_ReadRegister16(ina228, INA228_ADC_CONFIG_REG, value);
}

/**
  * @brief  Read the 16-bit Shunt Calibration Register (0x02).
  * @param  ina228 Pointer to a INA228_HandleTypeDef structure that contains
  *         the configuration information for connecting to the sensor.
  * @param  value Pointer to store the calibration register value.
  * @return HAL status
  */
HAL_StatusTypeDef INA228_GetShuntCalibrationReg(INA228_HandleTypeDef *ina228, uint16_t *value)
{
    return INA228_ReadRegister16(ina228, INA228_SHUNT_CAL_REG, value);
}

/**
  * @brief  Read the 16-bit Shunt Temperature Coefficient Register (0x03).
  * @param  ina228 Pointer to a INA228_HandleTypeDef structure that contains
  *         the configuration information for connecting to the sensor.
  * @param  value Pointer to store the temperature coefficient value.
  * @return HAL status
  */
HAL_StatusTypeDef INA228_GetTempCoefficientReg(INA228_HandleTypeDef *ina228, uint16_t *value)
{
    return INA228_ReadRegister16(ina228, INA228_SHUNT_TEMPCOEF_REG, value);
}

/**
  * @brief  Read the raw 24-bit Shunt Voltage Register (0x04).
  * @param  ina228 Pointer to a INA228_HandleTypeDef structure that contains
  *         the configuration information for connecting to the sensor.
  * @param  value Pointer to store the raw 24-bit shunt voltage value.
  * @return HAL status
  */
HAL_StatusTypeDef INA228_GetShuntVoltageReg(INA228_HandleTypeDef *ina228, uint32_t *value)
{
    return INA228_ReadRegister32(ina228, INA228_VSHUNT_REG, value);
}

/**
  * @brief  Read the raw 24-bit Bus Voltage Register (0x05).
  * @param  ina228 Pointer to a INA228_HandleTypeDef structure that contains
  *         the configuration information for connecting to the sensor.
  * @param  value Pointer to store the raw 24-bit bus voltage value.
  * @return HAL status
  */
HAL_StatusTypeDef INA228_GetBusVoltageReg(INA228_HandleTypeDef *ina228, uint32_t *value)
{
    return INA228_ReadRegister32(ina228, INA228_VBUS_REG, value);
}

/**
  * @brief  Read the raw 16-bit Die Temperature Register (0x06).
  * @param  ina228 Pointer to a INA228_HandleTypeDef structure that contains
  *         the configuration information for connecting to the sensor.
  * @param  value Pointer to store the raw 16-bit die temperature value.
  * @return HAL status
  */
HAL_StatusTypeDef INA228_GetDieTempReg(INA228_HandleTypeDef *ina228, uint16_t *value)
{
    return INA228_ReadRegister16(ina228, INA228_DIETEMP_REG, value);
}

/**
  * @brief  Read the raw 24-bit Current Register (0x07).
  * @param  ina228 Pointer to a INA228_HandleTypeDef structure that contains
  *         the configuration information for connecting to the sensor.
  * @param  value Pointer to store the raw 24-bit current value.
  * @return HAL status
  */
HAL_StatusTypeDef INA228_GetCurrentReg(INA228_HandleTypeDef *ina228, uint32_t *value)
{
    return INA228_ReadRegister32(ina228, INA228_CURRENT_REG, value);
}

/**
  * @brief  Read the raw 24-bit Power Register (0x08).
  * @param  ina228 Pointer to a INA228_HandleTypeDef structure that contains
  *         the configuration information for connecting to the sensor.
  * @param  value Pointer to store the raw 24-bit power value.
  * @return HAL status
  */
HAL_StatusTypeDef INA228_GetPowerReg(INA228_HandleTypeDef *ina228, uint32_t *value)
{
    return INA228_ReadRegister32(ina228, INA228_POWER_REG, value);
}

/**
  * @brief  Read the raw 40-bit Energy Register (0x09).
  * @param  ina228 Pointer to a INA228_HandleTypeDef structure that contains
  *         the configuration information for connecting to the sensor.
  * @param  value Pointer to store the raw 40-bit energy value.
  * @return HAL status
  */
HAL_StatusTypeDef INA228_GetEnergyReg(INA228_HandleTypeDef *ina228, uint64_t *value)
{
    return INA228_ReadRegister64(ina228, INA228_ENERGY_REG, value);
}

HAL_StatusTypeDef INA228_GetDiagAlertReg(INA228_HandleTypeDef *ina228, uint16_t *value)
{
    return INA228_ReadRegister16(ina228, INA228_DIAG_ALERT_REG, value);
}

/**
  * @brief  Read the raw 40-bit Charge Register (0x0A).
  * @param  ina228 Pointer to a INA228_HandleTypeDef structure that contains
  *         the configuration information for connecting to the sensor.
  * @param  value Pointer to store the raw 40-bit charge value.
  * @return HAL status
  */
HAL_StatusTypeDef INA228_GetChargeReg(INA228_HandleTypeDef *ina228, uint64_t *value)
{
    return INA228_ReadRegister64(ina228, INA228_CHARGE_REG, value);
}

/**
  * @brief  Write raw 16-bit value to Shunt Overvoltage Threshold Register (SOVL, 0x0C).
  * @param  ina228 Pointer to a INA228_HandleTypeDef structure.
  * @param  value Raw 16-bit threshold value (Two's complement).
  * @return HAL status
  */
HAL_StatusTypeDef INA228_SetSOVLReg(INA228_HandleTypeDef *ina228, uint16_t value)
{
    return INA228_WriteRegister16(ina228, INA228_SOVL_REG, value);
}

/**
  * @brief  Read raw 16-bit value from Shunt Overvoltage Threshold Register (SOVL, 0x0C).
  * @param  ina228 Pointer to a INA228_HandleTypeDef structure.
  * @param  value Pointer to store raw 16-bit threshold value.
  * @return HAL status
  */
HAL_StatusTypeDef INA228_GetSOVLReg(INA228_HandleTypeDef *ina228, uint16_t *value)
{
    return INA228_ReadRegister16(ina228, INA228_SOVL_REG, value);
}

/**
  * @brief  Write raw 16-bit value to Shunt Undervoltage Threshold Register (SUVL, 0x0D).
  * @param  ina228 Pointer to a INA228_HandleTypeDef structure.
  * @param  value Raw 16-bit threshold value (Two's complement).
  * @return HAL status
  */
HAL_StatusTypeDef INA228_SetSUVLReg(INA228_HandleTypeDef *ina228, uint16_t value)
{
    return INA228_WriteRegister16(ina228, INA228_SUVL_REG, value);
}

/**
  * @brief  Read raw 16-bit value from Shunt Undervoltage Threshold Register (SUVL, 0x0D).
  * @param  ina228 Pointer to a INA228_HandleTypeDef structure.
  * @param  value Pointer to store raw 16-bit threshold value.
  * @return HAL status
  */
HAL_StatusTypeDef INA228_GetSUVLReg(INA228_HandleTypeDef *ina228, uint16_t *value)
{
    return INA228_ReadRegister16(ina228, INA228_SUVL_REG, value);
}

/**
  * @brief  Write raw 16-bit value to Bus Overvoltage Threshold Register (BOVL, 0x0E).
  * @param  ina228 Pointer to a INA228_HandleTypeDef structure.
  * @param  value Raw 16-bit threshold value (15-bit unsigned).
  * @return HAL status
  */
HAL_StatusTypeDef INA228_SetBOVLReg(INA228_HandleTypeDef *ina228, uint16_t value)
{
    return INA228_WriteRegister16(ina228, INA228_BOVL_REG, value & 0x7FFF);
}

/**
  * @brief  Read raw 16-bit value from Bus Overvoltage Threshold Register (BOVL, 0x0E).
  * @param  ina228 Pointer to a INA228_HandleTypeDef structure.
  * @param  value Pointer to store raw 16-bit threshold value.
  * @return HAL status
  */
HAL_StatusTypeDef INA228_GetBOVLReg(INA228_HandleTypeDef *ina228, uint16_t *value)
{
    return INA228_ReadRegister16(ina228, INA228_BOVL_REG, value);
}

/**
  * @brief  Write raw 16-bit value to Bus Undervoltage Threshold Register (BUVL, 0x0F).
  * @param  ina228 Pointer to a INA228_HandleTypeDef structure.
  * @param  value Raw 16-bit threshold value (15-bit unsigned).
  * @return HAL status
  */
HAL_StatusTypeDef INA228_SetBUVLReg(INA228_HandleTypeDef *ina228, uint16_t value)
{
    return INA228_WriteRegister16(ina228, INA228_BUVL_REG, value & 0x7FFF);
}

/**
  * @brief  Read raw 16-bit value from Bus Undervoltage Threshold Register (BUVL, 0x0F).
  * @param  ina228 Pointer to a INA228_HandleTypeDef structure.
  * @param  value Pointer to store raw 16-bit threshold value.
  * @return HAL status
  */
HAL_StatusTypeDef INA228_GetBUVLReg(INA228_HandleTypeDef *ina228, uint16_t *value)
{
    return INA228_ReadRegister16(ina228, INA228_BUVL_REG, value);
}

/**
  * @brief  Write raw 16-bit value to Temperature Over-Limit Threshold Register (TEMP_LIMIT, 0x10).
  * @param  ina228 Pointer to a INA228_HandleTypeDef structure.
  * @param  value Raw 16-bit threshold value (Two's complement).
  * @return HAL status
  */
HAL_StatusTypeDef INA228_SetTempLimitReg(INA228_HandleTypeDef *ina228, uint16_t value)
{
    return INA228_WriteRegister16(ina228, INA228_TEMP_LIMIT_REG, value);
}

/**
  * @brief  Read raw 16-bit value from Temperature Over-Limit Threshold Register (TEMP_LIMIT, 0x10).
  * @param  ina228 Pointer to a INA228_HandleTypeDef structure.
  * @param  value Pointer to store raw 16-bit threshold value.
  * @return HAL status
  */
HAL_StatusTypeDef INA228_GetTempLimitReg(INA228_HandleTypeDef *ina228, uint16_t *value)
{
    return INA228_ReadRegister16(ina228, INA228_TEMP_LIMIT_REG, value);
}

/**
  * @brief  Write raw 16-bit value to Power Over-Limit Threshold Register (PWR_LIMIT, 0x11).
  * @param  ina228 Pointer to a INA228_HandleTypeDef structure.
  * @param  value Raw 16-bit threshold value (Unsigned).
  * @return HAL status
  */
HAL_StatusTypeDef INA228_SetPowerLimitReg(INA228_HandleTypeDef *ina228, uint16_t value)
{
    return INA228_WriteRegister16(ina228, INA228_POWER_LIMIT_REG, value);
}

/**
  * @brief  Read raw 16-bit value from Power Over-Limit Threshold Register (PWR_LIMIT, 0x11).
  * @param  ina228 Pointer to a INA228_HandleTypeDef structure.
  * @param  value Pointer to store raw 16-bit threshold value.
  * @return HAL status
  */
HAL_StatusTypeDef INA228_GetPowerLimitReg(INA228_HandleTypeDef *ina228, uint16_t *value)
{
    return INA228_ReadRegister16(ina228, INA228_POWER_LIMIT_REG, value);
}

/**
  * @brief  Read the Manufacturer ID Register (0x3E).
  * @param  ina228 Pointer to a INA228_HandleTypeDef structure that contains
  *         the configuration information for connecting to the sensor.
  * @param  value Pointer to store Manufacturer ID (0x5449 = "TI").
  * @return HAL status
  */
HAL_StatusTypeDef INA228_GetManufacturerID(INA228_HandleTypeDef *ina228, uint16_t *value)
{
    return INA228_ReadRegister16(ina228, INA228_MANUFACTURER_ID_REG, value);
}

/**
  * @brief  Read the Device ID Register (0x3F).
  * @param  ina228 Pointer to a INA228_HandleTypeDef structure that contains
  *         the configuration information for connecting to the sensor.
  * @param  value Pointer to store Device ID and Revision.
  * @return HAL status
  */
HAL_StatusTypeDef INA228_GetDeviceID(INA228_HandleTypeDef *ina228, uint16_t *value)
{
    return INA228_ReadRegister16(ina228, INA228_DEVICE_ID_REG, value);
}

/**
  * @brief  Configure the ADC full-scale differential range (±163.84 mV or ±40.96 mV).
  * @param  ina228 Pointer to a INA228_HandleTypeDef structure that contains
  *         the configuration information for connecting to the sensor.
  * @param  adcRange Selected ADC full-scale differential range option.
  *         This parameter can be one of the following values:
  *         @arg INA228_ADC_RANGE_163_84_MV: ±163.84 mV full-scale range (default, 0b)
  *         @arg INA228_ADC_RANGE_40_96_MV: ±40.96 mV full-scale range (1b)
  * @return HAL status
  */
HAL_StatusTypeDef INA228_SetAdcRange(INA228_HandleTypeDef *ina228, INA228_AdcRange_TypeDef adcRange)
{
    uint16_t regValue = 0;

    if (INA228_GetConfigurationReg(ina228, &regValue) == HAL_OK)
    {
        regValue &= ~INA228_ADCRANGE;
        regValue |= (adcRange << INA228_ADCRANGE_Pos) & INA228_ADCRANGE;

        ina228->_adcRange = (uint8_t)adcRange;

        if (adcRange == INA228_ADC_RANGE_40_96_MV)
        {
            ina228->_shuntAdcRange = 0.04096;
            ina228->_resolution = 0.000000078125; // 78.125 nV/LSB
        }
        else
        {
            ina228->_shuntAdcRange = 0.16384;
            ina228->_resolution = 0.0000003125; // 312.5 nV/LSB
        }

        return INA228_WriteRegister16(ina228, INA228_CONFIG_REG, regValue); 
    }

    return HAL_ERROR;
}

/**
  * @brief  Enable or disable shunt resistor temperature compensation.
  * @param  ina228 Pointer to a INA228_HandleTypeDef structure that contains
  *         the configuration information for connecting to the sensor.
  * @param  tempComp Shunt temperature compensation setting.
  *         This parameter can be one of the following values:
  *         @arg INA228_TEMP_COMP_DISABLED: Temperature compensation disabled (default, 0b)
  *         @arg INA228_TEMPERATURE_COMP_ENABLED: Temperature compensation enabled (1b)
  * @return HAL status
  */
HAL_StatusTypeDef INA228_SetTempComp(INA228_HandleTypeDef *ina228, INA228_TempComp_TypeDef tempComp)
{
    uint16_t regValue = 0;

    if (INA228_GetConfigurationReg(ina228, &regValue) == HAL_OK)
    {
        regValue &= ~INA228_TEMPCOMP; 
        regValue |= (tempComp << INA228_TEMPCOMP_Pos) & INA228_TEMPCOMP; 
        return INA228_WriteRegister16(ina228, INA228_CONFIG_REG, regValue); 
    }

    return HAL_ERROR;
}

/**
  * @brief  Set initial conversion delay time between measurements in milliseconds (0 to 510 ms).
  * @param  ina228 Pointer to an INA228_HandleTypeDef structure that contains
  *         the configuration information for connecting to the sensor.
  * @param  delay_ms Desired conversion delay in milliseconds (0 to 510 ms).
  *         Hardware steps are calculated automatically in 2 ms increments
  *         with ceiling rounding.
  * @return HAL status
  */
HAL_StatusTypeDef INA228_SetConversionDelay_ms(INA228_HandleTypeDef *ina228, uint16_t delay_ms)
{
    if (ina228 == NULL)
    {
        return HAL_ERROR;
    }

    if (delay_ms > 510) 
    {
        delay_ms = 510;
    }

    uint16_t regValue = 0;
    uint8_t convDelay = (uint8_t)((delay_ms + 1) / 2);

    if (INA228_GetConfigurationReg(ina228, &regValue) == HAL_OK)
    {
        regValue &= ~INA228_CONVDLY;
        regValue |= ((uint16_t)convDelay << INA228_CONVDLY_Pos) & INA228_CONVDLY;
        return INA228_WriteRegister16(ina228, INA228_CONFIG_REG, regValue);
    }

    return HAL_ERROR;
}


/**
  * @brief  Set internal sample averaging count (1x to 1024x).
  * @param  ina228 Pointer to a INA228_HandleTypeDef structure that contains
  *         the configuration information for connecting to the sensor.
  * @param  avg Selected conversion sample averaging count.
  *         This parameter can be one of the following values:
  *         @arg INA228_1_SAMPLE: 1 sample (no averaging, default, 000b)
  *         @arg INA228_4_SAMPLES: 4 samples averaged (001b)
  *         @arg INA228_16_SAMPLES: 16 samples averaged (010b)
  *         @arg INA228_64_SAMPLES: 64 samples averaged (011b)
  *         @arg INA228_128_SAMPLES: 128 samples averaged (100b)
  *         @arg INA228_256_SAMPLES: 256 samples averaged (101b)
  *         @arg INA228_512_SAMPLES: 512 samples averaged (110b)
  *         @arg INA228_1024_SAMPLES: 1024 samples averaged (111b)
  * @return HAL status
  */
HAL_StatusTypeDef INA228_SetAverage(INA228_HandleTypeDef *ina228, INA228_Average_TypeDef avg)
{
    uint16_t regValue = 0;

    if (INA228_GetAdcConfigurationReg(ina228, &regValue) == HAL_OK)
    {
        regValue &= ~INA228_AVG; 
        regValue |= (avg << INA228_AVG_Pos) & INA228_AVG; 
        return INA228_WriteRegister16(ina228, INA228_ADC_CONFIG_REG, regValue); 
    }

    return HAL_ERROR; 
}

/**
  * @brief  Set temperature measurement conversion time (50 us to 4120 us).
  * @param  ina228 Pointer to a INA228_HandleTypeDef structure that contains
  *         the configuration information for connecting to the sensor.
  * @param  convTime Selected temperature ADC conversion time.
  *         This parameter can be one of the following values:
  *         @arg INA228_50_US: 50 us conversion time (000b)
  *         @arg INA228_84_US: 84 us conversion time (001b)
  *         @arg INA228_150_US: 150 us conversion time (010b)
  *         @arg INA228_280_US: 280 us conversion time (011b)
  *         @arg INA228_540_US: 540 us conversion time (100b, default)
  *         @arg INA228_1052_US: 1052 us conversion time (101b)
  *         @arg INA228_2074_US: 2074 us conversion time (110b)
  *         @arg INA228_4120_US: 4120 us conversion time (111b)
  * @return HAL status
  */
HAL_StatusTypeDef INA228_SetTempConvTime(INA228_HandleTypeDef *ina228, INA228_ConvTime_TypeDef convTime)
{
    uint16_t regValue = 0;

    if (INA228_GetAdcConfigurationReg(ina228, &regValue) == HAL_OK)
    {
        regValue &= ~INA228_VTCT; 
        regValue |= (convTime << INA228_VTCT_Pos) & INA228_VTCT; 
        return INA228_WriteRegister16(ina228, INA228_ADC_CONFIG_REG, regValue); 
    }

    return HAL_ERROR; 
}

/**
  * @brief  Set shunt voltage measurement conversion time (50 us to 4120 us).
  * @param  ina228 Pointer to a INA228_HandleTypeDef structure that contains
  *         the configuration information for connecting to the sensor.
  * @param  convTime Selected shunt ADC conversion time.
  *         This parameter can be one of the following values:
  *         @arg INA228_50_US: 50 us conversion time (000b)
  *         @arg INA228_84_US: 84 us conversion time (001b)
  *         @arg INA228_150_US: 150 us conversion time (010b)
  *         @arg INA228_280_US: 280 us conversion time (011b)
  *         @arg INA228_540_US: 540 us conversion time (100b, default)
  *         @arg INA228_1052_US: 1052 us conversion time (101b)
  *         @arg INA228_2074_US: 2074 us conversion time (110b)
  *         @arg INA228_4120_US: 4120 us conversion time (111b)
  * @return HAL status
  */
HAL_StatusTypeDef INA228_SetShuntConvTime(INA228_HandleTypeDef *ina228, INA228_ConvTime_TypeDef convTime)
{
    uint16_t regValue = 0;

    if (INA228_GetAdcConfigurationReg(ina228, &regValue) == HAL_OK)
    {
        regValue &= ~INA228_VSHCT; 
        regValue |= (convTime << INA228_VSHCT_Pos) & INA228_VSHCT; 
        return INA228_WriteRegister16(ina228, INA228_ADC_CONFIG_REG, regValue); 
    }

    return HAL_ERROR; 
}

/**
  * @brief  Set bus voltage measurement conversion time (50 us to 4120 us).
  * @param  ina228 Pointer to a INA228_HandleTypeDef structure that contains
  *         the configuration information for connecting to the sensor.
  * @param  convTime Selected bus ADC conversion time.
  *         This parameter can be one of the following values:
  *         @arg INA228_50_US: 50 us conversion time (000b)
  *         @arg INA228_84_US: 84 us conversion time (001b)
  *         @arg INA228_150_US: 150 us conversion time (010b)
  *         @arg INA228_280_US: 280 us conversion time (011b)
  *         @arg INA228_540_US: 540 us conversion time (100b, default)
  *         @arg INA228_1052_US: 1052 us conversion time (101b)
  *         @arg INA228_2074_US: 2074 us conversion time (110b)
  *         @arg INA228_4120_US: 4120 us conversion time (111b)
  * @return HAL status
  */
HAL_StatusTypeDef INA228_SetBusConvTime(INA228_HandleTypeDef *ina228, INA228_ConvTime_TypeDef convTime)
{
    uint16_t regValue = 0;

    if (INA228_GetAdcConfigurationReg(ina228, &regValue) == HAL_OK)
    {
        regValue &= ~INA228_VBUSCT; 
        regValue |= (convTime << INA228_VBUSCT_Pos) & INA228_VBUSCT; 
        return INA228_WriteRegister16(ina228, INA228_ADC_CONFIG_REG, regValue); 
    }

    return HAL_ERROR;  
}

/**
  * @brief  Set the operational mode of the ADC (Shutdown, Continuous, One-Shot).
  * @param  ina228 Pointer to a INA228_HandleTypeDef structure that contains
  *         the configuration information for connecting to the sensor.
  * @param  mode Selected operational conversion mode.
  *         This parameter can be one of the following values:
  *         @arg INA228_SHUTDOWN_MODE: Shutdown mode (0x0)
  *         @arg INA228_BUS_ONE_SHOT: Triggered bus voltage, single-shot (0x1)
  *         @arg INA228_SHUNT_ONE_SHOT: Triggered shunt voltage, single-shot (0x2)
  *         @arg INA228_SHUNT_BUS_ONE_SHOT: Triggered shunt and bus voltage, single-shot (0x3)
  *         @arg INA228_TEMP_ONE_SHOT: Triggered temperature, single-shot (0x4)
  *         @arg INA228_TEMP_BUS_ONE_SHOT: Triggered temperature and bus voltage, single-shot (0x5)
  *         @arg INA228_TEMP_SHUNT_ONE_SHOT: Triggered temperature and shunt voltage, single-shot (0x6)
  *         @arg INA228_TEMP_SHUNT_BUS_ONE_SHOT: Triggered bus, shunt voltage and temperature, single-shot (0x7)
  *         @arg INA228_BUS_CONTINUOUS: Continuous bus voltage only (0x9)
  *         @arg INA228_SHUNT_CONTINUOUS: Continuous shunt voltage only (0xA)
  *         @arg INA228_SHUNT_BUS_CONTINUOUS: Continuous shunt and bus voltage (0xB)
  *         @arg INA228_TEMP_CONTINUOUS: Continuous temperature only (0xC)
  *         @arg INA228_TEMP_BUS_CONTINUOUS: Continuous bus voltage and temperature (0xD)
  *         @arg INA228_TEMP_SHUNT_CONTINUOUS: Continuous temperature and shunt voltage (0xE)
  *         @arg INA228_TEMP_SHUNT_BUS_CONTINUOUS: Continuous bus, shunt voltage and temperature (default, 0xF)
  * @return HAL status
  */
HAL_StatusTypeDef INA228_SetMode(INA228_HandleTypeDef *ina228, INA228_Mode_TypeDef mode)
{
    uint16_t regValue = 0;

    if (INA228_GetAdcConfigurationReg(ina228, &regValue) == HAL_OK)
    {
        regValue &= ~INA228_MODE; 
        regValue |= (mode << INA228_MODE_Pos) & INA228_MODE; 
        return INA228_WriteRegister16(ina228, INA228_ADC_CONFIG_REG, regValue); 
    }

    return HAL_ERROR; 
}

/**
  * @brief  Calculate Current LSB and program the Shunt Calibration Register (0x02).
  * @param  ina228 Pointer to a INA228_HandleTypeDef structure that contains
  *         the configuration information for connecting to the sensor.
  * @param  rShuntValue Shunt resistance value in Ohms (e.g. 0.005 for 5 mOhm).
  * @param  maxCurrent Maximum expected current in Amperes (e.g. 6.0 A).
  * @return HAL status
  */
HAL_StatusTypeDef INA228_SetShuntCalibration(INA228_HandleTypeDef *ina228, double rShuntValue, double maxCurrent)
{
    if (ina228 == NULL || rShuntValue <= 0.0 || maxCurrent <= 0.0)
    {
        return HAL_ERROR;
    }

    ina228->_shuntResistor = rShuntValue;
    ina228->_maximumCurrent = maxCurrent;

    // Minimum Current LSB = Max_Current / 2^19
    double currentLsbMinimum = maxCurrent / 524288.0; 
    ina228->_currentLsbMin = currentLsbMinimum;

    // Clean 1-2-5 rounded Current LSB
    double roundedLsb = INA228_RoundCurrentLsb(currentLsbMinimum);
    ina228->_currentLsb = roundedLsb;

    // SHUNT_CAL = 13107.2 x 10^6 * CURRENT_LSB * RSHUNT
    double shuntCalDouble = 13107200000.0 * roundedLsb * rShuntValue;

    if (ina228->_adcRange == INA228_ADC_RANGE_40_96_MV)
    {
        shuntCalDouble *= 4.0;
    }

    uint16_t shuntCal = (uint16_t)((uint32_t)shuntCalDouble & 0x7FFF); // Mask 15 bits

    return INA228_WriteRegister16(ina228, INA228_SHUNT_CAL_REG, shuntCal);
}

/**
  * @brief  Configure Diagnostic Alert pin function (Conversion Ready flag trigger).
  * @param  ina228 Pointer to a INA228_HandleTypeDef structure that contains
  *         the configuration information for connecting to the sensor.
  * @param  cnvr Conversion ready alert pin enable selection.
  *         This parameter can be one of the following values:
  *         @arg INA228_DISABLE_CNVR_FLAG_ON_ALERT_PIN: Disable conversion ready flag on ALERT pin (0b)
  *         @arg INA228_ENABLE_CNVR_FLAG_ON_ALERT_PIN: Enable conversion ready flag on ALERT pin (1b)
  * @return HAL status
  */
HAL_StatusTypeDef INA228_SetAlertPin(INA228_HandleTypeDef *ina228, INA228_CNVR_TypeDef cnvr)
{
    uint16_t regValue = 0;
    HAL_StatusTypeDef status = INA228_GetDiagAlertReg(ina228, &regValue);

    if (status == HAL_OK)
    {
        regValue &= ~INA228_CNVR;
        regValue |= ((uint16_t)cnvr << INA228_CNVR_Pos) & INA228_CNVR;
        return INA228_WriteRegister16(ina228, INA228_DIAG_ALERT_REG, regValue);
    }

    return status;
}

/**
  * @brief  Get Diagnostic Alert pin function (Conversion Ready flag trigger).
  * @param  ina228 Pointer to a INA228_HandleTypeDef structure that contains
  *         the configuration information for connecting to the sensor.
  * @param  cnvr Pointer to store conversion ready alert pin enable setting.
  *         The output parameter will be populated with one of the following values:
  *         @arg INA228_DISABLE_CNVR_FLAG_ON_ALERT_PIN: Disable conversion ready flag on ALERT pin (0b)
  *         @arg INA228_ENABLE_CNVR_FLAG_ON_ALERT_PIN: Enable conversion ready flag on ALERT pin (1b)
  * @return HAL status
  */
HAL_StatusTypeDef INA228_GetAlertPin(INA228_HandleTypeDef *ina228, INA228_CNVR_TypeDef *cnvr)
{
    if (cnvr == NULL) return HAL_ERROR;

    uint16_t regValue = 0;
    HAL_StatusTypeDef status = INA228_GetDiagAlertReg(ina228, &regValue);

    if (status == HAL_OK)
    {
        *cnvr = (INA228_CNVR_TypeDef)( (regValue & INA228_CNVR) >> INA228_CNVR_Pos );
    }

    return status;    
}

/**
  * @brief  Get the current ADC full-scale differential range setting.
  * @param  ina228 Pointer to a INA228_HandleTypeDef structure that contains
  *         the configuration information for connecting to the sensor.
  * @param  adcRange Pointer to store the current ADC full-scale range option.
  *         The output parameter will be populated with one of the following values:
  *         @arg INA228_ADC_RANGE_163_84_MV: ±163.84 mV full-scale range (0b)
  *         @arg INA228_ADC_RANGE_40_96_MV: ±40.96 mV full-scale range (1b)
  * @return HAL status
  */
HAL_StatusTypeDef INA228_GetAdcRange(INA228_HandleTypeDef *ina228, INA228_AdcRange_TypeDef *adcRange)
{
    if (adcRange == NULL) return HAL_ERROR;

    uint16_t regValue = 0;
    HAL_StatusTypeDef status = INA228_GetConfigurationReg(ina228, &regValue);

    if (status == HAL_OK)
    {
        *adcRange = (INA228_AdcRange_TypeDef)( (regValue & INA228_ADCRANGE) >> INA228_ADCRANGE_Pos );
    }

    return status;
}

/**
  * @brief  Get the current Shunt Temperature Compensation setting.
  * @param  ina228 Pointer to a INA228_HandleTypeDef structure that contains
  *         the configuration information for connecting to the sensor.
  * @param  tempComp Pointer to store temperature compensation setting.
  *         The output parameter will be populated with one of the following values:
  *         @arg INA228_TEMP_COMP_DISABLED: Temperature compensation disabled (0b)
  *         @arg INA228_TEMPERATURE_COMP_ENABLED: Temperature compensation enabled (1b)
  * @return HAL status
  */
HAL_StatusTypeDef INA228_GetTempComp(INA228_HandleTypeDef *ina228, INA228_TempComp_TypeDef *tempComp)
{
    if (tempComp == NULL) return HAL_ERROR;

    uint16_t regValue = 0;
    HAL_StatusTypeDef status = INA228_GetConfigurationReg(ina228, &regValue);

    if (status == HAL_OK)
    {
        *tempComp = (INA228_TempComp_TypeDef)( (regValue & INA228_TEMPCOMP) >> INA228_TEMPCOMP_Pos );
    }

    return status;
}

/**
  * @brief  Get the current initial Conversion Delay setting.
  * @param  ina228 Pointer to a INA228_HandleTypeDef structure that contains
  *         the configuration information for connecting to the sensor.
  * @param  convDelay Pointer to store conversion delay option.
  *         The output parameter will be populated with one of the following values:
  *         @arg INA228_DELAY_0_MS: 0 ms delay (0x00)
  *         @arg INA228_DELAY_2_MS: 2 ms delay (0x01)
  *         @arg INA228_DELAY_510_MS: 510 ms delay (0xFF)
  * @return HAL status
  */
HAL_StatusTypeDef INA228_GetConversionDelay(INA228_HandleTypeDef *ina228, INA228_ConvDelay_TypeDef *convDelay)
{
    if (convDelay == NULL) return HAL_ERROR;

    uint16_t regValue = 0;
    HAL_StatusTypeDef status = INA228_GetConfigurationReg(ina228, &regValue);

    if (status == HAL_OK)
    {
        *convDelay = (INA228_ConvDelay_TypeDef)( (regValue & INA228_CONVDLY) >> INA228_CONVDLY_Pos );
    }

    return status;
}

/**
  * @brief  Get the current Sample Averaging count.
  * @param  ina228 Pointer to a INA228_HandleTypeDef structure that contains
  *         the configuration information for connecting to the sensor.
  * @param  avg Pointer to store sample averaging setting.
  *         The output parameter will be populated with one of the following values:
  *         @arg INA228_1_SAMPLE: 1 sample (no averaging, 000b)
  *         @arg INA228_4_SAMPLES: 4 samples averaged (001b)
  *         @arg INA228_16_SAMPLES: 16 samples averaged (010b)
  *         @arg INA228_64_SAMPLES: 64 samples averaged (011b)
  *         @arg INA228_128_SAMPLES: 128 samples averaged (100b)
  *         @arg INA228_256_SAMPLES: 256 samples averaged (101b)
  *         @arg INA228_512_SAMPLES: 512 samples averaged (110b)
  *         @arg INA228_1024_SAMPLES: 1024 samples averaged (111b)
  * @return HAL status
  */
HAL_StatusTypeDef INA228_GetAverage(INA228_HandleTypeDef *ina228, INA228_Average_TypeDef *avg)
{
    if (avg == NULL) return HAL_ERROR;

    uint16_t regValue = 0;
    HAL_StatusTypeDef status = INA228_GetAdcConfigurationReg(ina228, &regValue);

    if (status == HAL_OK)
    {
        *avg = (INA228_Average_TypeDef)( (regValue & INA228_AVG) >> INA228_AVG_Pos );
    }

    return status;   
}

/**
  * @brief  Get the current Temperature Conversion Time.
  * @param  ina228 Pointer to a INA228_HandleTypeDef structure that contains
  *         the configuration information for connecting to the sensor.
  * @param  convTime Pointer to store temperature conversion time setting.
  *         The output parameter will be populated with one of the following values:
  *         @arg INA228_50_US: 50 us conversion time (000b)
  *         @arg INA228_84_US: 84 us conversion time (001b)
  *         @arg INA228_150_US: 150 us conversion time (010b)
  *         @arg INA228_280_US: 280 us conversion time (011b)
  *         @arg INA228_540_US: 540 us conversion time (100b)
  *         @arg INA228_1052_US: 1052 us conversion time (101b)
  *         @arg INA228_2074_US: 2074 us conversion time (110b)
  *         @arg INA228_4120_US: 4120 us conversion time (111b)
  * @return HAL status
  */
HAL_StatusTypeDef INA228_GetTempConvTime(INA228_HandleTypeDef *ina228, INA228_ConvTime_TypeDef *convTime)
{
    if (convTime == NULL) return HAL_ERROR;

    uint16_t regValue = 0;
    HAL_StatusTypeDef status = INA228_GetAdcConfigurationReg(ina228, &regValue);

    if (status == HAL_OK)
    {
        *convTime = (INA228_ConvTime_TypeDef)( (regValue & INA228_VTCT) >> INA228_VTCT_Pos );
    }

    return status;     
}

/**
  * @brief  Get the current Shunt Conversion Time.
  * @param  ina228 Pointer to a INA228_HandleTypeDef structure that contains
  *         the configuration information for connecting to the sensor.
  * @param  convTime Pointer to store shunt conversion time setting.
  *         The output parameter will be populated with one of the following values:
  *         @arg INA228_50_US: 50 us conversion time (000b)
  *         @arg INA228_84_US: 84 us conversion time (001b)
  *         @arg INA228_150_US: 150 us conversion time (010b)
  *         @arg INA228_280_US: 280 us conversion time (011b)
  *         @arg INA228_540_US: 540 us conversion time (100b)
  *         @arg INA228_1052_US: 1052 us conversion time (101b)
  *         @arg INA228_2074_US: 2074 us conversion time (110b)
  *         @arg INA228_4120_US: 4120 us conversion time (111b)
  * @return HAL status
  */
HAL_StatusTypeDef INA228_GetShuntConvTime(INA228_HandleTypeDef *ina228, INA228_ConvTime_TypeDef *convTime)
{
    if (convTime == NULL) return HAL_ERROR;

    uint16_t regValue = 0;
    HAL_StatusTypeDef status = INA228_GetAdcConfigurationReg(ina228, &regValue);

    if (status == HAL_OK)
    {
        *convTime = (INA228_ConvTime_TypeDef)( (regValue & INA228_VSHCT) >> INA228_VSHCT_Pos );
    }

    return status;      
}

/**
  * @brief  Get the current Bus Conversion Time.
  * @param  ina228 Pointer to a INA228_HandleTypeDef structure that contains
  *         the configuration information for connecting to the sensor.
  * @param  convTime Pointer to store bus conversion time setting.
  *         The output parameter will be populated with one of the following values:
  *         @arg INA228_50_US: 50 us conversion time (000b)
  *         @arg INA228_84_US: 84 us conversion time (001b)
  *         @arg INA228_150_US: 150 us conversion time (010b)
  *         @arg INA228_280_US: 280 us conversion time (011b)
  *         @arg INA228_540_US: 540 us conversion time (100b)
  *         @arg INA228_1052_US: 1052 us conversion time (101b)
  *         @arg INA228_2074_US: 2074 us conversion time (110b)
  *         @arg INA228_4120_US: 4120 us conversion time (111b)
  * @return HAL status
  */
HAL_StatusTypeDef INA228_GetBusConvTime(INA228_HandleTypeDef *ina228, INA228_ConvTime_TypeDef *convTime)
{
    if (convTime == NULL) return HAL_ERROR;

    uint16_t regValue = 0;
    HAL_StatusTypeDef status = INA228_GetAdcConfigurationReg(ina228, &regValue);

    if (status == HAL_OK)
    {
        *convTime = (INA228_ConvTime_TypeDef)( (regValue & INA228_VBUSCT) >> INA228_VBUSCT_Pos );
    }

    return status;     
}

/**
  * @brief  Get the current ADC Operational Mode.
  * @param  ina228 Pointer to a INA228_HandleTypeDef structure that contains
  *         the configuration information for connecting to the sensor.
  * @param  mode Pointer to store current operational conversion mode.
  *         The output parameter will be populated with one of the following values:
  *         @arg INA228_SHUTDOWN_MODE: Shutdown mode (0x0)
  *         @arg INA228_BUS_ONE_SHOT: Triggered bus voltage, single-shot (0x1)
  *         @arg INA228_SHUNT_ONE_SHOT: Triggered shunt voltage, single-shot (0x2)
  *         @arg INA228_SHUNT_BUS_ONE_SHOT: Triggered shunt and bus voltage, single-shot (0x3)
  *         @arg INA228_TEMP_ONE_SHOT: Triggered temperature, single-shot (0x4)
  *         @arg INA228_TEMP_BUS_ONE_SHOT: Triggered temperature and bus voltage, single-shot (0x5)
  *         @arg INA228_TEMP_SHUNT_ONE_SHOT: Triggered temperature and shunt voltage, single-shot (0x6)
  *         @arg INA228_TEMP_SHUNT_BUS_ONE_SHOT: Triggered bus, shunt voltage and temperature, single-shot (0x7)
  *         @arg INA228_BUS_CONTINUOUS: Continuous bus voltage only (0x9)
  *         @arg INA228_SHUNT_CONTINUOUS: Continuous shunt voltage only (0xA)
  *         @arg INA228_SHUNT_BUS_CONTINUOUS: Continuous shunt and bus voltage (0xB)
  *         @arg INA228_TEMP_CONTINUOUS: Continuous temperature only (0xC)
  *         @arg INA228_TEMP_BUS_CONTINUOUS: Continuous bus voltage and temperature (0xD)
  *         @arg INA228_TEMP_SHUNT_CONTINUOUS: Continuous temperature and shunt voltage (0xE)
  *         @arg INA228_TEMP_SHUNT_BUS_CONTINUOUS: Continuous bus, shunt voltage and temperature (0xF)
  * @return HAL status
  */
HAL_StatusTypeDef INA228_GetMode(INA228_HandleTypeDef *ina228, INA228_Mode_TypeDef *mode)
{
    if (mode == NULL) return HAL_ERROR;

    uint16_t regValue = 0;
    HAL_StatusTypeDef status = INA228_GetAdcConfigurationReg(ina228, &regValue);

    if (status == HAL_OK)
    {
        *mode = (INA228_Mode_TypeDef)( (regValue & INA228_MODE) >> INA228_MODE_Pos );
    }

    return status;      
}

/**
  * @brief  Read the internal Die Temperature in Celsius degrees (°C).
  * @param  ina228 Pointer to a INA228_HandleTypeDef structure that contains
  *         the configuration information for connecting to the sensor.
  * @param  temp Pointer to store calculated temperature (°C).
  * @return HAL status
  */
HAL_StatusTypeDef INA228_GetDieTemp_C(INA228_HandleTypeDef *ina228, double *temp)
{
    if (temp == NULL) return HAL_ERROR;

    uint16_t regValue = 0;
    HAL_StatusTypeDef status = INA228_GetDieTempReg(ina228, &regValue);

    if (status == HAL_OK)
    {
        int16_t rawTemp = (int16_t)regValue;
        *temp = (double)rawTemp * 0.0078125;
    }

    return status;
}

/**
  * @brief  Read the differential Shunt Voltage in millivolts (mV).
  * @param  ina228 Pointer to a INA228_HandleTypeDef structure that contains
  *         the configuration information for connecting to the sensor.
  * @param  vShunt Pointer to store calculated shunt voltage (mV).
  * @return HAL status
  */
HAL_StatusTypeDef INA228_GetShuntVoltage_mV(INA228_HandleTypeDef *ina228, double *vShunt)
{
    if (vShunt == NULL) return HAL_ERROR;

    uint32_t regValue = 0;
    HAL_StatusTypeDef status = INA228_GetShuntVoltageReg(ina228, &regValue);

    if (status == HAL_OK)
    {
        int32_t rawVshunt = ((int32_t)(regValue << 8)) >> 12;
        *vShunt = (double)rawVshunt * (ina228->_resolution * 1000.0); // Convert V to mV
    }

    return status;
}

/**
  * @brief  Read the Bus Voltage in Volts (V).
  * @param  ina228 Pointer to a INA228_HandleTypeDef structure that contains
  *         the configuration information for connecting to the sensor.
  * @param  vBus Pointer to store calculated bus voltage (V).
  * @return HAL status
  */
HAL_StatusTypeDef INA228_GetBusVoltage_V(INA228_HandleTypeDef *ina228, double *vBus)
{
    if (vBus == NULL) return HAL_ERROR;

    uint32_t regValue = 0;
    HAL_StatusTypeDef status = INA228_GetBusVoltageReg(ina228, &regValue);

    if (status == HAL_OK)
    {
        uint32_t rawVbus = (regValue >> 4) & 0x000FFFFF;
        *vBus = (double)rawVbus * 0.0001953125; // 195.3125 uV/LSB
    }

    return status;
}

/**
  * @brief  Read the calculated Power in Watts (W).
  * @param  ina228 Pointer to a INA228_HandleTypeDef structure that contains
  *         the configuration information for connecting to the sensor.
  * @param  power Pointer to store calculated power (W).
  * @return HAL status
  */
HAL_StatusTypeDef INA228_GetPower_W(INA228_HandleTypeDef *ina228, double *power)
{
    if (power == NULL) return HAL_ERROR;

    uint32_t regValue = 0;
    HAL_StatusTypeDef status = INA228_GetPowerReg(ina228, &regValue);

    if (status == HAL_OK)
    {
        uint32_t rawPower = regValue & 0x00FFFFFF;
        *power = (double)rawPower * 3.2 * ina228->_currentLsb;
    }

    return status;
}

/**
  * @brief  Read the calculated Current in Amperes (A).
  * @param  ina228 Pointer to a INA228_HandleTypeDef structure that contains
  *         the configuration information for connecting to the sensor.
  * @param  current Pointer to store calculated current (A).
  * @return HAL status
  */
HAL_StatusTypeDef INA228_GetCurrent_A(INA228_HandleTypeDef *ina228, double *current)
{
    if (current == NULL) return HAL_ERROR;

    uint32_t regValue = 0;
    HAL_StatusTypeDef status = INA228_GetCurrentReg(ina228, &regValue);

    if (status == HAL_OK)
    {   
        int32_t rawCurrent = ((int32_t)(regValue << 8)) >> 12;
        *current = (double)rawCurrent * ina228->_currentLsb;
    }

    return status;
}

/**
  * @brief  Read the accumulated Energy in Joules (J).
  * @param  ina228 Pointer to a INA228_HandleTypeDef structure that contains
  *         the configuration information for connecting to the sensor.
  * @param  energy Pointer to store accumulated energy (J).
  * @return HAL status
  */
HAL_StatusTypeDef INA228_GetEnergy_J(INA228_HandleTypeDef *ina228, double *energy)
{
    if (energy == NULL) return HAL_ERROR;

    uint64_t regValue = 0;
    HAL_StatusTypeDef status = INA228_GetEnergyReg(ina228, &regValue);

    if (status == HAL_OK)
    {
        uint64_t rawEnergy = regValue & 0x000000FFFFFFFFFFULL;
        *energy = (double)rawEnergy * 16.0 * 3.2 * ina228->_currentLsb;
    }

    return status; 
}

/**
  * @brief  Read the accumulated Charge in Coulombs (C = A*s).
  * @param  ina228 Pointer to a INA228_HandleTypeDef structure that contains
  *         the configuration information for connecting to the sensor.
  * @param  charge Pointer to store accumulated charge (Coulombs).
  * @return HAL status
  */
HAL_StatusTypeDef INA228_GetCharge_Coulomb(INA228_HandleTypeDef *ina228, double *charge)
{
    if (charge == NULL) return HAL_ERROR;

    uint64_t regValue = 0;
    HAL_StatusTypeDef status = INA228_GetChargeReg(ina228, &regValue);

    if (status == HAL_OK)
    {
        int64_t rawCharge = ((int64_t)(regValue << 24)) >> 24;
        *charge = (double)rawCharge * ina228->_currentLsb;
    }

    return status;    
}

/**
  * @brief  Read the accumulated Charge converted to Milliampere-hours (mAh).
  * @param  ina228 Pointer to a INA228_HandleTypeDef structure that contains
  *         the configuration information for connecting to the sensor.
  * @param  charge_mAh Pointer to store accumulated charge (mAh).
  * @return HAL status
  */
HAL_StatusTypeDef INA228_GetCharge_mAh(INA228_HandleTypeDef *ina228, double *charge_mAh)
{
    double charge_coulomb = 0.0;
    HAL_StatusTypeDef status = INA228_GetCharge_Coulomb(ina228, &charge_coulomb);

    if (status == HAL_OK && charge_mAh != NULL)
    {
        *charge_mAh = charge_coulomb / 3.6; // 1 mAh = 3.6 Coulombs
    }

    return status;
}

/**
  * @brief  Reset Energy and Charge accumulators (sets RSTACC bit in CONFIG_REG).
  * @param  ina228 Pointer to a INA228_HandleTypeDef structure that contains
  *         the configuration information for connecting to the sensor.
  * @return HAL status
  */
HAL_StatusTypeDef INA228_ResetEnergyAndCharge(INA228_HandleTypeDef *ina228)
{
    uint16_t regValue = 0;
    HAL_StatusTypeDef status = INA228_GetConfigurationReg(ina228, &regValue);

    if (status != HAL_OK) return status;

    regValue |= INA228_RSTACC;
    status = INA228_WriteRegister16(ina228, INA228_CONFIG_REG, regValue);

    if (status != HAL_OK) return status;

    uint32_t startTick = HAL_GetTick();
    const uint32_t timeout = 10;
    
    do 
    {
        status = INA228_GetConfigurationReg(ina228, &regValue);
        if (status != HAL_OK) return status;

        if (!(regValue & INA228_RSTACC))
        {
            return HAL_OK;
        }
    } while ((HAL_GetTick() - startTick) < timeout);

    return HAL_TIMEOUT;
}

/**
  * @brief  Perform software reset of the INA228 device (sets RST bit in CONFIG_REG).
  * @param  ina228 Pointer to a INA228_HandleTypeDef structure that contains
  *         the configuration information for connecting to the sensor.
  * @return HAL status
  */
HAL_StatusTypeDef INA228_ResetDevice(INA228_HandleTypeDef *ina228)
{
    uint16_t regValue = INA228_RST;
    HAL_StatusTypeDef status = INA228_WriteRegister16(ina228, INA228_CONFIG_REG, regValue);

    if (status != HAL_OK) return status;

    uint32_t startTick = HAL_GetTick();
    const uint32_t timeout = 100;

    do {
        status = INA228_GetConfigurationReg(ina228, &regValue);
        if (status != HAL_OK) return status;

        if (!(regValue & INA228_RST))
        {
            return HAL_OK;
        }
    } while ((HAL_GetTick() - startTick) < timeout);

    return HAL_TIMEOUT;
}

/**
  * @brief  Configure Diagnostic Alert pin output polarity (Active Low / Active High).
  * @param  ina228 Pointer to a INA228_HandleTypeDef structure that contains
  *         the configuration information for connecting to the sensor.
  * @param  pol Alert pin polarity selection.
  *         This parameter can be one of the following values:
  *         @arg INA228_ALERT_ACTIVE_LOW: Alert pin output active low (default, 0x0000)
  *         @arg INA228_ALERT_ACTIVE_HIGH: Alert pin output active high (0x1000)
  * @return HAL status
  */
HAL_StatusTypeDef INA228_SetAlertPinPolarity(INA228_HandleTypeDef *ina228, INA228_AlertPinPol_TypeDef pol)
{
    uint16_t regValue = 0;
    HAL_StatusTypeDef status = INA228_GetDiagAlertReg(ina228, &regValue);

    if (status == HAL_OK)
    {
        regValue &= ~INA228_APOL;
        regValue |= ((uint16_t)pol << INA228_APOL_Pos) & INA228_APOL;
        return INA228_WriteRegister16(ina228, INA228_DIAG_ALERT_REG, regValue);
    }

    return status;
}

/**
  * @brief  Get Diagnostic Alert pin output polarity setting.
  * @param  ina228 Pointer to a INA228_HandleTypeDef structure that contains
  *         the configuration information for connecting to the sensor.
  * @param  pol Pointer to store Alert pin polarity setting.
  *         The output parameter will be populated with one of the following values:
  *         @arg INA228_ALERT_ACTIVE_LOW: Alert pin output active low (0x0000)
  *         @arg INA228_ALERT_ACTIVE_HIGH: Alert pin output active high (0x1000)
  * @return HAL status
  */
HAL_StatusTypeDef INA228_GetAlertPinPolarity(INA228_HandleTypeDef *ina228, INA228_AlertPinPol_TypeDef *pol)
{
    if (pol == NULL) return HAL_ERROR;

    uint16_t regValue = 0;
    HAL_StatusTypeDef status = INA228_GetDiagAlertReg(ina228, &regValue);

    if (status == HAL_OK)
    {
        *pol = (INA228_AlertPinPol_TypeDef)( (regValue & INA228_APOL) >> INA228_APOL_Pos );
    }

    return status;
}

/**
  * @brief  Configure SLOWALERT mode for the ALERT function.
  *         When enabled, ALERT function is asserted on the completed averaged value.
  * @param  ina228 Pointer to a INA228_HandleTypeDef structure that contains
  *         the configuration information for connecting to the sensor.
  * @param  slowAlert Selected alert comparison timing mode.
  *         This parameter can be one of the following values:
  *         @arg INA228_ALERT_COMPARISON_NON_AVERAGED: ALERT comparison on non-averaged ADC value (default, 0b)
  *         @arg INA228_ALERT_COMPARISON_AVERAGED: ALERT comparison on averaged value (1b)
  * @return HAL status
  */
HAL_StatusTypeDef INA228_SetSlowAlert(INA228_HandleTypeDef *ina228, INA228_SlowAlert_TypeDef slowAlert)
{
    uint16_t regValue = 0;
    HAL_StatusTypeDef status = INA228_GetDiagAlertReg(ina228, &regValue);

    if (status == HAL_OK)
    {
        regValue &= ~INA228_SLOWALERT;
        regValue |= ((uint16_t)slowAlert << INA228_SLOWALERT_Pos) & INA228_SLOWALERT;
        return INA228_WriteRegister16(ina228, INA228_DIAG_ALERT_REG, regValue);
    }

    return status;
}

/**
  * @brief  Configure Diagnostic Alert latch mode (Transparent or Latched).
  * @param  ina228 Pointer to a INA228_HandleTypeDef structure that contains
  *         the configuration information for connecting to the sensor.
  * @param  latch Selected Alert latch mode.
  *         This parameter can be one of the following values:
  *         @arg INA228_ALERT_LATCH_TRANSPARENT: Alert pin/flag reset when fault clears (default, 0b)
  *         @arg INA228_ALERT_LATCH_ENABLED: Alert pin/flag remain latched until DIAG_ALRT read (1b)
  * @return HAL status
  */
HAL_StatusTypeDef INA228_SetAlertLatch(INA228_HandleTypeDef *ina228, INA228_AlertLatch_TypeDef latch)
{
    uint16_t regValue = 0;
    HAL_StatusTypeDef status = INA228_GetDiagAlertReg(ina228, &regValue);

    if (status == HAL_OK)
    {
        regValue &= ~INA228_ALATCH;
        regValue |= ((uint16_t)latch << INA228_ALATCH_Pos) & INA228_ALATCH;
        return INA228_WriteRegister16(ina228, INA228_DIAG_ALERT_REG, regValue);
    }

    return status;
}

/**
  * @brief  Get Diagnostic Alert latch mode setting.
  * @param  ina228 Pointer to a INA228_HandleTypeDef structure that contains
  *         the configuration information for connecting to the sensor.
  * @param  latch Pointer to store Alert latch mode setting.
  *         The output parameter will be populated with one of the following values:
  *         @arg INA228_ALERT_LATCH_TRANSPARENT: Alert pin/flag reset when fault clears (0b)
  *         @arg INA228_ALERT_LATCH_ENABLED: Alert pin/flag remain latched until DIAG_ALRT read (1b)
  * @return HAL status
  */
HAL_StatusTypeDef INA228_GetAlertLatch(INA228_HandleTypeDef *ina228, INA228_AlertLatch_TypeDef *latch)
{
    if (latch == NULL) return HAL_ERROR;

    uint16_t regValue = 0;
    HAL_StatusTypeDef status = INA228_GetDiagAlertReg(ina228, &regValue);

    if (status == HAL_OK)
    {
        *latch = (INA228_AlertLatch_TypeDef)( (regValue & INA228_ALATCH) >> INA228_ALATCH_Pos );
    }

    return status;
}

/**
  * @brief  Get SLOWALERT mode setting for the ALERT function.
  * @param  ina228 Pointer to a INA228_HandleTypeDef structure that contains
  *         the configuration information for connecting to the sensor.
  * @param  slowAlert Pointer to store SLOWALERT mode setting.
  *         The output parameter will be populated with one of the following values:
  *         @arg INA228_ALERT_COMPARISON_NON_AVERAGED: ALERT comparison on non-averaged ADC value (0b)
  *         @arg INA228_ALERT_COMPARISON_AVERAGED: ALERT comparison on averaged value (1b)
  * @return HAL status
  */
HAL_StatusTypeDef INA228_GetSlowAlert(INA228_HandleTypeDef *ina228, INA228_SlowAlert_TypeDef *slowAlert)
{
    if (slowAlert == NULL) return HAL_ERROR;

    uint16_t regValue = 0;
    HAL_StatusTypeDef status = INA228_GetDiagAlertReg(ina228, &regValue);

    if (status == HAL_OK)
    {
        *slowAlert = (INA228_SlowAlert_TypeDef)( (regValue & INA228_SLOWALERT) >> INA228_SLOWALERT_Pos );
    }

    return status;
}

/**
  * @brief  Read the Energy Register Overflow flag (ENERGYOF) from DIAG_ALRT register.
  * @param  ina228 Pointer to a INA228_HandleTypeDef structure that contains
  *         the configuration information for connecting to the sensor.
  * @param  overflow Pointer to store energy register overflow status.
  *         The output parameter will be populated with one of the following values:
  *         @arg INA228_ENERGY_REGISTER_NORMAL: Energy register normal, no overflow (0b)
  *         @arg INA228_ENERGY_REGISTER_OVERFLOW: Energy 40-bit register overflowed (1b)
  * @return HAL status
  */
HAL_StatusTypeDef INA228_GetEnergyOverflowFlag(INA228_HandleTypeDef *ina228, INA228_EnergyOverflow_TypeDef *overflow)
{
    if (overflow == NULL) return HAL_ERROR;

    uint16_t regValue = 0;
    HAL_StatusTypeDef status = INA228_GetDiagAlertReg(ina228, &regValue);

    if (status == HAL_OK)
    {
        *overflow = (INA228_EnergyOverflow_TypeDef)( (regValue & INA228_ENERGYOF) >> INA228_ENERGYOF_Pos );
    }

    return status;
}

/**
  * @brief  Read the Charge Register Overflow flag (CHARGEOF) from DIAG_ALRT register.
  * @param  ina228 Pointer to a INA228_HandleTypeDef structure that contains
  *         the configuration information for connecting to the sensor.
  * @param  overflow Pointer to store charge register overflow status.
  *         The output parameter will be populated with one of the following values:
  *         @arg INA228_CHARGE_REGISTER_NORMAL: Charge register normal, no overflow (0b)
  *         @arg INA228_CHARGE_REGISTER_OVERFLOW: Charge 40-bit register overflowed (1b)
  * @return HAL status
  */
HAL_StatusTypeDef INA228_GetChargeOverflowFlag(INA228_HandleTypeDef *ina228, INA228_ChargeOverflow_TypeDef *overflow)
{
    if (overflow == NULL) return HAL_ERROR;

    uint16_t regValue = 0;
    HAL_StatusTypeDef status = INA228_GetDiagAlertReg(ina228, &regValue);

    if (status == HAL_OK)
    {
        *overflow = (INA228_ChargeOverflow_TypeDef)( (regValue & INA228_CHARGEOF) >> INA228_CHARGEOF_Pos );
    }

    return status;
}

/**
  * @brief  Read the Math Overflow flag (MATHOF) from DIAG_ALRT register.
  * @param  ina228 Pointer to a INA228_HandleTypeDef structure that contains
  *         the configuration information for connecting to the sensor.
  * @param  overflow Pointer to store math operation overflow status.
  *         The output parameter will be populated with one of the following values:
  *         @arg INA228_MATH_NORMAL: Arithmetic calculations normal (0b)
  *         @arg INA228_MATH_OVERFLOW: Math calculation overflow error (1b)
  * @return HAL status
  */
HAL_StatusTypeDef INA228_GetMathOverflowFlag(INA228_HandleTypeDef *ina228, INA228_MathOverflow_TypeDef *overflow)
{
    if (overflow == NULL) return HAL_ERROR;

    uint16_t regValue = 0;
    HAL_StatusTypeDef status = INA228_GetDiagAlertReg(ina228, &regValue);

    if (status == HAL_OK)
    {
        *overflow = (INA228_MathOverflow_TypeDef)( (regValue & INA228_MATHOF) >> INA228_MATHOF_Pos );
    }

    return status;
}

/**
  * @brief  Read the Temperature Over-Limit flag (TMPOL) from DIAG_ALRT register.
  * @param  ina228 Pointer to a INA228_HandleTypeDef structure that contains
  *         the configuration information for connecting to the sensor.
  * @param  flag Pointer to store temperature over-limit event status.
  *         The output parameter will be populated with one of the following values:
  *         @arg INA228_TEMP_LIMIT_NORMAL: Temperature within normal range (0b)
  *         @arg INA228_TEMP_OVER_LIMIT_EVENT: Temperature exceeded limit (1b)
  * @return HAL status
  */
HAL_StatusTypeDef INA228_GetTempOverLimitFlag(INA228_HandleTypeDef *ina228, INA228_TempOverLimit_TypeDef *flag)
{
    if (flag == NULL) return HAL_ERROR;

    uint16_t regValue = 0;
    HAL_StatusTypeDef status = INA228_GetDiagAlertReg(ina228, &regValue);

    if (status == HAL_OK)
    {
        *flag = (INA228_TempOverLimit_TypeDef)( (regValue & INA228_TMPOL) >> INA228_TMPOL_Pos );
    }

    return status;
}

/**
  * @brief  Read the Shunt Voltage Over-Limit flag (SHNTOL) from DIAG_ALRT register.
  * @param  ina228 Pointer to a INA228_HandleTypeDef structure that contains
  *         the configuration information for connecting to the sensor.
  * @param  flag Pointer to store shunt over-limit event status.
  *         The output parameter will be populated with one of the following values:
  *         @arg INA228_SHUNT_LIMIT_NORMAL: Shunt voltage within upper threshold (0b)
  *         @arg INA228_SHUNT_OVER_LIMIT_EVENT: Shunt voltage exceeded over-limit threshold (1b)
  * @return HAL status
  */
HAL_StatusTypeDef INA228_GetShuntOverLimitFlag(INA228_HandleTypeDef *ina228, INA228_ShuntOverLimit_TypeDef *flag)
{
    if (flag == NULL) return HAL_ERROR;

    uint16_t regValue = 0;
    HAL_StatusTypeDef status = INA228_GetDiagAlertReg(ina228, &regValue);

    if (status == HAL_OK)
    {
        *flag = (INA228_ShuntOverLimit_TypeDef)( (regValue & INA228_SHUNTOL) >> INA228_SHUNTOL_Pos );
    }

    return status;
}

/**
  * @brief  Read the Shunt Voltage Under-Limit flag (SHNTUL) from DIAG_ALRT register.
  * @param  ina228 Pointer to a INA228_HandleTypeDef structure that contains
  *         the configuration information for connecting to the sensor.
  * @param  flag Pointer to store shunt under-limit event status.
  *         The output parameter will be populated with one of the following values:
  *         @arg INA228_SHUNT_UNDER_LIMIT_NORMAL: Shunt voltage above lower threshold (0b)
  *         @arg INA228_SHUNT_UNDER_LIMIT_EVENT: Shunt voltage fell below under-limit threshold (1b)
  * @return HAL status
  */
HAL_StatusTypeDef INA228_GetShuntUnderLimitFlag(INA228_HandleTypeDef *ina228, INA228_ShuntUnderLimit_TypeDef *flag)
{
    if (flag == NULL) return HAL_ERROR;

    uint16_t regValue = 0;
    HAL_StatusTypeDef status = INA228_GetDiagAlertReg(ina228, &regValue);

    if (status == HAL_OK)
    {
        *flag = (INA228_ShuntUnderLimit_TypeDef)( (regValue & INA228_SHUNTUL) >> INA228_SHUNTUL_Pos );
    }

    return status;
}

/**
  * @brief  Read the Bus Voltage Over-Limit flag (BUSOL) from DIAG_ALRT register.
  * @param  ina228 Pointer to a INA228_HandleTypeDef structure that contains
  *         the configuration information for connecting to the sensor.
  * @param  flag Pointer to store bus over-limit event status.
  *         The output parameter will be populated with one of the following values:
  *         @arg INA228_BUS_LIMIT_NORMAL: Bus voltage within upper threshold (0b)
  *         @arg INA228_BUS_OVER_LIMIT_EVENT: Bus voltage exceeded over-limit threshold (1b)
  * @return HAL status
  */
HAL_StatusTypeDef INA228_GetBusOverLimitFlag(INA228_HandleTypeDef *ina228, INA228_BusOverLimit_TypeDef *flag)
{
    if (flag == NULL) return HAL_ERROR;

    uint16_t regValue = 0;
    HAL_StatusTypeDef status = INA228_GetDiagAlertReg(ina228, &regValue);

    if (status == HAL_OK)
    {
        *flag = (INA228_BusOverLimit_TypeDef)( (regValue & INA228_BUSOL) >> INA228_BUSOL_Pos );
    }

    return status;
}

/**
  * @brief  Read the Bus Voltage Under-Limit flag (BUSUL) from DIAG_ALRT register.
  * @param  ina228 Pointer to a INA228_HandleTypeDef structure that contains
  *         the configuration information for connecting to the sensor.
  * @param  flag Pointer to store bus under-limit event status.
  *         The output parameter will be populated with one of the following values:
  *         @arg INA228_BUS_UNDER_LIMIT_NORMAL: Bus voltage above lower threshold (0b)
  *         @arg INA228_BUS_UNDER_LIMIT_EVENT: Bus voltage fell below under-limit threshold (1b)
  * @return HAL status
  */
HAL_StatusTypeDef INA228_GetBusUnderLimitFlag(INA228_HandleTypeDef *ina228, INA228_BusUnderLimit_TypeDef *flag)
{
    if (flag == NULL) return HAL_ERROR;

    uint16_t regValue = 0;
    HAL_StatusTypeDef status = INA228_GetDiagAlertReg(ina228, &regValue);

    if (status == HAL_OK)
    {
        *flag = (INA228_BusUnderLimit_TypeDef)( (regValue & INA228_BUSUL) >> INA228_BUSUL_Pos );
    }

    return status;
}

/**
  * @brief  Read the Power Over-Limit flag (POL) from DIAG_ALRT register.
  * @param  ina228 Pointer to a INA228_HandleTypeDef structure that contains
  *         the configuration information for connecting to the sensor.
  * @param  flag Pointer to store power over-limit event status.
  *         The output parameter will be populated with one of the following values:
  *         @arg INA228_POWER_LIMIT_NORMAL: Power measurement within limit (0b)
  *         @arg INA228_POWER_OVER_LIMIT_EVENT: Power measurement exceeded limit (1b)
  * @return HAL status
  */
HAL_StatusTypeDef INA228_GetPowerOverLimitFlag(INA228_HandleTypeDef *ina228, INA228_PowerOverLimit_TypeDef *flag)
{
    if (flag == NULL) return HAL_ERROR;

    uint16_t regValue = 0;
    HAL_StatusTypeDef status = INA228_GetDiagAlertReg(ina228, &regValue);

    if (status == HAL_OK)
    {
        *flag = (INA228_PowerOverLimit_TypeDef)( (regValue & INA228_POL) >> INA228_POL_Pos );
    }

    return status;
}

/**
  * @brief  Read the Conversion Ready flag (CNVRF) from DIAG_ALRT register.
  * @param  ina228 Pointer to a INA228_HandleTypeDef structure that contains
  *         the configuration information for connecting to the sensor.
  * @param  flag Pointer to store conversion ready status.
  *         The output parameter will be populated with one of the following values:
  *         @arg INA228_CONVERSION_NOT_READY: Conversion in progress or not complete (0b)
  *         @arg INA228_CONVERSION_READY: Conversion cycle has completed (1b)
  * @return HAL status
  */
HAL_StatusTypeDef INA228_GetConversionReadyFlag(INA228_HandleTypeDef *ina228, INA228_ConvReadyFlag_TypeDef *flag)
{
    if (flag == NULL) return HAL_ERROR;

    uint16_t regValue = 0;
    HAL_StatusTypeDef status = INA228_GetDiagAlertReg(ina228, &regValue);

    if (status == HAL_OK)
    {
        *flag = (INA228_ConvReadyFlag_TypeDef)( (regValue & INA228_CNVRF) >> INA228_CNVRF_Pos );
    }

    return status;
}

/**
  * @brief  Read the Diagnostic Memory Checksum status (MEMSTAT) from DIAG_ALRT register.
  * @param  ina228 Pointer to a INA228_HandleTypeDef structure that contains
  *         the configuration information for connecting to the sensor.
  * @param  status Pointer to store trim memory checksum status.
  *         The output parameter will be populated with one of the following values:
  *         @arg INA228_MEMORY_CHECKSUM_ERROR: Memory checksum error detected (0b)
  *         @arg INA228_MEMORY_NORMAL_OPERATION: Normal memory operation (1b)
  * @return HAL status
  */
HAL_StatusTypeDef INA228_GetMemoryStatus(INA228_HandleTypeDef *ina228, INA228_MemStatus_TypeDef *status)
{
    if (status == NULL) return HAL_ERROR;

    uint16_t regValue = 0;
    HAL_StatusTypeDef diagStatus = INA228_GetDiagAlertReg(ina228, &regValue);

    if (diagStatus == HAL_OK)
    {
        *status = (INA228_MemStatus_TypeDef)( (regValue & INA228_MEMSTAT) >> INA228_MEMSTAT_Pos );
    }

    return diagStatus;
}

/**
  * @brief  Read Diagnostic and Alert Register (0x0B).
  * @param  ina228 Pointer to a INA228_HandleTypeDef structure that contains
  *         the configuration information for connecting to the sensor.
  * @param  diagAlert Pointer to store 16-bit diagnostic alert flags.
  * @return HAL status
  */
HAL_StatusTypeDef INA228_GetDiagAlert(INA228_HandleTypeDef *ina228, uint16_t *diagAlert)
{
    return INA228_ReadRegister16(ina228, INA228_DIAG_ALERT_REG, diagAlert);
}

/**
  * @brief  Set Shunt Overvoltage Threshold in millivolts (mV).
  * @param  ina228 Pointer to a INA228_HandleTypeDef structure.
  * @param  threshold_mV Target threshold in mV (e.g. 50.0 mV).
  *         Conversion factor is 5 uV/LSB when ADCRANGE = 0 (±163.84 mV),
  *         or 1.25 uV/LSB when ADCRANGE = 1 (±40.96 mV).
  * @return HAL status
  */
HAL_StatusTypeDef INA228_SetShuntOverVoltageThreshold_mV(INA228_HandleTypeDef *ina228, double threshold_mV)
{
    if (ina228 == NULL) return HAL_ERROR;

    double lsb_mV = (ina228->_adcRange == INA228_ADC_RANGE_40_96_MV) ? 0.00125 : 0.005;
    int32_t rawVal = (int32_t)round(threshold_mV / lsb_mV);

    if (rawVal > 32767) rawVal = 32767;
    if (rawVal < -32768) rawVal = -32768;

    return INA228_SetSOVLReg(ina228, (uint16_t)((int16_t)rawVal));
}

/**
  * @brief  Read Shunt Overvoltage Threshold in millivolts (mV).
  * @param  ina228 Pointer to a INA228_HandleTypeDef structure.
  * @param  threshold_mV Pointer to store calculated threshold in mV.
  * @return HAL status
  */
HAL_StatusTypeDef INA228_GetShuntOverVoltageThreshold_mV(INA228_HandleTypeDef *ina228, double *threshold_mV)
{
    if (ina228 == NULL || threshold_mV == NULL) return HAL_ERROR;

    uint16_t regVal = 0;
    HAL_StatusTypeDef status = INA228_GetSOVLReg(ina228, &regVal);

    if (status == HAL_OK)
    {
        int16_t rawSigned = (int16_t)regVal;
        double lsb_mV = (ina228->_adcRange == INA228_ADC_RANGE_40_96_MV) ? 0.00125 : 0.005;
        *threshold_mV = (double)rawSigned * lsb_mV;
    }

    return status;
}

/**
  * @brief  Set Shunt Undervoltage Threshold in millivolts (mV).
  * @param  ina228 Pointer to a INA228_HandleTypeDef structure.
  * @param  threshold_mV Target threshold in mV (e.g. -10.0 mV).
  *         Conversion factor is 5 uV/LSB when ADCRANGE = 0 (±163.84 mV),
  *         or 1.25 uV/LSB when ADCRANGE = 1 (±40.96 mV).
  * @return HAL status
  */
HAL_StatusTypeDef INA228_SetShuntUnderVoltageThreshold_mV(INA228_HandleTypeDef *ina228, double threshold_mV)
{
    if (ina228 == NULL) return HAL_ERROR;

    double lsb_mV = (ina228->_adcRange == INA228_ADC_RANGE_40_96_MV) ? 0.00125 : 0.005;
    int32_t rawVal = (int32_t)round(threshold_mV / lsb_mV);

    if (rawVal > 32767) rawVal = 32767;
    if (rawVal < -32768) rawVal = -32768;

    return INA228_SetSUVLReg(ina228, (uint16_t)((int16_t)rawVal));
}

/**
  * @brief  Read Shunt Undervoltage Threshold in millivolts (mV).
  * @param  ina228 Pointer to a INA228_HandleTypeDef structure.
  * @param  threshold_mV Pointer to store calculated threshold in mV.
  * @return HAL status
  */
HAL_StatusTypeDef INA228_GetShuntUnderVoltageThreshold_mV(INA228_HandleTypeDef *ina228, double *threshold_mV)
{
    if (ina228 == NULL || threshold_mV == NULL) return HAL_ERROR;

    uint16_t regVal = 0;
    HAL_StatusTypeDef status = INA228_GetSUVLReg(ina228, &regVal);

    if (status == HAL_OK)
    {
        int16_t rawSigned = (int16_t)regVal;
        double lsb_mV = (ina228->_adcRange == INA228_ADC_RANGE_40_96_MV) ? 0.00125 : 0.005;
        *threshold_mV = (double)rawSigned * lsb_mV;
    }

    return status;
}

/**
  * @brief  Set Bus Overvoltage Threshold in Volts (V).
  * @param  ina228 Pointer to a INA228_HandleTypeDef structure.
  * @param  threshold_V Target threshold in Volts (e.g. 15.0 V).
  *         Conversion factor is 3.125 mV/LSB (0.003125 V/LSB). Unsigned positive value only.
  * @return HAL status
  */
HAL_StatusTypeDef INA228_SetBusOverVoltageThreshold_V(INA228_HandleTypeDef *ina228, double threshold_V)
{
    if (ina228 == NULL || threshold_V < 0.0) return HAL_ERROR;

    uint32_t rawVal = (uint32_t)round(threshold_V / 0.003125);
    if (rawVal > 0x7FFF) rawVal = 0x7FFF;

    return INA228_SetBOVLReg(ina228, (uint16_t)rawVal);
}

/**
  * @brief  Read Bus Overvoltage Threshold in Volts (V).
  * @param  ina228 Pointer to a INA228_HandleTypeDef structure.
  * @param  threshold_V Pointer to store calculated threshold in Volts.
  * @return HAL status
  */
HAL_StatusTypeDef INA228_GetBusOverVoltageThreshold_V(INA228_HandleTypeDef *ina228, double *threshold_V)
{
    if (ina228 == NULL || threshold_V == NULL) return HAL_ERROR;

    uint16_t regVal = 0;
    HAL_StatusTypeDef status = INA228_GetBOVLReg(ina228, &regVal);

    if (status == HAL_OK)
    {
        uint16_t rawUnsigned = regVal & 0x7FFF;
        *threshold_V = (double)rawUnsigned * 0.003125;
    }

    return status;
}

/**
  * @brief  Set Bus Undervoltage Threshold in Volts (V).
  * @param  ina228 Pointer to a INA228_HandleTypeDef structure.
  * @param  threshold_V Target threshold in Volts (e.g. 4.5 V).
  *         Conversion factor is 3.125 mV/LSB (0.003125 V/LSB). Unsigned positive value only.
  * @return HAL status
  */
HAL_StatusTypeDef INA228_SetBusUnderVoltageThreshold_V(INA228_HandleTypeDef *ina228, double threshold_V)
{
    if (ina228 == NULL || threshold_V < 0.0) return HAL_ERROR;

    uint32_t rawVal = (uint32_t)round(threshold_V / 0.003125);
    if (rawVal > 0x7FFF) rawVal = 0x7FFF;

    return INA228_SetBUVLReg(ina228, (uint16_t)rawVal);
}

/**
  * @brief  Read Bus Undervoltage Threshold in Volts (V).
  * @param  ina228 Pointer to a INA228_HandleTypeDef structure.
  * @param  threshold_V Pointer to store calculated threshold in Volts.
  * @return HAL status
  */
HAL_StatusTypeDef INA228_GetBusUnderVoltageThreshold_V(INA228_HandleTypeDef *ina228, double *threshold_V)
{
    if (ina228 == NULL || threshold_V == NULL) return HAL_ERROR;

    uint16_t regVal = 0;
    HAL_StatusTypeDef status = INA228_GetBUVLReg(ina228, &regVal);

    if (status == HAL_OK)
    {
        uint16_t rawUnsigned = regVal & 0x7FFF;
        *threshold_V = (double)rawUnsigned * 0.003125;
    }

    return status;
}

/**
  * @brief  Set Temperature Over-Limit Threshold in Celsius degrees (°C).
  * @param  ina228 Pointer to a INA228_HandleTypeDef structure.
  * @param  threshold_C Target threshold in °C (e.g. 85.0 °C).
  *         Conversion factor is 7.8125 m°C/LSB (0.0078125 °C/LSB). Two's complement.
  * @return HAL status
  */
HAL_StatusTypeDef INA228_SetTempOverLimitThreshold_C(INA228_HandleTypeDef *ina228, double threshold_C)
{
    if (ina228 == NULL) return HAL_ERROR;

    int32_t rawVal = (int32_t)round(threshold_C / 0.0078125);
    if (rawVal > 32767) rawVal = 32767;
    if (rawVal < -32768) rawVal = -32768;

    return INA228_SetTempLimitReg(ina228, (uint16_t)((int16_t)rawVal));
}

/**
  * @brief  Read Temperature Over-Limit Threshold in Celsius degrees (°C).
  * @param  ina228 Pointer to a INA228_HandleTypeDef structure.
  * @param  threshold_C Pointer to store calculated threshold in °C.
  * @return HAL status
  */
HAL_StatusTypeDef INA228_GetTempOverLimitThreshold_C(INA228_HandleTypeDef *ina228, double *threshold_C)
{
    if (ina228 == NULL || threshold_C == NULL) return HAL_ERROR;

    uint16_t regVal = 0;
    HAL_StatusTypeDef status = INA228_GetTempLimitReg(ina228, &regVal);

    if (status == HAL_OK)
    {
        int16_t rawSigned = (int16_t)regVal;
        *threshold_C = (double)rawSigned * 0.0078125;
    }

    return status;
}

/**
  * @brief  Set Power Over-Limit Threshold in Watts (W).
  * @param  ina228 Pointer to a INA228_HandleTypeDef structure.
  * @param  threshold_W Target threshold in Watts (e.g. 10.0 W).
  *         Conversion factor is 256 * Power LSB = 256 * 3.2 * Current LSB. Unsigned value.
  * @return HAL status
  */
HAL_StatusTypeDef INA228_SetPowerOverLimitThreshold_W(INA228_HandleTypeDef *ina228, double threshold_W)
{
    if (ina228 == NULL || threshold_W < 0.0 || ina228->_currentLsb <= 0.0) return HAL_ERROR;

    double pwrLsb = 256.0 * 3.2 * ina228->_currentLsb;
    uint32_t rawVal = (uint32_t)round(threshold_W / pwrLsb);
    if (rawVal > 65535) rawVal = 65535;

    return INA228_SetPowerLimitReg(ina228, (uint16_t)rawVal);
}

/**
  * @brief  Read Power Over-Limit Threshold in Watts (W).
  * @param  ina228 Pointer to a INA228_HandleTypeDef structure.
  * @param  threshold_W Pointer to store calculated threshold in Watts.
  * @return HAL status
  */
HAL_StatusTypeDef INA228_GetPowerOverLimitThreshold_W(INA228_HandleTypeDef *ina228, double *threshold_W)
{
    if (ina228 == NULL || threshold_W == NULL || ina228->_currentLsb <= 0.0) return HAL_ERROR;

    uint16_t regVal = 0;
    HAL_StatusTypeDef status = INA228_GetPowerLimitReg(ina228, &regVal);

    if (status == HAL_OK)
    {
        double pwrLsb = 256.0 * 3.2 * ina228->_currentLsb;
        *threshold_W = (double)regVal * pwrLsb;
    }

    return status;
}
