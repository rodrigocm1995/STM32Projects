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
        
        PowerRead_SetAdcRange(g_user_settings.ina228_adc_range);
        PowerRead_SetMode(g_user_settings.ina228_mode);
        PowerRead_SetAverage(g_user_settings.ina228_samples);
        PowerRead_SetConversionDelay(g_user_settings.ina228_conv_delay);
        PowerRead_SetBusConvTime(g_user_settings.ina228_vbus_ct);
        PowerRead_SetShuntConvTime(g_user_settings.ina228_vsh_ct);
        PowerRead_SetTempConvTime(g_user_settings.ina228_temp_ct);
        PowerRead_SetCalibration(g_user_settings.ina228_rshunt, g_user_settings.ina228_max_current);
        PowerRead_SetAlertLatch(g_user_settings.ina228_alert_latch);
        PowerRead_SetAlertPin(g_user_settings.ina228_alert_cnvr);
        PowerRead_SetAlertPinPolarity(g_user_settings.ina228_alert_pol);
        PowerRead_SetSlowAlert(g_user_settings.ina228_alert_filter);

        PowerRead_SetShuntOverVoltage(g_user_settings.ina228_thr_sovl);
        PowerRead_SetShuntUnderVoltage(g_user_settings.ina228_thr_suvl);
        PowerRead_SetBusOverVoltage(g_user_settings.ina228_thr_bovl);
        PowerRead_SetBusUnderVoltage(g_user_settings.ina228_thr_buvl);
        PowerRead_SetTempLimit(g_user_settings.ina228_thr_temp);
        PowerRead_SetPowerLimit(g_user_settings.ina228_thr_pwr);

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

    double v_bus = 0, i_amp = 0, p_watt = 0, temp_c = 0, mah = 0, energy_j = 0;;

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
    if (INA228_GetEnergy_J(&hina228, &energy_j) == HAL_OK) {
        g_power_sim.energy_wh = (float)(energy_j / 3600.0);
    }

    /* Verificación de eFuse / Protección con datos reales */
    if (g_power_sim.voltage > g_power_sim.ovp_limit || g_power_sim.current > g_power_sim.ocp_limit) {
        g_power_sim.efuse_tripped = true;
    } else {
        g_power_sim.efuse_tripped = false;
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

HAL_StatusTypeDef PowerRead_SetConversionDelay(uint16_t delay_ms)
{
#if (USE_PHYSICAL_INA228 == 1)
    if (s_ina228_online)
    {
        return INA228_SetConversionDelay_ms(&hina228, delay_ms);
    }
#endif
    return HAL_OK;
}

HAL_StatusTypeDef PowerRead_GetConversionDelay(uint16_t *delay_ms)
{
#if (USE_PHYSICAL_INA228 == 1)
    if (s_ina228_online && delay_ms != NULL)
    {
        INA228_ConvDelay_TypeDef raw_val;
        HAL_StatusTypeDef status = INA228_GetConversionDelay(&hina228, &raw_val);
        if (status == HAL_OK)
        {
            *delay_ms = (uint16_t)raw_val * 2;
            return HAL_OK;
        }
    }
#endif
    return HAL_ERROR;
}

HAL_StatusTypeDef PowerRead_SetBusConvTime(uint8_t conv_time)
{
#if (USE_PHYSICAL_INA228 == 1)
    if (s_ina228_online)
    {
        return INA228_SetBusConvTime(&hina228, (INA228_ConvTime_TypeDef)conv_time);
    }
#endif
    return HAL_OK;
}

HAL_StatusTypeDef PowerRead_GetBusConvTime(uint8_t *conv_time)
{
#if (USE_PHYSICAL_INA228 == 1)
    if (s_ina228_online && conv_time != NULL)
    {
        INA228_ConvTime_TypeDef ct;
        HAL_StatusTypeDef status = INA228_GetBusConvTime(&hina228, &ct);
        if (status == HAL_OK)
        {
            *conv_time = (uint8_t)ct;
            return HAL_OK;
        }
    }
#endif
    return HAL_ERROR;
}

HAL_StatusTypeDef PowerRead_SetShuntConvTime(uint8_t conv_time)
{
#if (USE_PHYSICAL_INA228 == 1)
    if (s_ina228_online)
    {
        return INA228_SetShuntConvTime(&hina228, (INA228_ConvTime_TypeDef)conv_time);
    }
#endif
    return HAL_OK;
}

HAL_StatusTypeDef PowerRead_GetShuntConvTime(uint8_t *conv_time)
{
#if (USE_PHYSICAL_INA228 == 1)
    if (s_ina228_online && conv_time != NULL)
    {
        INA228_ConvTime_TypeDef ct;
        HAL_StatusTypeDef status = INA228_GetShuntConvTime(&hina228, &ct);
        if (status == HAL_OK)
        {
            *conv_time = (uint8_t)ct;
            return HAL_OK;
        }
    }
#endif
    return HAL_ERROR;
}

HAL_StatusTypeDef PowerRead_SetTempConvTime(uint8_t conv_time)
{
#if (USE_PHYSICAL_INA228 == 1)
    if (s_ina228_online)
    {
        return INA228_SetTempConvTime(&hina228, (INA228_ConvTime_TypeDef)conv_time);
    }
#endif
    return HAL_OK;
}

HAL_StatusTypeDef PowerRead_GetTempConvTime(uint8_t *conv_time)
{
#if (USE_PHYSICAL_INA228 == 1)
    if (s_ina228_online && conv_time != NULL)
    {
        INA228_ConvTime_TypeDef ct;
        HAL_StatusTypeDef status = INA228_GetTempConvTime(&hina228, &ct);
        if (status == HAL_OK)
        {
            *conv_time = (uint8_t)ct;
            return HAL_OK;
        }
    }
#endif
    return HAL_ERROR;
}

HAL_StatusTypeDef PowerRead_SetCalibration(float rshunt_mohms, float max_current_a)
{
#if (USE_PHYSICAL_INA228 == 1)
    if (s_ina228_online && rshunt_mohms > 0.0f) {
        if (max_current_a <= 0.0f) {
            max_current_a = 6.5f; /* Valor seguro por defecto */
        }
        double r_ohms = (double)(rshunt_mohms / 1000.0f);
        return INA228_SetShuntCalibration(&hina228, r_ohms, (double)max_current_a);
    }
#endif
    return HAL_ERROR;
}

HAL_StatusTypeDef PowerRead_SetAlertLatch(uint8_t latch_state)
{
#if (USE_PHYSICAL_INA228 == 1)
    if (s_ina228_online) {
        INA228_AlertLatch_TypeDef latch = (latch_state == 0) ? INA228_ALERT_LATCH_ENABLED : INA228_ALERT_LATCH_TRANSPARENT;
        return INA228_SetAlertLatch(&hina228, latch);
    }
#endif
    return HAL_OK;
}

HAL_StatusTypeDef PowerRead_GetAlertLatch(uint8_t *latch_state)
{
#if (USE_PHYSICAL_INA228 == 1)
    if (s_ina228_online && latch_state != NULL) {
        INA228_AlertLatch_TypeDef latch;
        HAL_StatusTypeDef status = INA228_GetAlertLatch(&hina228, &latch);
        if (status == HAL_OK) {
            *latch_state = (latch == INA228_ALERT_LATCH_TRANSPARENT) ? 1 : 0;
            return HAL_OK;
        }
    }
#endif
    return HAL_ERROR;
}

HAL_StatusTypeDef PowerRead_SetAlertPin(uint8_t cnvr_state)
{
#if (USE_PHYSICAL_INA228 == 1)
    if (s_ina228_online) {
        INA228_CNVR_TypeDef cnvr = (cnvr_state == 1) ? INA228_ENABLE_CNVR_FLAG_ON_ALERT_PIN : INA228_DISABLE_CNVR_FLAG_ON_ALERT_PIN;
        return INA228_SetAlertPin(&hina228, cnvr);
    }
#endif
    return HAL_OK;
}

HAL_StatusTypeDef PowerRead_GetAlertPin(uint8_t *cnvr_state)
{
#if (USE_PHYSICAL_INA228 == 1)
    if (s_ina228_online && cnvr_state != NULL) {
        INA228_CNVR_TypeDef cnvr;
        HAL_StatusTypeDef status = INA228_GetAlertPin(&hina228, &cnvr);
        if (status == HAL_OK) {
            *cnvr_state = (cnvr == INA228_ENABLE_CNVR_FLAG_ON_ALERT_PIN) ? 1 : 0;
            return HAL_OK;
        }
    }
#endif
    return HAL_ERROR;
}

HAL_StatusTypeDef PowerRead_SetAlertPinPolarity(uint8_t pol_state)
{
#if (USE_PHYSICAL_INA228 == 1)
    if (s_ina228_online) {
        INA228_AlertPinPol_TypeDef pol = (pol_state == 1) ? INA228_ALERT_ACTIVE_HIGH : INA228_ALERT_ACTIVE_LOW;
        return INA228_SetAlertPinPolarity(&hina228, pol);
    }
#endif
    return HAL_OK;
}

HAL_StatusTypeDef PowerRead_GetAlertPinPolarity(uint8_t *pol_state)
{
#if (USE_PHYSICAL_INA228 == 1)
    if (s_ina228_online && pol_state != NULL) {
        INA228_AlertPinPol_TypeDef pol;
        HAL_StatusTypeDef status = INA228_GetAlertPinPolarity(&hina228, &pol);
        if (status == HAL_OK) {
            *pol_state = (pol == INA228_ALERT_ACTIVE_HIGH) ? 1 : 0;
            return HAL_OK;
        }
    }
#endif
    return HAL_ERROR;
}

HAL_StatusTypeDef PowerRead_SetSlowAlert(uint8_t filter_state)
{
#if (USE_PHYSICAL_INA228 == 1)
    if (s_ina228_online) {
        INA228_SlowAlert_TypeDef slow = (filter_state == 1) ? INA228_ALERT_COMPARISON_AVERAGED : INA228_ALERT_COMPARISON_NON_AVERAGED;
        return INA228_SetSlowAlert(&hina228, slow);
    }
#endif
    return HAL_OK;
}

HAL_StatusTypeDef PowerRead_GetSlowAlert(uint8_t *filter_state)
{
#if (USE_PHYSICAL_INA228 == 1)
    if (s_ina228_online && filter_state != NULL) {
        INA228_SlowAlert_TypeDef slow;
        HAL_StatusTypeDef status = INA228_GetSlowAlert(&hina228, &slow);
        if (status == HAL_OK) {
            *filter_state = (slow == INA228_ALERT_COMPARISON_AVERAGED) ? 1 : 0;
            return HAL_OK;
        }
    }
#endif
    return HAL_ERROR;
}

HAL_StatusTypeDef PowerRead_GetDiagAlert(uint16_t *diag_alrt)
{
#if (USE_PHYSICAL_INA228 == 1)
    if (s_ina228_online && diag_alrt != NULL) {
        return INA228_GetDiagAlert(&hina228, diag_alrt);
    }
#endif
    return HAL_ERROR;
}

HAL_StatusTypeDef PowerRead_SetShuntOverVoltage(float threshold_mV)
{
#if (USE_PHYSICAL_INA228 == 1)
    if (s_ina228_online) {
        return INA228_SetShuntOverVoltageThreshold_mV(&hina228, (double)threshold_mV);
    }
#endif
    return HAL_OK;
}

HAL_StatusTypeDef PowerRead_GetShuntOverVoltage(float *threshold_mV)
{
#if (USE_PHYSICAL_INA228 == 1)
    if (s_ina228_online && threshold_mV != NULL) {
        double val = 0.0;
        HAL_StatusTypeDef status = INA228_GetShuntOverVoltageThreshold_mV(&hina228, &val);
        if (status == HAL_OK) {
            *threshold_mV = (float)val;
            return HAL_OK;
        }
    }
#endif
    return HAL_ERROR;
}

HAL_StatusTypeDef PowerRead_SetShuntUnderVoltage(float threshold_mV)
{
#if (USE_PHYSICAL_INA228 == 1)
    if (s_ina228_online) {
        return INA228_SetShuntUnderVoltageThreshold_mV(&hina228, (double)threshold_mV);
    }
#endif
    return HAL_OK;
}

HAL_StatusTypeDef PowerRead_GetShuntUnderVoltage(float *threshold_mV)
{
#if (USE_PHYSICAL_INA228 == 1)
    if (s_ina228_online && threshold_mV != NULL) {
        double val = 0.0;
        HAL_StatusTypeDef status = INA228_GetShuntUnderVoltageThreshold_mV(&hina228, &val);
        if (status == HAL_OK) {
            *threshold_mV = (float)val;
            return HAL_OK;
        }
    }
#endif
    return HAL_ERROR;
}

HAL_StatusTypeDef PowerRead_SetBusOverVoltage(float threshold_V)
{
#if (USE_PHYSICAL_INA228 == 1)
    if (s_ina228_online) {
        return INA228_SetBusOverVoltageThreshold_V(&hina228, (double)threshold_V);
    }
#endif
    return HAL_OK;
}

HAL_StatusTypeDef PowerRead_GetBusOverVoltage(float *threshold_V)
{
#if (USE_PHYSICAL_INA228 == 1)
    if (s_ina228_online && threshold_V != NULL) {
        double val = 0.0;
        HAL_StatusTypeDef status = INA228_GetBusOverVoltageThreshold_V(&hina228, &val);
        if (status == HAL_OK) {
            *threshold_V = (float)val;
            return HAL_OK;
        }
    }
#endif
    return HAL_ERROR;
}

HAL_StatusTypeDef PowerRead_SetBusUnderVoltage(float threshold_V)
{
#if (USE_PHYSICAL_INA228 == 1)
    if (s_ina228_online) {
        return INA228_SetBusUnderVoltageThreshold_V(&hina228, (double)threshold_V);
    }
#endif
    return HAL_OK;
}

HAL_StatusTypeDef PowerRead_GetBusUnderVoltage(float *threshold_V)
{
#if (USE_PHYSICAL_INA228 == 1)
    if (s_ina228_online && threshold_V != NULL) {
        double val = 0.0;
        HAL_StatusTypeDef status = INA228_GetBusUnderVoltageThreshold_V(&hina228, &val);
        if (status == HAL_OK) {
            *threshold_V = (float)val;
            return HAL_OK;
        }
    }
#endif
    return HAL_ERROR;
}

HAL_StatusTypeDef PowerRead_SetTempLimit(float threshold_C)
{
#if (USE_PHYSICAL_INA228 == 1)
    if (s_ina228_online) {
        return INA228_SetTempOverLimitThreshold_C(&hina228, (double)threshold_C);
    }
#endif
    return HAL_OK;
}

HAL_StatusTypeDef PowerRead_GetTempLimit(float *threshold_C)
{
#if (USE_PHYSICAL_INA228 == 1)
    if (s_ina228_online && threshold_C != NULL) {
        double val = 0.0;
        HAL_StatusTypeDef status = INA228_GetTempOverLimitThreshold_C(&hina228, &val);
        if (status == HAL_OK) {
            *threshold_C = (float)val;
            return HAL_OK;
        }
    }
#endif
    return HAL_ERROR;
}

HAL_StatusTypeDef PowerRead_SetPowerLimit(float threshold_W)
{
#if (USE_PHYSICAL_INA228 == 1)
    if (s_ina228_online) {
        return INA228_SetPowerOverLimitThreshold_W(&hina228, (double)threshold_W);
    }
#endif
    return HAL_OK;
}

HAL_StatusTypeDef PowerRead_GetPowerLimit(float *threshold_W)
{
#if (USE_PHYSICAL_INA228 == 1)
    if (s_ina228_online && threshold_W != NULL) {
        double val = 0.0;
        HAL_StatusTypeDef status = INA228_GetPowerOverLimitThreshold_W(&hina228, &val);
        if (status == HAL_OK) {
            *threshold_W = (float)val;
            return HAL_OK;
        }
    }
#endif
    return HAL_ERROR;
}

HAL_StatusTypeDef PowerRead_ResetAccumulators(void)
{
#if (USE_PHYSICAL_INA228 == 1)
    if (s_ina228_online) {
        return INA228_ResetEnergyAndCharge(&hina228);
    }
#endif
    return HAL_OK;
}