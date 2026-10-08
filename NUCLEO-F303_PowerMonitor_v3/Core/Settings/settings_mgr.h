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
} UserSettings_t;

extern UserSettings_t g_user_settings;

/* Funciones del gestor de configuración */
void Settings_Init(void);
void Settings_Save(void);

#ifdef __cplusplus
}
#endif

#endif /* SETTINGS_MGR_H */