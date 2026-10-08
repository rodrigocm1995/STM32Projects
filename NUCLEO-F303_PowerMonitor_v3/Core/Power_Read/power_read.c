#include "power_read.h"
#include "power_sim.h"
#include "INA228.h"
#include "settings_mgr.h"
#include "stm32f3xx_hal_def.h"

static INA228_HandleTypeDef hina228;
static bool s_ina228_online = false;

HAL_StatusTypeDef PowerRead_Init(I2C_HandleTypeDef *hi2c) 
{
    if (hi2c == NULL) {
        s_ina228_online = false;
        return HAL_ERROR;
    }

    HAL_Delay(50); /* Dar 50ms al sensor para estabilizarse tras el encendido */

    /* Escaneo rápido de direcciones I2C del INA228 (0x40, 0x41, 0x44, 0x45) */
    uint8_t possible_addrs[] = {0x40, 0x41, 0x44, 0x45};
    uint8_t found_addr = 0;

    for (int i = 0; i < 4; i++) {
        if (HAL_I2C_IsDeviceReady(hi2c, (possible_addrs[i] << 1), 5, 50) == HAL_OK) {
            found_addr = possible_addrs[i];
            break;
        }
    }

    if (found_addr == 0) {
        s_ina228_online = false;
        return HAL_ERROR;
    }

    /* Inicializar INA228 en I2C2 con la dirección detectada */
    HAL_StatusTypeDef status = INA228_Init(&hina228, hi2c, found_addr);
    if (status == HAL_OK) {
        s_ina228_online = true;
        /* Configurar Shunt de 15 mOhm (0.015 Ohm) y Max Corriente 15.0A */
        INA228_SetShuntCalibration(&hina228, 0.015, 15.0);
        
        PowerRead_SetAdcRange(g_user_settings.ina228_adc_range);
        PowerRead_SetMode(g_user_settings.ina228_mode);
        PowerRead_SetAverage(g_user_settings.ina228_samples);

    } 
    else 
    {
        s_ina228_online = false;
    }

    return status;
}

void PowerRead_Update(uint32_t delta_ms) {
    /* Si el sensor no respondió al arrancar, usar simulación para no congelar la pantalla */
    if (!s_ina228_online) {
        PowerSim_Update(delta_ms);
        return;
    }

    double v_bus = 0, i_amp = 0, p_watt = 0, temp_c = 0, mah = 0;

    if (INA228_GetBusVoltage_V(&hina228, &v_bus) == HAL_OK) {
        g_power_sim.voltage = (float)v_bus;
    }
    if (INA228_GetCurrent_A(&hina228, &i_amp) == HAL_OK) {
        g_power_sim.current = (float)i_amp;
    }
    if (INA228_GetPower_W(&hina228, &p_watt) == HAL_OK) {
        g_power_sim.power = (float)p_watt;
    }
    if (INA228_GetDieTemp_C(&hina228, &temp_c) == HAL_OK) {
        g_power_sim.temperature = (float)temp_c;
    }
    if (INA228_GetCharge_mAh(&hina228, &mah) == HAL_OK) {
        g_power_sim.capacity_mah = (float)mah;
    }

    /* Verificación de eFuse / Protección con datos reales */
    if (g_power_sim.voltage > g_power_sim.ovp_limit || g_power_sim.current > g_power_sim.ocp_limit) {
        g_power_sim.efuse_tripped = true;
    }
}

HAL_StatusTypeDef PowerRead_GetDeviceInfo(uint16_t *manuf_id, uint16_t *device_id, uint16_t *shunt_cal)
{
#if (USE_PHYSICAL_INA228 == 1)
    uint16_t m_id = 0, d_id = 0, cal_reg = 0;
    
    // Intentar leer los registros reales del INA228 por I2C
    HAL_StatusTypeDef st1 = INA228_GetManufacturerID(&hina228, &m_id);
    HAL_StatusTypeDef st2 = INA228_GetDeviceID(&hina228, &d_id);
    HAL_StatusTypeDef st3 = INA228_GetShuntCalibrationReg(&hina228, &cal_reg);
    // Si el sensor físico respondió correctamente
    if (st1 == HAL_OK && st2 == HAL_OK) {
        s_ina228_online = true;
        if (manuf_id != NULL)  *manuf_id  = m_id;
        if (device_id != NULL) *device_id = d_id;
        if (shunt_cal != NULL) *shunt_cal = (st3 == HAL_OK) ? cal_reg : 0x0000;
        return HAL_OK;
    }
#endif
    // Si no está habilitado el sensor físico o si falló la comunicación I2C
    return HAL_ERROR;
}

HAL_StatusTypeDef PowerRead_SetAdcRange(uint8_t range)
{
    #if (USE_PHYSICAL_INA228 == 1)
    {
        if (s_ina228_online)
        {
            INA228_AdcRange_TypeDef adcEnum = (range == 1) ? INA228_ADC_RANGE_40_96_MV : INA228_ADC_RANGE_163_84_MV;
            return INA228_SetAdcRange(&hina228, adcEnum);
        }
    }
    #endif

    return HAL_OK;
}

HAL_StatusTypeDef PowerRead_GetAdcRange(uint8_t *range)
{
#if (USE_PHYSICAL_INA228 == 1)
    if (s_ina228_online && range != NULL) {
        INA228_AdcRange_TypeDef adcEnum;
        HAL_StatusTypeDef status = INA228_GetAdcRange(&hina228, &adcEnum);
        if (status == HAL_OK) {
            *range = (uint8_t)adcEnum;
            return HAL_OK;
        }
    }
#endif
    return HAL_ERROR;
}

HAL_StatusTypeDef PowerRead_SetMode(uint8_t mode)
{
    #if (USE_PHYSICAL_INA228 == 1)
        if (s_ina228_online)
        {
            return INA228_SetMode(&hina228, (INA228_Mode_TypeDef)mode);
        }
    #endif
        
        return HAL_OK;
}

HAL_StatusTypeDef PowerRead_GetMode(uint8_t *mode)
{
#if (USE_PHYSICAL_INA228 == 1)
    if (s_ina228_online && mode != NULL) {
        INA228_Mode_TypeDef m;
        HAL_StatusTypeDef status = INA228_GetMode(&hina228, &m);
        if (status == HAL_OK) {
            *mode = (uint8_t)m;
            return HAL_OK;
        }
    }
#endif
    return HAL_ERROR;
}

HAL_StatusTypeDef PowerRead_SetAverage(uint8_t samples)
{
    #if (USE_PHYSICAL_INA228 == 1)
        if (s_ina228_online)
        {
            return INA228_SetAverage(&hina228, (INA228_Average_TypeDef)samples);
        }
    #endif

        return HAL_ERROR;
}


HAL_StatusTypeDef PowerRead_GetAverage(uint8_t *samples)
{
    #if (USE_PHYSICAL_INA228 == 1)
        if (s_ina228_online && samples != NULL) 
        {
            INA228_Average_TypeDef avg;
            HAL_StatusTypeDef status = INA228_GetAverage(&hina228, &avg);
            if (status == HAL_OK) 
            {
                *samples = (uint8_t)avg;
                return HAL_OK;
            }
        }
    #endif
        return HAL_ERROR;
}
