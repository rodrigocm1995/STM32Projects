#include "power_sim.h"
#include "screens.h"
#include "lvgl.h"
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>

#define M_PI_F 3.14159265358979323846f

/* Macro conveniente para marcar variables que irán a CCMRAM */
#define CCMRAM_DATA __attribute__((section(".ccmram")))
/* Ejemplo 1: Mover la estructura de simulación a CCMRAM */
CCMRAM_DATA PowerSim_Data_t g_power_sim;
/* Ejemplo 2: Buffer de memoria de trabajo */
CCMRAM_DATA uint8_t mi_buffer_trabajo[2048];


PowerSim_Data_t g_power_sim;
static uint32_t total_elapsed_ms = 0;

void PowerSim_Init(void) {

     /* Limpiar toda la estructura en CCMRAM a cero absoluto */
    memset(&g_power_sim, 0, sizeof(PowerSim_Data_t));

    g_power_sim.voltage = 0.0f;
    g_power_sim.current = 0.0f;
    g_power_sim.power = 0.0f;
    g_power_sim.energy_wh = 0.0f;
    g_power_sim.capacity_mah = 0.0f;
    g_power_sim.temperature = 25.0f;
    g_power_sim.runtime_sec = 0;
    
    g_power_sim.ovp_limit = 55.0f;
    g_power_sim.ocp_limit = 7.0f;
    g_power_sim.efuse_tripped = false;
    
    total_elapsed_ms = 0;
}

/* --- FUNCIÓN 1: Simulación de Voltaje parametrizada por período y rango --- */
void PowerSim_SimulateVoltage(float period_sec, float min_v, float max_v) {
    if (period_sec <= 0.0f) return;
    
    float t_sec = total_elapsed_ms / 1000.0f;
    float mid = (min_v + max_v) / 2.0f;
    float amplitude = (max_v - min_v) / 2.0f;
    
    g_power_sim.voltage = mid + amplitude * sinf((2.0f * M_PI_F * t_sec) / period_sec);
    if (g_power_sim.voltage < 0.0f) g_power_sim.voltage = 0.0f;
}

/* --- FUNCIÓN 2: Simulación de Corriente parametrizada por período y rango --- */
void PowerSim_SimulateCurrent(float period_sec, float min_a, float max_a) {
    if (period_sec <= 0.0f) return;
    
    float t_sec = total_elapsed_ms / 1000.0f;
    float mid = (min_a + max_a) / 2.0f;
    float amplitude = (max_a - min_a) / 2.0f;
    
    g_power_sim.current = mid + amplitude * sinf((2.0f * M_PI_F * t_sec) / period_sec);
    if (g_power_sim.current < 0.0f) g_power_sim.current = 0.0f;
}

/* --- FUNCIÓN 3: Cálculo de Potencia Instantánea P = V * I --- */
void PowerSim_CalculatePower(void) {
    g_power_sim.power = g_power_sim.voltage * g_power_sim.current;
}

/* --- FUNCIÓN 4: Simulación Térmica según la carga --- */
void PowerSim_SimulateTemperature(void) {
    float target_temp = 25.0f + (g_power_sim.current * 3.5f);
    g_power_sim.temperature += (target_temp - g_power_sim.temperature) * 0.05f;
}

/* --- FUNCIÓN 5: Acumulación de Energía (Wh) y Capacidad (mAh) --- */
void PowerSim_AccumulateEnergyAndCapacity(uint32_t delta_ms) {
    float seconds = delta_ms / 1000.0f;
    g_power_sim.energy_wh += (g_power_sim.power * seconds) / 3600.0f;
    g_power_sim.capacity_mah += (g_power_sim.current * 1000.0f * seconds) / 3600.0f;
}

/* --- ORQUESTADOR: Llama a cada función modular dentro de PowerSim_Update --- */
void PowerSim_Update(uint32_t delta_ms) {
    total_elapsed_ms += delta_ms;

    if (g_power_sim.efuse_tripped) {
        g_power_sim.current = 0.0f;
        g_power_sim.power = 0.0f;
        return;
    }

    /* 1. Voltaje: Ciclo completo en 10.0 segundos, rango 0V a 52V */
    PowerSim_SimulateVoltage(60.0f, 0.0f, 52.0f);

    /* 2. Corriente: Ciclo completo en 8.0 segundos, rango 0A a 6A */
    PowerSim_SimulateCurrent(45.0f, 0.0f, 6.0f);

    /* 3. Potencia */
    PowerSim_CalculatePower();

    /* 4. Temperatura */
    PowerSim_SimulateTemperature();

    /* 5. Acumulación */
    PowerSim_AccumulateEnergyAndCapacity(delta_ms);

    /* 6. Verificación de eFuse */
    if (g_power_sim.voltage > g_power_sim.ovp_limit || g_power_sim.current > g_power_sim.ocp_limit) {
        g_power_sim.efuse_tripped = true;
    }
}

