#include "settings_mgr.h"
#include "stm32f3xx_hal.h"

/* Dirección segura en el último sector de 512KB (lejos del código de 307KB) */
#define SETTINGS_FLASH_ADDR   0x0807F800U  
#define SETTINGS_MAGIC        0x55AA1234U

UserSettings_t g_user_settings;

void Settings_Init(void) {
    UserSettings_t *flash_ptr = (UserSettings_t *)SETTINGS_FLASH_ADDR;
    
    if (flash_ptr->magic == SETTINGS_MAGIC) {
        g_user_settings = *flash_ptr;
    } else {
        g_user_settings.magic = SETTINGS_MAGIC;
        g_user_settings.brightness = 100;
        g_user_settings.ovp_limit = 54.0f;
        g_user_settings.ocp_limit = 6.2f;
        g_user_settings.ina228_adc_range = 0; /* Por defecto ±163.84 mV */
        g_user_settings.ina228_mode = 0x0F;
        g_user_settings.ina228_samples = 0x03;
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