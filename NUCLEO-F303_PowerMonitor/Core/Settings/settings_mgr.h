#ifndef SETTINGS_MGR_H
#define SETTINGS_MGR_H

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/* Estructura de ajustes guardados en Flash */
typedef struct {
    uint32_t magic;           /* Clave de validación */
    uint8_t  brightness;      /* Brillo guardado */
    float    ovp_limit;       /* Límite OVP */
    float    ocp_limit;       /* Límite OCP */
    uint8_t  cc_enabled;      /* Estado CC: 0 = OFF, 1 = ON */
    uint8_t ina228_adc_range;  /* 0: +- 163.84 mV (defecto), 1: +-40.96 mV */
    uint8_t ina228_mode;         
    uint8_t ina228_samples;
    float    ina228_rshunt;
    float    ina228_max_current;
    uint8_t  ina228_alert_latch;   /* 0: LATCHED (switch OFF), 1: TRANSPARENT (switch ON) */
    uint8_t  ina228_alert_cnvr;    /* 0: OFF (switch OFF), 1: ON (switch ON) */
    uint8_t  ina228_alert_pol;     /* 0: ACTIVE LOW (switch OFF), 1: ACTIVE HIGH (switch ON) */
    uint8_t  ina228_alert_filter;  /* 0: NON AVERAGED (switch OFF), 1: AVERAGED (switch ON) */
    float    ina228_thr_sovl;      /* Shunt Overvoltage (mV) */
    float    ina228_thr_suvl;      /* Shunt Undervoltage (mV) */
    float    ina228_thr_bovl;      /* Bus Overvoltage (V) */
    float    ina228_thr_buvl;      /* Bus Undervoltage (V) */
    float    ina228_thr_temp;      /* Over Temperature (°C) */
    float    ina228_thr_pwr;       /* Over Power (W) */
    uint16_t ina228_conv_delay;    /* Conversion Delay (ms): 0..510 ms */
    uint8_t  ina228_vbus_ct;       /* VBUS Conv Time: 0..7 */
    uint8_t  ina228_vsh_ct;        /* VSHUNT Conv Time: 0..7 */
    uint8_t  ina228_temp_ct;       /* TEMP Conv Time: 0..7 */
    uint8_t  ina228_temp_comp;     /* Shunt Temp Compensation: 0: OFF, 1: ON */
} UserSettings_t;

extern UserSettings_t g_user_settings;

/* Funciones del gestor de configuración */
void Settings_Init(void);
void Settings_Save(void);

#ifdef __cplusplus
}
#endif

#endif /* SETTINGS_MGR_H */