/* --- ACTUALIZADOR DE INTERFAZ GRÁFICA LVGL CON AUTO-RANGO DE UNIDADES --- */
void PowerSim_UpdateUI(void) {
    
    /* Si la pantalla principal no es la activa actualmente, salir de inmediato */
    if (lv_scr_act() != objects.main_screen) {
        return;
    }

    /* 0. ESTADO DE EFUSE (TRIPPED vs NORMAL) */
    if (objects.lbl_main_title != NULL) 
    {
        if (g_power_sim.efuse_tripped) 
        {
            lv_label_set_text(objects.lbl_main_title, "eFuse TRIPPED!");
            lv_obj_set_style_text_color(objects.lbl_main_title, lv_color_hex(0xFF5555), LV_PART_MAIN | LV_STATE_DEFAULT); // Rojo
        } 
        else 
        {
            lv_label_set_text(objects.lbl_main_title, "eFuse ON");
            lv_obj_set_style_text_color(objects.lbl_main_title, lv_color_hex(0xA2FF67), LV_PART_MAIN | LV_STATE_DEFAULT); // Verde
        }
    }

    /* 1. VOLTAJE (mV vs V) */
    if (objects.lbl_val_volt != NULL && objects.lbl_unit_volt != NULL) {
        float v_disp;
        if (g_power_sim.voltage < 1.0f) {
            v_disp = g_power_sim.voltage * 1000.0f; /* Convertir a mV */
            lv_label_set_text(objects.lbl_unit_volt, "mV");
        } else {
            v_disp = g_power_sim.voltage;            /* Mantener en V */
            lv_label_set_text(objects.lbl_unit_volt, "V");
        }
        
        int v_int = (int)v_disp;
        int v_dec = (int)(abs((int)((v_disp - v_int) * 100)));
        lv_label_set_text_fmt(objects.lbl_val_volt, "%03d.%04d", v_int, v_dec);
    }
    
    /* 2. CORRIENTE (mA vs A) */
    if (objects.lbl_val_curr != NULL && objects.lbl_unit_curr != NULL) {
        float i_disp;
        if (g_power_sim.current < 1.0f) {
            i_disp = g_power_sim.current * 1000.0f; /* Convertir a mA */
            lv_label_set_text(objects.lbl_unit_curr, "mA");
        } else {
            i_disp = g_power_sim.current;            /* Mantener en A */
            lv_label_set_text(objects.lbl_unit_curr, "A");
        }
        
        int i_int = (int)i_disp;
        int i_dec = (int)(abs((int)((i_disp - i_int) * 10000)));
        lv_label_set_text_fmt(objects.lbl_val_curr, "%03d.%04d", i_int, i_dec);
    }

    /* 3. POTENCIA (mW vs W) */
    if (objects.lbl_val_pwr != NULL && objects.lbl_unit_pwr != NULL) {
        float p_disp;
        if (g_power_sim.power < 1.0f) {
            p_disp = g_power_sim.power * 1000.0f;   /* Convertir a mW */
            lv_label_set_text(objects.lbl_unit_pwr, "mW");
        } else {
            p_disp = g_power_sim.power;              /* Mantener en W */
            lv_label_set_text(objects.lbl_unit_pwr, "W");
        }
        
        int p_int = (int)p_disp;
        int p_dec = (int)(abs((int)((p_disp - p_int) * 100)));
        lv_label_set_text_fmt(objects.lbl_val_pwr, "%03d.%04d", p_int, p_dec);
    }

    /* 4. ENERGÍA (mWh vs Wh) */
    if (objects.lbl_val_energy != NULL && objects.lbl_unit_energy != NULL) {
        float e_disp;
        if (g_power_sim.energy_wh < 1.0f) {
            e_disp = g_power_sim.energy_wh * 1000.0f; /* Convertir a mWh */
            lv_label_set_text(objects.lbl_unit_energy, "mWh");
        } else {
            e_disp = g_power_sim.energy_wh;            /* Mantener en Wh */
            lv_label_set_text(objects.lbl_unit_energy, "Wh");
        }
        
        int e_int = (int)e_disp;
        int e_dec = (int)(abs((int)((e_disp - e_int) * 100)));
        lv_label_set_text_fmt(objects.lbl_val_energy, "%03d.%04d", e_int, e_dec);
    }

    /* 5. CAPACIDAD (mAh vs Ah) */
    if (objects.lbl_val_capacity != NULL && objects.lbl_unit_capacity != NULL) {
        float c_disp;
        if (g_power_sim.capacity_mah < 1000.0f) {
            c_disp = g_power_sim.capacity_mah;         /* Mantener en mAh */
            lv_label_set_text(objects.lbl_unit_capacity, "mAh");
        } else {
            c_disp = g_power_sim.capacity_mah / 1000.0f;/* Convertir a Ah */
            lv_label_set_text(objects.lbl_unit_capacity, "Ah");
        }
        
        int c_int = (int)c_disp;
        int c_dec = (int)(abs((int)((c_disp - c_int) * 10)));
        lv_label_set_text_fmt(objects.lbl_val_capacity, "%03d.%04d", c_int, c_dec);
    }

    /* 6. RUNTIME (HH:MM:SS) */
    if (objects.lbl_val_runtime != NULL) {
        uint32_t hrs = g_power_sim.runtime_sec / 3600;
        uint32_t mins = (g_power_sim.runtime_sec % 3600) / 60;
        uint32_t secs = g_power_sim.runtime_sec % 60;
        lv_label_set_text_fmt(objects.lbl_val_runtime, "%02lu:%02lu:%02lu", hrs, mins, secs);
    }

    /* 7. TEMPERATURA */
    if (objects.lbl_temp != NULL) {
        int t_int = (int)g_power_sim.temperature;
        int t_dec = (int)(abs((int)((g_power_sim.temperature - t_int) * 10)));
        lv_label_set_text_fmt(objects.lbl_temp, "%03d.%01d°C", t_int, t_dec);
    }
}

void PowerSim_ResetStats(void) {
    g_power_sim.energy_wh = 0.0f;
    g_power_sim.capacity_mah = 0.0f;
    g_power_sim.runtime_sec = 0;
    g_power_sim.efuse_tripped = false;
    total_elapsed_ms = 0;
}