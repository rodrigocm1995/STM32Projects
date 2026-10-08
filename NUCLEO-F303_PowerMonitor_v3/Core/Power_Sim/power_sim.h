#ifndef POWER_SIM_H
#define POWER_SIM_H

#include <stdint.h>
#include <stdbool.h>

/* ===================================================================
   CONFIGURACIÓN DE FUENTE DE DATOS
   1: Lee el sensor físico INA228 por I2C2
   0: Usa la simulación matemática de voltaje/corriente
   =================================================================== */
#define USE_HARDWARE_INA228   0

#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
    float voltage;       /* Voltios (V) */
    float current;       /* Amperios (A) */
    float power;         /* Vatios (W) */
    float energy_wh;     /* Vatios-hora (Wh) */
    float capacity_mah;  /* Miliamperios-hora (mAh) */
    float temperature;   /* Grados Celsius (°C) */
    uint32_t runtime_sec;/* Tiempo transcurrido en segundos */
    
    float ovp_limit;     /* Límite de Sobrevoltaje (V) */
    float ocp_limit;     /* Límite de Sobrecorriente (A) */
    bool efuse_tripped;  /* Estado del eFuse */
} PowerSim_Data_t;

extern PowerSim_Data_t g_power_sim;

/* Funciones principales del módulo */
void PowerSim_Init(void);
void PowerSim_Update(uint32_t delta_ms);
void PowerSim_UpdateUI(void);
void PowerSim_ResetStats(void);

/* Funciones modulares independientes de simulación */
void PowerSim_SimulateVoltage(float period_sec, float min_v, float max_v);
void PowerSim_SimulateCurrent(float period_sec, float min_a, float max_a);
void PowerSim_CalculatePower(void);
void PowerSim_SimulateTemperature(void);
void PowerSim_AccumulateEnergyAndCapacity(uint32_t delta_ms);

#ifdef __cplusplus
}
#endif

#endif /* POWER_SIM_H */