#include "settings_mgr.h"
#include "stm32f3xx_hal.h"

/* Dirección segura en el último sector de 512KB (lejos del código de 307KB) */
#define SETTINGS_FLASH_ADDR   0x0807F800U  
#define SETTINGS_MAGIC        0x55AA1238U

UserSettings_t g_user_settings;

void Settings_Init(void) 
{
    UserSettings_t *flash_ptr = (UserSettings_t *)SETTINGS_FLASH_ADDR;
    
    if (flash_ptr->magic == SETTINGS_MAGIC) {
        g_user_settings = *flash_ptr;

        /* Seguridad: si algún valor viene en 0 o corrupto, restaurar defaults */
        if (g_user_settings.ina228_rshunt <= 0.0f || g_user_settings.ina228_rshunt > 1000.0f) 
        {
            g_user_settings.ina228_rshunt = 15.0f;
        }
        if (g_user_settings.ina228_max_current <= 0.0f || g_user_settings.ina228_max_current > 100.0f) 
        {
            g_user_settings.ina228_max_current = 6.5f;
        }
        if (g_user_settings.ina228_alert_latch > 1)  g_user_settings.ina228_alert_latch = 0;
        if (g_user_settings.ina228_alert_cnvr > 1)   g_user_settings.ina228_alert_cnvr = 0;
        if (g_user_settings.ina228_alert_pol > 1)    g_user_settings.ina228_alert_pol = 0;
        if (g_user_settings.ina228_alert_filter > 1) g_user_settings.ina228_alert_filter = 0;

        if (g_user_settings.ina228_thr_sovl < 0.0f || g_user_settings.ina228_thr_sovl > 200.0f)   g_user_settings.ina228_thr_sovl = 50.0f;
        if (g_user_settings.ina228_thr_bovl <= 0.0f || g_user_settings.ina228_thr_bovl > 85.0f)  g_user_settings.ina228_thr_bovl = 52.0f;
        if (g_user_settings.ina228_thr_buvl < 0.0f || g_user_settings.ina228_thr_buvl > 85.0f)   g_user_settings.ina228_thr_buvl = 4.4f;
        if (g_user_settings.ina228_thr_temp <= 0.0f || g_user_settings.ina228_thr_temp > 150.0f) g_user_settings.ina228_thr_temp = 85.0f;
        if (g_user_settings.ina228_thr_pwr <= 0.0f || g_user_settings.ina228_thr_pwr > 1000.0f)  g_user_settings.ina228_thr_pwr = 100.0f;
        if (g_user_settings.ina228_conv_delay > 510) g_user_settings.ina228_conv_delay = 0;
        g_user_settings.ina228_conv_delay &= ~1; /* Asegurar múltiplo de 2 ms */
        if (g_user_settings.ina228_vbus_ct > 7) g_user_settings.ina228_vbus_ct = 4;
        if (g_user_settings.ina228_vsh_ct > 7)  g_user_settings.ina228_vsh_ct = 4;
        if (g_user_settings.ina228_temp_ct > 7) g_user_settings.ina228_temp_ct = 4;
    } else {
        g_user_settings.magic = SETTINGS_MAGIC;
        g_user_settings.brightness = 100;
        g_user_settings.ovp_limit = 54.0f;
        g_user_settings.ocp_limit = 6.2f;
        g_user_settings.ina228_adc_range = 0; /* Por defecto ±163.84 mV */
        g_user_settings.ina228_mode = 0x0F;
        g_user_settings.ina228_samples = 0x03;
        g_user_settings.ina228_rshunt = 15.0f;
        g_user_settings.ina228_max_current = 6.5f;
        g_user_settings.ina228_alert_latch = 0;   /* OFF: LATCHED */
        g_user_settings.ina228_alert_cnvr = 0;    /* OFF: Disabled */
        g_user_settings.ina228_alert_pol = 0;     /* OFF: Active Low */
        g_user_settings.ina228_alert_filter = 0;  /* OFF: Non averaged */
        g_user_settings.ina228_thr_sovl = 50.0f;  /* 50.00 mV */
        g_user_settings.ina228_thr_suvl = -10.0f; /* -10.00 mV */
        g_user_settings.ina228_thr_bovl = 52.0f;  /* 52.00 V */
        g_user_settings.ina228_thr_buvl = 4.4f;   /* 4.40 V */
        g_user_settings.ina228_thr_temp = 85.0f;  /* 85.0 °C */
        g_user_settings.ina228_thr_pwr  = 100.0f; /* 100.0 W */
        g_user_settings.ina228_conv_delay = 0;    /* 0 ms */
        g_user_settings.ina228_vbus_ct = 4;       /* 540 us */
        g_user_settings.ina228_vsh_ct = 4;        /* 540 us */
        g_user_settings.ina228_temp_ct = 4;       /* 540 us */
        Settings_Save();
    }
}

void Settings_Save(void) {
    HAL_FLASH_Unlock();
    
    __HAL_FLASH_CLEAR_FLAG(FLASH_FLAG_EOP | FLASH_FLAG_PGERR | FLASH_FLAG_WRPERR);
    
    FLASH_EraseInitTypeDef erase_init;
    uint32_t page_error = 0;
    
    erase_init.TypeErase = FLASH_TYPEERASE_PAGES;
    erase_init.PageAddress = SETTINGS_FLASH_ADDR;
    erase_init.NbPages = 1;
    
    if (HAL_FLASHEx_Erase(&erase_init, &page_error) == HAL_OK) {
        uint32_t *src = (uint32_t *)&g_user_settings;
        uint32_t dest = SETTINGS_FLASH_ADDR;
        uint32_t words = (sizeof(UserSettings_t) + 3) / 4;
        
        for (uint32_t i = 0; i < words; i++) {
            HAL_FLASH_Program(FLASH_TYPEPROGRAM_WORD, dest + (i * 4), src[i]);
        }
    }
    
    HAL_FLASH_Lock();
}