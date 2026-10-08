#include "ui_events.h"
#include "screens.h"
#include "ui.h"
#include "lvgl.h"
#include "power_sim.h"
#include "power_read.h"
#include "Display_Driver.h"
#include "settings_mgr.h"
#include <stdlib.h>
#include "console_uart.h"

typedef enum 
{
    MODE_BRIGHTNESS,
    MODE_CURRENT_LIMIT,
    MODE_OVP_LIMIT,
    MODE_SHUNT_RESISTOR,
    MODE_MAX_CURRENT,
    MODE_THR_SOVL,
    MODE_THR_SUVL,
    MODE_THR_BOVL,
    MODE_THR_BUVL,
    MODE_THR_TEMP,
    MODE_THR_PWR,
    MODE_INA_CONVDLY
} LimitMode_t;

/* Estructura para rotar entre los modos disponibles */
typedef struct 
{
    uint8_t mode_val;
    const char *text;
} AdcModeOption_t;

/* Estructura para rotas entre las muestras (samples) disponibles */
typedef struct 
{
    uint8_t sample_val;
    const char *text;
} SampleOption_t;

static const AdcModeOption_t s_adc_modes[] = {
    { 0x0F, "CONT: T + S + B" },  /* Continuo Tensión + Corriente + Temperatura (Defecto) */
    { 0x0E, "CONT: T + S"     },
    { 0x0D, "CONT: T + B"     },  
    { 0x0C, "CONT: T"         },  
    { 0x0B, "CONT: S + B"     },  
    { 0x0A, "CONT: S"         },  
    { 0x09, "CONT: B"         },
    { 0x07, "OS: T + S + B"   },
    { 0x06, "OS: T + S"       },
    { 0x05, "OS: T + B"       },
    { 0x04, "OS: T"          },
    { 0x03, "OS: S + B"      },
    { 0x02, "OS: S"          },
    { 0x01, "OS: B"          },
    { 0x00, "SHUTDOWN"       }
};

static const SampleOption_t s_adc_samples[] = {
    {0x00, "1 Sample"},
    {0x01, "4 Samples"},
    {0x02, "16 Samples"},
    {0x03, "64 samples"},
    {0x04, "128 samples"},
    {0x05, "256 samples"},
    {0x06, "512 samples"},
    {0x07, "1024 samples"}
};

static const char * const s_conv_time_labels[] = {
    "50 us",
    "84 us",
    "150 us",
    "280 us",
    "540 us",
    "1052 us",
    "2074 us",
    "4120 us"
};

#define ADC_MODE_COUNT (sizeof(s_adc_modes) / sizeof(s_adc_modes[0]))
#define ADC_SAMPLES_COUNT (sizeof(s_adc_samples) / sizeof(s_adc_samples[0]))
#define CONV_TIME_COUNT (sizeof(s_conv_time_labels) / sizeof(s_conv_time_labels[0]))

static const char* get_conv_time_text(uint8_t ct)
{
    if (ct < CONV_TIME_COUNT) {
        return s_conv_time_labels[ct];
    }
    return "540 us";
}

static LimitMode_t g_active_limit_mode = MODE_BRIGHTNESS;
static float g_temp_current_limit = 5.0f;
static float g_temp_ovp_limit     = 50.0f;
/* Variables de estado temporales para la pantalla ADC */
static uint8_t  g_temp_adc_range     = 0; /* 0: +- 163.84 mV, 1: +-40.96 mV */
static uint8_t  g_temp_adc_mode      = 0x0F;
static uint8_t  g_temp_adc_sample    = 0x03; /* 64 samples */
static uint16_t g_temp_adc_convdelay = 0;    /* 0..510 ms */
static uint8_t  g_temp_adc_vbus_ct   = 4;    /* 540 us */
static uint8_t  g_temp_adc_vshunt_ct = 4;    /* 540 us */
static uint8_t  g_temp_adc_temp_ct   = 4;    /* 540 us */
static bool     s_adc_settings_loaded = false;

/* Variables de estado de Brillo */
static uint8_t g_saved_brightness = 100;
static uint8_t g_temp_brightness  = 100;
static float g_temp_shunt_res = 15.0f;
static float g_temp_max_current = 6.5f;

/* Variables de estado temporales para Pantalla 3 (Alertas INA228) */
static uint8_t g_temp_alert_latch  = 0;
static uint8_t g_temp_alert_cnvr   = 0;
static uint8_t g_temp_alert_pol    = 0;
static uint8_t g_temp_alert_filter = 0;
static bool    s_alert_settings_loaded = false;

/* Variables de estado temporales para Pantalla 4 (Límites INA228) */
static float   g_temp_thr_sovl = 50.0f;
static float   g_temp_thr_suvl = -10.0f;
static float   g_temp_thr_bovl = 52.0f;
static float   g_temp_thr_buvl = 4.4f;
static float   g_temp_thr_temp = 85.0f;
static float   g_temp_thr_pwr  = 100.0f;
static bool    s_thr_settings_loaded = false;

/* Variables de estado para Pantalla Osciloscopio (graph_screen) */
static lv_chart_series_t *s_ser_volt = NULL;
static lv_chart_series_t *s_ser_curr = NULL;
static bool               s_graph_paused = false;

static const char* get_mode_text(uint8_t mode_val)
{
    for (int i = 0; i < ADC_MODE_COUNT; i++) 
    {
        if (s_adc_modes[i].mode_val == mode_val) 
        {
            return s_adc_modes[i].text;
        }
    }
    return "CONT: T + S + B";
}

static const char* get_sample_text(uint8_t sample_val)
{
    for (int i = 0; i < ADC_SAMPLES_COUNT; i++)
    {
        if (s_adc_samples[i].sample_val == sample_val)
        {
            return s_adc_samples[i].text;
        }
    }
    return "64 samples";
}

/* Índice de opción seleccionada en menu_screen (0: efuse, 1: cc, 2: ovp, 3: backlight, 4: ina) */
static int g_selected_menu_index = 0;

/* Declaraciones anticipadas de callbacks y setup */
static void setup_menu_screen(void);
static void setup_limit_screen(void);
static void setup_reset_modal(void);
static void setup_ina228_adc_screen(void);
static void setup_ina228_cal_screen(void);
static void setup_ina228_alert_screen(void);
static void setup_ina228_limits_screen(void);
static void setup_graph_screen(void);

static void on_btn_graph_back_clicked(lv_event_t *e);
static void on_btn_graph_pause_clicked(lv_event_t *e);
static void on_btn_graph_clear_clicked(lv_event_t *e);

static void on_btn_menu_select_clicked(lv_event_t *e);
static void on_btn_menu_enter_clicked(lv_event_t *e);
static void on_btn_menu_back_clicked(lv_event_t *e);
static void on_btn_menu_edit1_clicked(lv_event_t *e);
static void on_btn_menu_edit2_changed(lv_event_t *e);
static void on_btn_menu_edit3_clicked(lv_event_t *e);
static void on_btn_menu_edit4_clicked(lv_event_t *e);
static void on_btn_menu_edit5_clicked(lv_event_t *e);
static void on_box_clicked(lv_event_t *e);

static void on_limit_slider_changed(lv_event_t *e);
static void on_btn_limit_dec_clicked(lv_event_t *e);
static void on_btn_limit_inc_clicked(lv_event_t *e);
static void on_btn_limit_preset1_clicked(lv_event_t *e);
static void on_btn_limit_preset2_clicked(lv_event_t *e);
static void on_btn_limit_preset3_clicked(lv_event_t *e);
static void on_btn_limit_cancel_clicked(lv_event_t *e);
static void on_btn_limit_save_clicked(lv_event_t *e);

static void on_btn_modal_abort_clicked(lv_event_t *e);
static void on_btn_modal_reset_clicked(lv_event_t *e);

static void on_btn_adc_cancel_clicked(lv_event_t *e);
static void on_btn_adc_save_clicked(lv_event_t *e);
static void on_btn_adc_next_clicked(lv_event_t *e);
static void on_btn_cal_prev_clicked(lv_event_t *e);
static void on_btn_cal_save_clicked(lv_event_t *e);
static void on_btn_cal_next_clicked(lv_event_t *e);
static void on_btn_alert_prev_clicked(lv_event_t *e);
static void on_btn_alert_save_clicked(lv_event_t *e);
static void on_btn_alert_next_clicked(lv_event_t *e);
static void on_switch_alert1_changed(lv_event_t *e);
static void on_switch_alert2_changed(lv_event_t *e);
static void on_switch_alert3_changed(lv_event_t *e);
static void on_switch_alert4_changed(lv_event_t *e);
static void on_btn_thr_prev_clicked(lv_event_t *e);
static void on_btn_thr_save_clicked(lv_event_t *e);

static void on_btn_ina228_adc_edit1_clicked(lv_event_t *e);
static void on_btn_ina228_adc_edit2_clicked(lv_event_t *e);
static void on_btn_ina228_adc_edit3_clicked(lv_event_t *e);
static void on_btn_ina228_adc_edit4_clicked(lv_event_t *e);
static void on_btn_ina228_adc_edit5_clicked(lv_event_t *e);
static void on_btn_ina228_adc_edit6_clicked(lv_event_t *e);
static void on_btn_ina228_adc_edit7_clicked(lv_event_t *e);
static void setup_convdelay_limit_screen(void);

static void on_btn_ina228_cal_edit1_clicked(lv_event_t *e);
static void on_btn_ina228_cal_edit2_clicked(lv_event_t *e);
static void update_shunt_display(float val);

static void setup_shunt_limit_screen(void);
static void setup_max_current_limit_screen(void);

static void on_btn_ina228_thr_edit1_clicked(lv_event_t *e);
static void on_btn_ina228_thr_edit2_clicked(lv_event_t *e);
static void on_btn_ina228_thr_edit3_clicked(lv_event_t *e);
static void on_btn_ina228_thr_edit4_clicked(lv_event_t *e);
static void on_btn_ina228_thr_edit5_clicked(lv_event_t *e);
static void on_btn_ina228_thr_edit6_clicked(lv_event_t *e);

static void setup_thr_sovl_screen(void);
static void setup_thr_suvl_screen(void);
static void setup_thr_bovl_screen(void);
static void setup_thr_buvl_screen(void);
static void setup_thr_temp_screen(void);
static void setup_thr_pwr_screen(void);

/* Resalta únicamente el contenedor seleccionado (#383838) y restaura los demás (#272727) */
static void update_menu_selection_highlight(void)
{
    lv_obj_t *boxes[5] = {
        objects.box_efuse,
        objects.box_cc,
        objects.box_ovp,
        objects.box_dispaly_bl,
        objects.box_ina_config
    };
    for (int i = 0; i < 5; i++) {
        if (boxes[i] != NULL) {
            lv_color_t color = (i == g_selected_menu_index) ? lv_color_hex(0x383838) : lv_color_hex(0x272727);
            
            lv_obj_set_style_bg_color(boxes[i], color, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_bg_color(boxes[i], color, LV_PART_MAIN | LV_STATE_FOCUSED);
            lv_obj_set_style_bg_color(boxes[i], color, LV_PART_MAIN | LV_STATE_HOVERED);
            lv_obj_set_style_bg_color(boxes[i], color, LV_PART_MAIN | LV_STATE_PRESSED);
            
            lv_obj_remove_state(boxes[i], LV_STATE_FOCUSED | LV_STATE_HOVERED | LV_STATE_PRESSED);
        }
    }
}

/* Actualizar etiquetas en la pantalla del menú */
static void update_menu_screen_ui(void)
{
    if (objects.lbl_menu_bl != NULL) {
        lv_label_set_text_fmt(objects.lbl_menu_bl, "%d %%", g_saved_brightness);
    }
    
    if (objects.lbl_val_efuse_limit != NULL) {
        int i_int = (int)g_user_settings.ocp_limit;
        int i_dec = (int)(abs((int)((g_user_settings.ocp_limit - i_int) * 100)));
        lv_label_set_text_fmt(objects.lbl_val_efuse_limit, "%d.%02d A", i_int, i_dec);
    }

    if (objects.lbl_val_ovp != NULL) {
        int v_int = (int)g_user_settings.ovp_limit;
        int v_dec = (int)(abs((int)((g_user_settings.ovp_limit - v_int) * 100)));
        lv_label_set_text_fmt(objects.lbl_val_ovp, "%d.%02d V", v_int, v_dec);
    }

    if (objects.btn_menu_edit2 != NULL && objects.lbl_cc_state != NULL) {
        if (g_user_settings.cc_enabled) {
            lv_obj_add_state(objects.btn_menu_edit2, LV_STATE_CHECKED);
            lv_label_set_text(objects.lbl_cc_state, "ON");
        } else {
            lv_obj_remove_state(objects.btn_menu_edit2, LV_STATE_CHECKED);
            lv_label_set_text(objects.lbl_cc_state, "OFF");
        }
    }

    update_menu_selection_highlight();
}

/* Creación Bajo Demanda de menu_screen */
static void setup_menu_screen(void)
{
    if (objects.menu_screen == NULL) {
        create_screen_menu_screen();

        if (objects.btn_menu_select != NULL) lv_obj_add_event_cb(objects.btn_menu_select, on_btn_menu_select_clicked, LV_EVENT_CLICKED, NULL);
        if (objects.btn_menu_enter != NULL)  lv_obj_add_event_cb(objects.btn_menu_enter, on_btn_menu_enter_clicked, LV_EVENT_CLICKED, NULL);
        if (objects.btn_menu_back != NULL)   lv_obj_add_event_cb(objects.btn_menu_back, on_btn_menu_back_clicked, LV_EVENT_CLICKED, NULL);
        if (objects.btn_menu_edit1 != NULL)  lv_obj_add_event_cb(objects.btn_menu_edit1, on_btn_menu_edit1_clicked, LV_EVENT_CLICKED, NULL);
        if (objects.btn_menu_edit2 != NULL)  lv_obj_add_event_cb(objects.btn_menu_edit2, on_btn_menu_edit2_changed, LV_EVENT_VALUE_CHANGED, NULL);
        if (objects.btn_menu_edit3 != NULL)  lv_obj_add_event_cb(objects.btn_menu_edit3, on_btn_menu_edit3_clicked, LV_EVENT_CLICKED, NULL);
        if (objects.btn_menu_edit4 != NULL)  lv_obj_add_event_cb(objects.btn_menu_edit4, on_btn_menu_edit4_clicked, LV_EVENT_CLICKED, NULL);
        if (objects.btn_menu_edit5 != NULL)  lv_obj_add_event_cb(objects.btn_menu_edit5, on_btn_menu_edit5_clicked, LV_EVENT_CLICKED, NULL);

        if (objects.box_efuse != NULL)       lv_obj_add_event_cb(objects.box_efuse, on_box_clicked, LV_EVENT_CLICKED, NULL);
        if (objects.box_cc != NULL)          lv_obj_add_event_cb(objects.box_cc, on_box_clicked, LV_EVENT_CLICKED, NULL);
        if (objects.box_ovp != NULL)         lv_obj_add_event_cb(objects.box_ovp, on_box_clicked, LV_EVENT_CLICKED, NULL);
        if (objects.box_dispaly_bl != NULL)  lv_obj_add_event_cb(objects.box_dispaly_bl, on_box_clicked, LV_EVENT_CLICKED, NULL);
        if (objects.box_ina_config != NULL)  lv_obj_add_event_cb(objects.box_ina_config, on_box_clicked, LV_EVENT_CLICKED, NULL);
    }
}

/* Creación Bajo Demanda de limit_screen */
static void setup_limit_screen(void)
{
    if (objects.limit_screen == NULL) {
        create_screen_limit_screen();

        if (objects.btn_limit_cancel != NULL)  lv_obj_add_event_cb(objects.btn_limit_cancel, on_btn_limit_cancel_clicked, LV_EVENT_CLICKED, NULL);
        if (objects.btn_limit_save != NULL)    lv_obj_add_event_cb(objects.btn_limit_save, on_btn_limit_save_clicked, LV_EVENT_CLICKED, NULL);
        if (objects.limit_slider != NULL)      lv_obj_add_event_cb(objects.limit_slider, on_limit_slider_changed, LV_EVENT_VALUE_CHANGED, NULL);
        if (objects.btn_limit_dec != NULL)     lv_obj_add_event_cb(objects.btn_limit_dec, on_btn_limit_dec_clicked, LV_EVENT_CLICKED, NULL);
        if (objects.btn_limit_inc != NULL)     lv_obj_add_event_cb(objects.btn_limit_inc, on_btn_limit_inc_clicked, LV_EVENT_CLICKED, NULL);
        if (objects.btn_limit_preset1 != NULL) lv_obj_add_event_cb(objects.btn_limit_preset1, on_btn_limit_preset1_clicked, LV_EVENT_CLICKED, NULL);
        if (objects.btn_limit_preset2 != NULL) lv_obj_add_event_cb(objects.btn_limit_preset2, on_btn_limit_preset2_clicked, LV_EVENT_CLICKED, NULL);
        if (objects.btn_limit_preset3 != NULL) lv_obj_add_event_cb(objects.btn_limit_preset3, on_btn_limit_preset3_clicked, LV_EVENT_CLICKED, NULL);
    }
}


/* Creación Bajo Demanda de reset_modal */
static void setup_reset_modal(void)
{
    if (objects.reset_modal == NULL) {
        create_screen_reset_modal();

        if (objects.btn_modal_abort != NULL) lv_obj_add_event_cb(objects.btn_modal_abort, on_btn_modal_abort_clicked, LV_EVENT_CLICKED, NULL);
        if (objects.btn_modal_reset != NULL) lv_obj_add_event_cb(objects.btn_modal_reset, on_btn_modal_reset_clicked, LV_EVENT_CLICKED, NULL);
    }
}

/* Prepara la pantalla 1 del INA228 (ADC) */
static void setup_ina228_adc_screen(void)
{
    if (objects.ina228_adc_screen == NULL) 
    {
        create_screen_ina228_adc_screen();

        if (objects.btn_adc_cancel != NULL)
        {
            lv_obj_add_event_cb(objects.btn_adc_cancel, on_btn_adc_cancel_clicked, LV_EVENT_CLICKED, NULL);
        }         
        if (objects.btn_adc_save != NULL)
        {
            lv_obj_add_event_cb(objects.btn_adc_save, on_btn_adc_save_clicked, LV_EVENT_CLICKED, NULL);
        }           
        if (objects.btn_adc_next != NULL)
        {
            lv_obj_add_event_cb(objects.btn_adc_next, on_btn_adc_next_clicked, LV_EVENT_CLICKED, NULL);
        }           
        if (objects.btn_ina228_adc_edit1 != NULL)
        {
            lv_obj_add_event_cb(objects.btn_ina228_adc_edit1, on_btn_ina228_adc_edit1_clicked, LV_EVENT_CLICKED, NULL);
        }
        
        if (objects.btn_ina228_adc_edit2 != NULL)
        {
            lv_obj_add_event_cb(objects.btn_ina228_adc_edit2, on_btn_ina228_adc_edit2_clicked, LV_EVENT_CLICKED, NULL); 
        }

        if (objects.btn_ina228_adc_edit3 != NULL)
        {
            lv_obj_add_event_cb(objects.btn_ina228_adc_edit3, on_btn_ina228_adc_edit3_clicked, LV_EVENT_CLICKED, NULL); 
        }

        if (objects.btn_ina228_adc_edit4 != NULL)
        {
            lv_obj_add_event_cb(objects.btn_ina228_adc_edit4, on_btn_ina228_adc_edit4_clicked, LV_EVENT_CLICKED, NULL);
        }

        if (objects.btn_ina228_adc_edit5 != NULL)
        {
            lv_obj_add_event_cb(objects.btn_ina228_adc_edit5, on_btn_ina228_adc_edit5_clicked, LV_EVENT_CLICKED, NULL);
        }

        if (objects.btn_ina228_adc_edit6 != NULL)
        {
            lv_obj_add_event_cb(objects.btn_ina228_adc_edit6, on_btn_ina228_adc_edit6_clicked, LV_EVENT_CLICKED, NULL);
        }

        if (objects.btn_ina228_adc_edit7 != NULL)
        {
            lv_obj_add_event_cb(objects.btn_ina228_adc_edit7, on_btn_ina228_adc_edit7_clicked, LV_EVENT_CLICKED, NULL);
        }
    }

    if (!s_adc_settings_loaded) 
    {
        g_temp_adc_range     = g_user_settings.ina228_adc_range;
        g_temp_adc_mode      = g_user_settings.ina228_mode;
        g_temp_adc_sample    = g_user_settings.ina228_samples;
        g_temp_adc_convdelay = g_user_settings.ina228_conv_delay;
        g_temp_adc_vbus_ct   = g_user_settings.ina228_vbus_ct;
        g_temp_adc_vshunt_ct = g_user_settings.ina228_vsh_ct;
        g_temp_adc_temp_ct   = g_user_settings.ina228_temp_ct;
        s_adc_settings_loaded = true;
    }

    if (objects.lbl_val_adc_range != NULL) 
    {
        if (g_temp_adc_range == 1) 
        {
            lv_label_set_text(objects.lbl_val_adc_range, "+-40.96 mV");
        } 
        else 
        {
            lv_label_set_text(objects.lbl_val_adc_range, "+- 163.84 mV");
        }
    }

    if (objects.lbl_val_adc_mode != NULL) 
    {
        lv_label_set_text(objects.lbl_val_adc_mode, get_mode_text(g_temp_adc_mode));
    }

    if (objects.lbl_val_adc_samples != NULL) 
    {
        lv_label_set_text(objects.lbl_val_adc_samples, get_sample_text(g_temp_adc_sample));
    }

    if (objects.lbl_val_adc_convdelay != NULL)
    {
        lv_label_set_text_fmt(objects.lbl_val_adc_convdelay, "%d ms", g_temp_adc_convdelay);
    }

    if (objects.lbl_val_adc_vbus_time != NULL)
    {
        lv_label_set_text(objects.lbl_val_adc_vbus_time, get_conv_time_text(g_temp_adc_vbus_ct));
    }

    if (objects.lbl_val_adc_vshunt_time != NULL)
    {
        lv_label_set_text(objects.lbl_val_adc_vshunt_time, get_conv_time_text(g_temp_adc_vshunt_ct));
    }

    if (objects.lbl_val_adc_temp_time != NULL)
    {
        lv_label_set_text(objects.lbl_val_adc_temp_time, get_conv_time_text(g_temp_adc_temp_ct));
    }
}

/* Prepara las etiquetas al ingresar a la pantalla 2 de Calibración */
static void setup_ina228_cal_screen(void)
{
    if (objects.ina228_cal_screen == NULL) {
        create_screen_ina228_cal_screen();
        if (objects.btn_cal_prev != NULL)   lv_obj_add_event_cb(objects.btn_cal_prev, on_btn_cal_prev_clicked, LV_EVENT_CLICKED, NULL);
        if (objects.btn_cal_save != NULL)   lv_obj_add_event_cb(objects.btn_cal_save, on_btn_cal_save_clicked, LV_EVENT_CLICKED, NULL);
        if (objects.btn_cal_next != NULL)   lv_obj_add_event_cb(objects.btn_cal_next, on_btn_cal_next_clicked, LV_EVENT_CLICKED, NULL);
        
        /* <--- ENLAZAR EL BOTÓN EDIT1 DE SHUNT ---> */
        if (objects.btn_ina228_cal_edit1 != NULL) {
            lv_obj_add_event_cb(objects.btn_ina228_cal_edit1, on_btn_ina228_cal_edit1_clicked, LV_EVENT_CLICKED, NULL);
        }
        /* <--- ENLAZAR EL BOTÓN EDIT2 DE CORRIENTE MÁXIMA ---> */
        if (objects.btn_ina228_cal_edit2 != NULL) {
            lv_obj_add_event_cb(objects.btn_ina228_cal_edit2, on_btn_ina228_cal_edit2_clicked, LV_EVENT_CLICKED, NULL);
        }
    }

    /* Mostrar el valor actual del Shunt en la pantalla de calibración */
    if (objects.lbl_val_shunt_res != NULL) {
        int r_int = (int)g_user_settings.ina228_rshunt;
        int r_dec = (int)(abs((int)((g_user_settings.ina228_rshunt - r_int) * 10.0f + 0.5f)));
        lv_label_set_text_fmt(objects.lbl_val_shunt_res, "%d.%d", r_int, r_dec);
    }

    /* Mostrar la corriente máxima actual en la pantalla de calibración */
    if (objects.lbl_val_max_curr != NULL) {
        int i_int = (int)g_user_settings.ina228_max_current;
        int i_dec = (int)(abs((int)((g_user_settings.ina228_max_current - i_int) * 100.0f + 0.5f)));
        lv_label_set_text_fmt(objects.lbl_val_max_curr, "%d.%02d", i_int, i_dec);
    }

    uint16_t manuf_id = 0, device_id = 0, shunt_cal = 0;
    
    // Si el sensor INA228 está conectado y responde por I2C
    if (PowerRead_GetDeviceInfo(&manuf_id, &device_id, &shunt_cal) == HAL_OK) {
        if (objects.lbl_val_manuf_id != NULL)  lv_label_set_text_fmt(objects.lbl_val_manuf_id, "0x%04X", manuf_id);
        if (objects.lbl_val_device_id != NULL) lv_label_set_text_fmt(objects.lbl_val_device_id, "0x%04X", device_id);
        if (objects.lbl_val_shunt_cal != NULL) lv_label_set_text_fmt(objects.lbl_val_shunt_cal, "0x%04X", shunt_cal);
    } 
    else {
        if (objects.lbl_val_manuf_id != NULL)  lv_label_set_text(objects.lbl_val_manuf_id, "-");
        if (objects.lbl_val_device_id != NULL) lv_label_set_text(objects.lbl_val_device_id, "-");
        if (objects.lbl_val_shunt_cal != NULL) lv_label_set_text(objects.lbl_val_shunt_cal, "-");
    }
}

/* Prepara la pantalla 3 del INA228 (Alertas) */
static void setup_ina228_alert_screen(void)
{
    if (objects.ina228_alert_screen == NULL) {
        create_screen_ina228_alert_screen();
        if (objects.btn_alert_prev != NULL) lv_obj_add_event_cb(objects.btn_alert_prev, on_btn_alert_prev_clicked, LV_EVENT_CLICKED, NULL);
        if (objects.btn_alert_save != NULL) lv_obj_add_event_cb(objects.btn_alert_save, on_btn_alert_save_clicked, LV_EVENT_CLICKED, NULL);
        if (objects.btn_alert_next != NULL) lv_obj_add_event_cb(objects.btn_alert_next, on_btn_alert_next_clicked, LV_EVENT_CLICKED, NULL);

        if (objects.switch_alert1 != NULL) lv_obj_add_event_cb(objects.switch_alert1, on_switch_alert1_changed, LV_EVENT_VALUE_CHANGED, NULL);
        if (objects.switch_alert2 != NULL) lv_obj_add_event_cb(objects.switch_alert2, on_switch_alert2_changed, LV_EVENT_VALUE_CHANGED, NULL);
        if (objects.switch_alert3 != NULL) lv_obj_add_event_cb(objects.switch_alert3, on_switch_alert3_changed, LV_EVENT_VALUE_CHANGED, NULL);
        if (objects.switch_alert4 != NULL) lv_obj_add_event_cb(objects.switch_alert4, on_switch_alert4_changed, LV_EVENT_VALUE_CHANGED, NULL);
    }

    if (!s_alert_settings_loaded) {
        g_temp_alert_latch  = g_user_settings.ina228_alert_latch;
        g_temp_alert_cnvr   = g_user_settings.ina228_alert_cnvr;
        g_temp_alert_pol    = g_user_settings.ina228_alert_pol;
        g_temp_alert_filter = g_user_settings.ina228_alert_filter;
        s_alert_settings_loaded = true;
    }

    /* Estado de switch_alert1 (OFF: LATCHED, ON: TRANSPARENT) */
    if (objects.switch_alert1 != NULL) {
        if (g_temp_alert_latch == 1) {
            lv_obj_add_state(objects.switch_alert1, LV_STATE_CHECKED);
        } else {
            lv_obj_remove_state(objects.switch_alert1, LV_STATE_CHECKED);
        }
    }
    if (objects.lbl_alert_val1 != NULL) {
        lv_label_set_text(objects.lbl_alert_val1, (g_temp_alert_latch == 1) ? "TRANSPARENT" : "LATCHED");
    }

    /* Estado de switch_alert2 (OFF: OFF, ON: ON) */
    if (objects.switch_alert2 != NULL) {
        if (g_temp_alert_cnvr == 1) {
            lv_obj_add_state(objects.switch_alert2, LV_STATE_CHECKED);
        } else {
            lv_obj_remove_state(objects.switch_alert2, LV_STATE_CHECKED);
        }
    }
    if (objects.lbl_alert_val2 != NULL) {
        lv_label_set_text(objects.lbl_alert_val2, (g_temp_alert_cnvr == 1) ? "ON" : "OFF");
    }

    /* Estado de switch_alert3 (OFF: ACTIVE LOW, ON: ACTIVE HIGH) */
    if (objects.switch_alert3 != NULL) {
        if (g_temp_alert_pol == 1) {
            lv_obj_add_state(objects.switch_alert3, LV_STATE_CHECKED);
        } else {
            lv_obj_remove_state(objects.switch_alert3, LV_STATE_CHECKED);
        }
    }
    if (objects.lbl_alert_val3 != NULL) {
        lv_label_set_text(objects.lbl_alert_val3, (g_temp_alert_pol == 1) ? "ACTIVE HIGH" : "ACTIVE LOW");
    }

    /* Estado de switch_alert4 (OFF: NON AVERAGED, ON: AVERAGED) */
    if (objects.switch_alert4 != NULL) {
        if (g_temp_alert_filter == 1) {
            lv_obj_add_state(objects.switch_alert4, LV_STATE_CHECKED);
        } else {
            lv_obj_remove_state(objects.switch_alert4, LV_STATE_CHECKED);
        }
    }
    if (objects.lbl_alert_val4 != NULL) {
        lv_label_set_text(objects.lbl_alert_val4, (g_temp_alert_filter == 1) ? "AVERAGED" : "NON AVERAGED");
    }
}

/* Prepara la pantalla 4 del INA228 (Límites) */
static void setup_ina228_limits_screen(void)
{
    if (objects.ina228_limits_screen == NULL) {
        create_screen_ina228_limits_screen();
        if (objects.btn_thr_prev != NULL)   lv_obj_add_event_cb(objects.btn_thr_prev, on_btn_thr_prev_clicked, LV_EVENT_CLICKED, NULL);
        if (objects.btn_thr_save != NULL)   lv_obj_add_event_cb(objects.btn_thr_save, on_btn_thr_save_clicked, LV_EVENT_CLICKED, NULL);

        if (objects.btn_ina228_thr_edit1 != NULL) lv_obj_add_event_cb(objects.btn_ina228_thr_edit1, on_btn_ina228_thr_edit1_clicked, LV_EVENT_CLICKED, NULL);
        if (objects.btn_ina228_thr_edit2 != NULL) lv_obj_add_event_cb(objects.btn_ina228_thr_edit2, on_btn_ina228_thr_edit2_clicked, LV_EVENT_CLICKED, NULL);
        if (objects.btn_ina228_thr_edit3 != NULL) lv_obj_add_event_cb(objects.btn_ina228_thr_edit3, on_btn_ina228_thr_edit3_clicked, LV_EVENT_CLICKED, NULL);
        if (objects.btn_ina228_thr_edit4 != NULL) lv_obj_add_event_cb(objects.btn_ina228_thr_edit4, on_btn_ina228_thr_edit4_clicked, LV_EVENT_CLICKED, NULL);
        if (objects.btn_ina228_thr_edit5 != NULL) lv_obj_add_event_cb(objects.btn_ina228_thr_edit5, on_btn_ina228_thr_edit5_clicked, LV_EVENT_CLICKED, NULL);
        if (objects.btn_ina228_thr_edit6 != NULL) lv_obj_add_event_cb(objects.btn_ina228_thr_edit6, on_btn_ina228_thr_edit6_clicked, LV_EVENT_CLICKED, NULL);
    }

    if (!s_thr_settings_loaded) {
        g_temp_thr_sovl = g_user_settings.ina228_thr_sovl;
        g_temp_thr_suvl = g_user_settings.ina228_thr_suvl;
        g_temp_thr_bovl = g_user_settings.ina228_thr_bovl;
        g_temp_thr_buvl = g_user_settings.ina228_thr_buvl;
        g_temp_thr_temp = g_user_settings.ina228_thr_temp;
        g_temp_thr_pwr  = g_user_settings.ina228_thr_pwr;
        s_thr_settings_loaded = true;
    }

    /* Actualizar los 6 labels con los valores temporales */
    if (objects.lbl_val_thr1 != NULL) {
        int i_val = (int)g_temp_thr_sovl;
        int d_val = (int)(abs((int)((g_temp_thr_sovl - i_val) * 100.0f + 0.5f)));
        lv_label_set_text_fmt(objects.lbl_val_thr1, "%d.%02d mV", i_val, d_val);
    }
    if (objects.lbl_val_thr2 != NULL) {
        int i_val = (int)g_temp_thr_suvl;
        int d_val = (int)(abs((int)((g_temp_thr_suvl - i_val) * 100.0f + (g_temp_thr_suvl >= 0 ? 0.5f : -0.5f))));
        if (g_temp_thr_suvl < 0 && i_val == 0) {
            lv_label_set_text_fmt(objects.lbl_val_thr2, "-%d.%02d mV", abs(i_val), d_val);
        } else {
            lv_label_set_text_fmt(objects.lbl_val_thr2, "%d.%02d mV", i_val, d_val);
        }
    }
    if (objects.lbl_val_thr3 != NULL) {
        int i_val = (int)g_temp_thr_bovl;
        int d_val = (int)(abs((int)((g_temp_thr_bovl - i_val) * 100.0f + 0.5f)));
        lv_label_set_text_fmt(objects.lbl_val_thr3, "%d.%02d V", i_val, d_val);
    }
    if (objects.lbl_val_thr4 != NULL) {
        int i_val = (int)g_temp_thr_buvl;
        int d_val = (int)(abs((int)((g_temp_thr_buvl - i_val) * 100.0f + 0.5f)));
        lv_label_set_text_fmt(objects.lbl_val_thr4, "%d.%02d V", i_val, d_val);
    }
    if (objects.lbl_val_thr5 != NULL) {
        int i_val = (int)g_temp_thr_temp;
        int d_val = (int)(abs((int)((g_temp_thr_temp - i_val) * 10.0f + 0.5f)));
        lv_label_set_text_fmt(objects.lbl_val_thr5, "%d.%d °C", i_val, d_val);
    }
    if (objects.lbl_val_thr6 != NULL) {
        int i_val = (int)g_temp_thr_pwr;
        int d_val = (int)(abs((int)((g_temp_thr_pwr - i_val) * 10.0f + 0.5f)));
        lv_label_set_text_fmt(objects.lbl_val_thr6, "%d.%d W", i_val, d_val);
    }
}

/* Tablas de escala estándar de osciloscopio (secuencia 1 - 2 - 5) */
static const float s_volt_div_steps[] = {
    0.010f, // 10 mV/div  (FS = 40 mV)
    0.020f, // 20 mV/div  (FS = 80 mV)
    0.050f, // 50 mV/div  (FS = 200 mV)
    0.100f, // 100 mV/div (FS = 400 mV)
    0.200f, // 200 mV/div (FS = 800 mV)
    0.500f, // 500 mV/div (FS = 2.0 V)
    1.000f, // 1.0 V/div  (FS = 4.0 V)
    2.000f, // 2.0 V/div  (FS = 8.0 V)
    5.000f, // 5.0 V/div  (FS = 20.0 V)
    10.00f, // 10.0 V/div (FS = 40.0 V)
    15.00f  // 15.0 V/div (FS = 60.0 V)
};
#define VOLT_DIV_STEPS_COUNT (sizeof(s_volt_div_steps) / sizeof(s_volt_div_steps[0]))

static const float s_curr_div_steps[] = {
    0.010f, // 10 mA/div  (FS = 40 mA)
    0.020f, // 20 mA/div  (FS = 80 mA)
    0.050f, // 50 mA/div  (FS = 200 mA)
    0.100f, // 100 mA/div (FS = 400 mA)
    0.200f, // 200 mA/div (FS = 800 mA)
    0.500f, // 500 mA/div (FS = 2.0 A)
    1.000f, // 1.0 A/div  (FS = 4.0 A)
    2.000f  // 2.0 A/div  (FS = 8.0 A)
};
#define CURR_DIV_STEPS_COUNT (sizeof(s_curr_div_steps) / sizeof(s_curr_div_steps[0]))

static int s_v_step_idx = -1;
static int s_i_step_idx = -1;
static int s_v_down_cnt = 0;
static int s_i_down_cnt = 0;

static void update_volt_div_label(float v_step)
{
    if (objects.lbl_volt_div == NULL) return;
    lv_obj_set_style_text_color(objects.lbl_volt_div, lv_color_hex(0xBB86FC), LV_PART_MAIN);
    if (v_step < 0.999f) {
        int mv = (int)(v_step * 1000.0f + 0.5f);
        lv_label_set_text_fmt(objects.lbl_volt_div, "V: %d mV/div", mv);
    } else {
        int v_int = (int)v_step;
        int v_dec = (int)(abs((int)((v_step - v_int) * 10.0f + 0.5f)));
        if (v_dec == 0) {
            lv_label_set_text_fmt(objects.lbl_volt_div, "V: %d V/div", v_int);
        } else {
            lv_label_set_text_fmt(objects.lbl_volt_div, "V: %d.%d V/div", v_int, v_dec);
        }
    }
}

static void update_curr_div_label(float i_step)
{
    if (objects.lbl_curr_div == NULL) return;
    lv_obj_set_style_text_color(objects.lbl_curr_div, lv_color_hex(0x50C9FF), LV_PART_MAIN);
    if (i_step < 0.999f) {
        int ma = (int)(i_step * 1000.0f + 0.5f);
        lv_label_set_text_fmt(objects.lbl_curr_div, "I: %d mA/div", ma);
    } else {
        int i_int = (int)i_step;
        int i_dec = (int)(abs((int)((i_step - i_int) * 10.0f + 0.5f)));
        if (i_dec == 0) {
            lv_label_set_text_fmt(objects.lbl_curr_div, "I: %d A/div", i_int);
        } else {
            lv_label_set_text_fmt(objects.lbl_curr_div, "I: %d.%d A/div", i_int, i_dec);
        }
    }
}

static void update_auto_range(float v_meas, float i_meas)
{
    if (objects.obj3 == NULL) return;

    /* 1. Auto-rango de Voltaje */
    bool v_changed = false;
    if (s_v_step_idx < 0) {
        s_v_step_idx = 0;
        for (int idx = 0; idx < (int)VOLT_DIV_STEPS_COUNT; idx++) {
            float fs = s_volt_div_steps[idx] * 4.0f;
            if (v_meas <= fs * 0.85f || idx == (int)VOLT_DIV_STEPS_COUNT - 1) {
                s_v_step_idx = idx;
                break;
            }
        }
        s_v_down_cnt = 0;
        v_changed = true;
    } else {
        float fs = s_volt_div_steps[s_v_step_idx] * 4.0f;
        /* Subir de escala inmediatamente si supera el 85% del fondo */
        if (v_meas > fs * 0.85f && s_v_step_idx < (int)VOLT_DIV_STEPS_COUNT - 1) {
            s_v_step_idx++;
            s_v_down_cnt = 0;
            v_changed = true;
        }
        /* Bajar de escala con histeresis tras 5 ciclos por debajo del escalón anterior */
        else if (s_v_step_idx > 0 && v_meas < (s_volt_div_steps[s_v_step_idx - 1] * 4.0f * 0.75f)) {
            if (++s_v_down_cnt >= 5) {
                s_v_step_idx--;
                s_v_down_cnt = 0;
                v_changed = true;
            }
        } else {
            s_v_down_cnt = 0;
        }
    }

    if (v_changed) {
        float v_step = s_volt_div_steps[s_v_step_idx];
        int32_t fs_v_mv = (int32_t)(v_step * 4.0f * 1000.0f + 0.5f);
        lv_chart_set_axis_range(objects.obj3, LV_CHART_AXIS_PRIMARY_Y, 0, fs_v_mv);
        update_volt_div_label(v_step);
    }

    /* 2. Auto-rango de Corriente */
    bool i_changed = false;
    if (s_i_step_idx < 0) {
        s_i_step_idx = 0;
        for (int idx = 0; idx < (int)CURR_DIV_STEPS_COUNT; idx++) {
            float fs = s_curr_div_steps[idx] * 4.0f;
            if (i_meas <= fs * 0.85f || idx == (int)CURR_DIV_STEPS_COUNT - 1) {
                s_i_step_idx = idx;
                break;
            }
        }
        s_i_down_cnt = 0;
        i_changed = true;
    } else {
        float fs = s_curr_div_steps[s_i_step_idx] * 4.0f;
        /* Subir de escala inmediatamente si supera el 85% del fondo */
        if (i_meas > fs * 0.85f && s_i_step_idx < (int)CURR_DIV_STEPS_COUNT - 1) {
            s_i_step_idx++;
            s_i_down_cnt = 0;
            i_changed = true;
        }
        /* Bajar de escala con histeresis tras 5 ciclos por debajo del escalón anterior */
        else if (s_i_step_idx > 0 && i_meas < (s_curr_div_steps[s_i_step_idx - 1] * 4.0f * 0.75f)) {
            if (++s_i_down_cnt >= 5) {
                s_i_step_idx--;
                s_i_down_cnt = 0;
                i_changed = true;
            }
        } else {
            s_i_down_cnt = 0;
        }
    }

    if (i_changed) {
        float i_step = s_curr_div_steps[s_i_step_idx];
        int32_t fs_i_ma = (int32_t)(i_step * 4.0f * 1000.0f + 0.5f);
        lv_chart_set_axis_range(objects.obj3, LV_CHART_AXIS_SECONDARY_Y, 0, fs_i_ma);
        update_curr_div_label(i_step);
    }
}

/* Dibuja las marcas de subdivisión (sub-ticks de osciloscopio) únicamente en el eje vertical */
static void on_chart_draw_post(lv_event_t * e)
{
    lv_layer_t * layer = lv_event_get_layer(e);
    lv_obj_t * obj = lv_event_get_target(e);

    lv_area_t coords;
    lv_obj_get_coords(obj, &coords);
    int32_t y1 = coords.y1;
    int32_t w  = lv_obj_get_content_width(obj);
    int32_t h  = lv_obj_get_content_height(obj);

    int32_t x_mid = coords.x1 + w / 2;

    lv_draw_line_dsc_t line_dsc;
    lv_draw_line_dsc_init(&line_dsc);
    line_dsc.color = lv_color_hex(0x777777);
    line_dsc.opa = LV_OPA_60;
    line_dsc.width = 1;

    /* 4 divisiones mayores -> 20 subdivisiones (0.2 div por marca) solo en el eje vertical */
    for (int k = 1; k < 20; k++) {
        if (k % 5 == 0) continue; /* Saltar los cruces con las líneas principales */
        int32_t y = y1 + (h * k) / 20;
        line_dsc.p1.x = x_mid - 2;
        line_dsc.p2.x = x_mid + 2;
        line_dsc.p1.y = y;
        line_dsc.p2.y = y;
        lv_draw_line(layer, &line_dsc);
    }
}

/* Prepara la pantalla de Osciloscopio en Tiempo Real (graph_screen) */
static void setup_graph_screen(void)
{
    if (objects.graph_screen == NULL) {
        create_screen_graph_screen();

        if (objects.btn_graph_back != NULL) {
            lv_obj_add_event_cb(objects.btn_graph_back, on_btn_graph_back_clicked, LV_EVENT_CLICKED, NULL);
        }
        if (objects.btn_graph_pause != NULL) {
            lv_obj_add_event_cb(objects.btn_graph_pause, on_btn_graph_pause_clicked, LV_EVENT_CLICKED, NULL);
        }
        if (objects.btn_graph_clear != NULL) {
            lv_obj_add_event_cb(objects.btn_graph_clear, on_btn_graph_clear_clicked, LV_EVENT_CLICKED, NULL);
        }

        if (objects.obj3 != NULL) {
            lv_chart_set_type(objects.obj3, LV_CHART_TYPE_LINE);
            lv_chart_set_update_mode(objects.obj3, LV_CHART_UPDATE_MODE_SHIFT);
            lv_chart_set_point_count(objects.obj3, 50);
            /* 5 líneas horizontales (3 internas) y 5 líneas verticales (3 internas) */
            lv_chart_set_div_line_count(objects.obj3, 5, 5);

            /* Fondo idéntico al tema (#121212) para evitar contrastes residuales */
            lv_obj_set_style_bg_color(objects.obj3, lv_color_hex(0x121212), LV_PART_MAIN);

            /* División/rejilla del osciloscopio: color gris (#555555), opacidad tenue y punteada */
            lv_obj_set_style_line_color(objects.obj3, lv_color_hex(0x555555), LV_PART_MAIN);
            lv_obj_set_style_line_opa(objects.obj3, LV_OPA_40, LV_PART_MAIN);
            lv_obj_set_style_line_width(objects.obj3, 1, LV_PART_MAIN);
            lv_obj_set_style_line_dash_width(objects.obj3, 2, LV_PART_MAIN);
            lv_obj_set_style_line_dash_gap(objects.obj3, 3, LV_PART_MAIN);

            /* Asegurar que la cuadrícula inicie en (0,0) sin márgenes ni marcos adicionales */
            lv_obj_set_style_pad_all(objects.obj3, 0, LV_PART_MAIN);
            lv_obj_set_style_border_width(objects.obj3, 0, LV_PART_MAIN);

            /* Quitar puntos circulares para trazo limpio de osciloscopio */
            lv_obj_set_style_size(objects.obj3, 0, 0, LV_PART_INDICATOR);
            lv_obj_set_style_line_width(objects.obj3, 2, LV_PART_ITEMS);

            /* Evento para dibujar subdivisiones (ticks) en la cruz central */
            lv_obj_add_event_cb(objects.obj3, on_chart_draw_post, LV_EVENT_DRAW_POST, NULL);

            /* Serie Voltaje: Púrpura (#BB86FC) asignado al Eje Primario Y */
            s_ser_volt = lv_chart_add_series(objects.obj3, lv_color_hex(0xBB86FC), LV_CHART_AXIS_PRIMARY_Y);
            /* Serie Corriente: Azul claro (#50C9FF) asignado al Eje Secundario Y */
            s_ser_curr = lv_chart_add_series(objects.obj3, lv_color_hex(0x50C9FF), LV_CHART_AXIS_SECONDARY_Y);

            if (s_ser_volt != NULL) lv_chart_set_all_values(objects.obj3, s_ser_volt, 0);
            if (s_ser_curr != NULL) lv_chart_set_all_values(objects.obj3, s_ser_curr, 0);
        }
    }

    /* Inicializar auto-rango dinámico según lectura instantánea actual */
    s_v_step_idx = -1;
    s_i_step_idx = -1;
    update_auto_range(g_power_sim.voltage, g_power_sim.current);

    s_graph_paused = false;
    if (objects.btn_label_graph_2 != NULL) {
        lv_label_set_text(objects.btn_label_graph_2, "PAUSE");
    }
    if (objects.btn_graph_pause != NULL) {
        lv_obj_set_style_bg_color(objects.btn_graph_pause, lv_color_hex(0xFFC241), LV_PART_MAIN | LV_STATE_DEFAULT);
    }
}

/* Función auxiliar multimodo para previsualizar el slider y las etiquetas */
static void update_limit_preview(int slider_val)
{
    if (g_active_limit_mode == MODE_BRIGHTNESS) {
        if (slider_val < 0) slider_val = 0;
        if (slider_val > 100) slider_val = 100;
        
        g_temp_brightness = (uint8_t)slider_val;

        if (objects.limit_slider != NULL && lv_slider_get_value(objects.limit_slider) != slider_val) {
            lv_slider_set_value(objects.limit_slider, slider_val, LV_ANIM_OFF);
        }
        if (objects.lbl_limit_val != NULL) {
            lv_label_set_text_fmt(objects.lbl_limit_val, "%d", slider_val);
        }

        Display_SetBrightness(g_temp_brightness);
    }
    else if (g_active_limit_mode == MODE_CURRENT_LIMIT) {
        if (slider_val < 0) slider_val = 0;
        if (slider_val > 650) slider_val = 650;
        
        g_temp_current_limit = slider_val / 100.0f;

        if (objects.limit_slider != NULL && lv_slider_get_value(objects.limit_slider) != slider_val) {
            lv_slider_set_value(objects.limit_slider, slider_val, LV_ANIM_OFF);
        }
        if (objects.lbl_limit_val != NULL) {
            int i_int = (int)g_temp_current_limit;
            int i_dec = (int)(abs((int)((g_temp_current_limit - i_int) * 100)));
            lv_label_set_text_fmt(objects.lbl_limit_val, "%d.%02d", i_int, i_dec);
        }
    }
    else if (g_active_limit_mode == MODE_OVP_LIMIT) 
    {
        if (slider_val < 100) slider_val = 100;
        if (slider_val > 5400) slider_val = 5400;
        
        g_temp_ovp_limit = slider_val / 100.0f;

        if (objects.limit_slider != NULL && lv_slider_get_value(objects.limit_slider) != slider_val) 
        {
            lv_slider_set_value(objects.limit_slider, slider_val, LV_ANIM_OFF);
        }

        if (objects.lbl_limit_val != NULL) 
        {
            int v_int = slider_val / 100;
            int v_dec = slider_val % 100;
            lv_label_set_text_fmt(objects.lbl_limit_val, "%d.%02d", v_int, v_dec);
        }
    }
    else if (g_active_limit_mode == MODE_MAX_CURRENT) {
        if (slider_val < 0) slider_val = 0;
        if (slider_val > 650) slider_val = 650;
        
        g_temp_max_current = slider_val / 100.0f;

        if (objects.limit_slider != NULL && lv_slider_get_value(objects.limit_slider) != slider_val) {
            lv_slider_set_value(objects.limit_slider, slider_val, LV_ANIM_OFF);
        }
        if (objects.lbl_limit_val != NULL) {
            int i_int = (int)g_temp_max_current;
            int i_dec = (int)(abs((int)((g_temp_max_current - i_int) * 100.0f + 0.5f)));
            lv_label_set_text_fmt(objects.lbl_limit_val, "%d.%02d", i_int, i_dec);
        }
    }
    else if (g_active_limit_mode == MODE_THR_SOVL) {
        if (slider_val < 0) slider_val = 0;
        if (slider_val > 16000) slider_val = 16000;
        
        g_temp_thr_sovl = slider_val / 100.0f;

        if (objects.limit_slider != NULL && lv_slider_get_value(objects.limit_slider) != slider_val) {
            lv_slider_set_value(objects.limit_slider, slider_val, LV_ANIM_OFF);
        }
        if (objects.lbl_limit_val != NULL) {
            int i_val = (int)g_temp_thr_sovl;
            int d_val = (int)(abs((int)((g_temp_thr_sovl - i_val) * 100.0f + 0.5f)));
            lv_label_set_text_fmt(objects.lbl_limit_val, "%d.%02d", i_val, d_val);
        }
    }
    else if (g_active_limit_mode == MODE_THR_SUVL) {
        if (slider_val < -10000) slider_val = -10000;
        if (slider_val > 5000) slider_val = 5000;
        
        g_temp_thr_suvl = slider_val / 100.0f;

        if (objects.limit_slider != NULL && lv_slider_get_value(objects.limit_slider) != slider_val) {
            lv_slider_set_value(objects.limit_slider, slider_val, LV_ANIM_OFF);
        }
        if (objects.lbl_limit_val != NULL) {
            int i_val = (int)g_temp_thr_suvl;
            int d_val = (int)(abs((int)((g_temp_thr_suvl - i_val) * 100.0f + (g_temp_thr_suvl >= 0 ? 0.5f : -0.5f))));
            if (g_temp_thr_suvl < 0 && i_val == 0) {
                lv_label_set_text_fmt(objects.lbl_limit_val, "-%d.%02d", abs(i_val), d_val);
            } else {
                lv_label_set_text_fmt(objects.lbl_limit_val, "%d.%02d", i_val, d_val);
            }
        }
    }
    else if (g_active_limit_mode == MODE_THR_BOVL) {
        if (slider_val < 100) slider_val = 100;
        if (slider_val > 6000) slider_val = 6000;
        
        g_temp_thr_bovl = slider_val / 100.0f;

        if (objects.limit_slider != NULL && lv_slider_get_value(objects.limit_slider) != slider_val) {
            lv_slider_set_value(objects.limit_slider, slider_val, LV_ANIM_OFF);
        }
        if (objects.lbl_limit_val != NULL) {
            int i_val = (int)g_temp_thr_bovl;
            int d_val = (int)(abs((int)((g_temp_thr_bovl - i_val) * 100.0f + 0.5f)));
            lv_label_set_text_fmt(objects.lbl_limit_val, "%d.%02d", i_val, d_val);
        }
    }
    else if (g_active_limit_mode == MODE_THR_BUVL) {
        if (slider_val < 0) slider_val = 0;
        if (slider_val > 4800) slider_val = 4800;
        
        g_temp_thr_buvl = slider_val / 100.0f;

        if (objects.limit_slider != NULL && lv_slider_get_value(objects.limit_slider) != slider_val) {
            lv_slider_set_value(objects.limit_slider, slider_val, LV_ANIM_OFF);
        }
        if (objects.lbl_limit_val != NULL) {
            int i_val = (int)g_temp_thr_buvl;
            int d_val = (int)(abs((int)((g_temp_thr_buvl - i_val) * 100.0f + 0.5f)));
            lv_label_set_text_fmt(objects.lbl_limit_val, "%d.%02d", i_val, d_val);
        }
    }
    else if (g_active_limit_mode == MODE_THR_TEMP) {
        if (slider_val < 0) slider_val = 0;
        if (slider_val > 1250) slider_val = 1250;
        
        g_temp_thr_temp = slider_val / 10.0f;

        if (objects.limit_slider != NULL && lv_slider_get_value(objects.limit_slider) != slider_val) {
            lv_slider_set_value(objects.limit_slider, slider_val, LV_ANIM_OFF);
        }
        if (objects.lbl_limit_val != NULL) {
            int i_val = (int)g_temp_thr_temp;
            int d_val = (int)(abs((int)((g_temp_thr_temp - i_val) * 10.0f + 0.5f)));
            lv_label_set_text_fmt(objects.lbl_limit_val, "%d.%d", i_val, d_val);
        }
    }
    else if (g_active_limit_mode == MODE_THR_PWR) {
        if (slider_val < 0) slider_val = 0;
        if (slider_val > 3500) slider_val = 3500;
        
        g_temp_thr_pwr = slider_val / 10.0f;

        if (objects.limit_slider != NULL && lv_slider_get_value(objects.limit_slider) != slider_val) {
            lv_slider_set_value(objects.limit_slider, slider_val, LV_ANIM_OFF);
        }
        if (objects.lbl_limit_val != NULL) {
            int i_val = (int)g_temp_thr_pwr;
            int d_val = (int)(abs((int)((g_temp_thr_pwr - i_val) * 10.0f + 0.5f)));
            lv_label_set_text_fmt(objects.lbl_limit_val, "%d.%d", i_val, d_val);
        }
    }
    else if (g_active_limit_mode == MODE_INA_CONVDLY) {
        if (slider_val < 0) slider_val = 0;
        if (slider_val > 255) slider_val = 255;
        
        g_temp_adc_convdelay = (uint16_t)slider_val * 2;

        if (objects.limit_slider != NULL && lv_slider_get_value(objects.limit_slider) != slider_val) {
            lv_slider_set_value(objects.limit_slider, slider_val, LV_ANIM_OFF);
        }
        if (objects.lbl_limit_val != NULL) {
            lv_label_set_text_fmt(objects.lbl_limit_val, "%d", g_temp_adc_convdelay);
        }
    }
}

static void setup_current_limit_screen(void)
{
    setup_limit_screen();
    g_active_limit_mode = MODE_CURRENT_LIMIT;
    g_temp_current_limit = g_user_settings.ocp_limit;

    if (objects.lbl_limit_text != NULL) {
        lv_label_set_text(objects.lbl_limit_text, "CURRENT LIMIT ADJUSTMENT (eFuse)");
    }
    if (objects.lbl_limit_unit != NULL) {
        lv_label_set_text(objects.lbl_limit_unit, "A");
    }
    if (objects.lbl_limit_min != NULL) {
        lv_label_set_text(objects.lbl_limit_min, "0.00 A");
    }
    if (objects.lbl_limit_max != NULL) {
        lv_label_set_text(objects.lbl_limit_max, "6.50 A");
        lv_obj_update_layout(objects.lbl_limit_max); 
        lv_obj_set_x(objects.lbl_limit_max, lv_obj_get_x(objects.lbl_limit_max));
    }

    if (objects.label_limit_dec != NULL) {
        lv_label_set_text(objects.label_limit_dec, "-0.5A");
    }
    if (objects.label_limit_inc != NULL) {
        lv_label_set_text(objects.label_limit_inc, "+0.5A");
    }
    if (objects.label_limit_preset1 != NULL) {
        lv_label_set_text(objects.label_limit_preset1, "1.0 A");
    }
    if (objects.label_limit_preset2 != NULL) {
        lv_label_set_text(objects.label_limit_preset2, "3.0 A");
    }
    if (objects.label_limit_preset3 != NULL) {
        lv_label_set_text(objects.label_limit_preset3, "5.0 A");
    }

    if (objects.limit_slider != NULL) {
        lv_slider_set_range(objects.limit_slider, 0, 650);
        int slider_val = (int)(g_temp_current_limit * 100.0f);
        lv_slider_set_value(objects.limit_slider, slider_val, LV_ANIM_OFF);
    }
    
    if (objects.lbl_limit_val != NULL) {
        int i_int = (int)g_temp_current_limit;
        int i_dec = (int)(abs((int)((g_temp_current_limit - i_int) * 100)));
        lv_label_set_text_fmt(objects.lbl_limit_val, "%d.%02d", i_int, i_dec);
    }
}

static void setup_ovp_limit_screen(void)
{
    setup_limit_screen();
    g_active_limit_mode = MODE_OVP_LIMIT;
    g_temp_ovp_limit = g_user_settings.ovp_limit;

    if (objects.lbl_limit_text != NULL) {
        lv_label_set_text(objects.lbl_limit_text, "OVERVOLTAGE PROTECTION (OVP)");
    }
    if (objects.lbl_limit_unit != NULL) {
        lv_label_set_text(objects.lbl_limit_unit, "V");
    }
    if (objects.lbl_limit_min != NULL) {
        lv_label_set_text(objects.lbl_limit_min, "1.00 V");
    }
    if (objects.lbl_limit_max != NULL) {
        lv_label_set_text(objects.lbl_limit_max, "54.00 V");
        lv_obj_update_layout(objects.lbl_limit_max); 
        lv_obj_set_x(objects.lbl_limit_max, 255);
    }

    if (objects.label_limit_dec != NULL) {
        lv_label_set_text(objects.label_limit_dec, "-0.1V");
    }
    if (objects.label_limit_inc != NULL) {
        lv_label_set_text(objects.label_limit_inc, "+0.1V");
    }
    if (objects.label_limit_preset1 != NULL) {
        lv_label_set_text(objects.label_limit_preset1, "5.5 V");
    }
    if (objects.label_limit_preset2 != NULL) {
        lv_label_set_text(objects.label_limit_preset2, "12.0 V");
    }
    if (objects.label_limit_preset3 != NULL) {
        lv_label_set_text(objects.label_limit_preset3, "24.0 V");
    }

    if (objects.limit_slider != NULL) {
        lv_slider_set_range(objects.limit_slider, 100, 5400);
        int slider_val = (int)(g_temp_ovp_limit * 100.0f);
        lv_slider_set_value(objects.limit_slider, slider_val, LV_ANIM_OFF);
    }
    
    if (objects.lbl_limit_val != NULL) {
        int v_int = (int)g_temp_ovp_limit;
        int v_dec = (int)(abs((int)((g_temp_ovp_limit - v_int) * 100)));
        lv_label_set_text_fmt(objects.lbl_limit_val, "%d.%02d", v_int, v_dec);
    }
}

static void setup_brightness_screen(void)
{
    setup_limit_screen();
    g_active_limit_mode = MODE_BRIGHTNESS;
    g_temp_brightness = g_saved_brightness;

    if (objects.lbl_limit_text != NULL) {
        lv_label_set_text(objects.lbl_limit_text, "DISPLAY BRIGHTNESS ADJUSTMENT");
    }
    if (objects.lbl_limit_unit != NULL) {
        lv_label_set_text(objects.lbl_limit_unit, "%");
    }
    if (objects.lbl_limit_min != NULL) {
        lv_label_set_text(objects.lbl_limit_min, "0 %");
    }
    if (objects.lbl_limit_max != NULL) {
        lv_label_set_text(objects.lbl_limit_max, "100 %");
        lv_obj_update_layout(objects.lbl_limit_max); 
        lv_obj_set_x(objects.lbl_limit_max, 261);
    }

    if (objects.label_limit_dec != NULL) {
        lv_label_set_text(objects.label_limit_dec, "-10%");
    }
    if (objects.label_limit_inc != NULL) {
        lv_label_set_text(objects.label_limit_inc, "+10%");
    }
    if (objects.label_limit_preset1 != NULL) {
        lv_label_set_text(objects.label_limit_preset1, "10%");
    }
    if (objects.label_limit_preset2 != NULL) {
        lv_label_set_text(objects.label_limit_preset2, "50%");
    }
    if (objects.label_limit_preset3 != NULL) {
        lv_label_set_text(objects.label_limit_preset3, "100%");
    }

    if (objects.limit_slider != NULL) {
        lv_slider_set_range(objects.limit_slider, 0, 100);
        lv_slider_set_value(objects.limit_slider, g_temp_brightness, LV_ANIM_OFF);
    }
    if (objects.lbl_limit_val != NULL) {
        lv_label_set_text_fmt(objects.lbl_limit_val, "%d", g_temp_brightness);
    }
}

static void setup_shunt_limit_screen(void)
{
    setup_limit_screen();
    g_active_limit_mode = MODE_SHUNT_RESISTOR;
    g_temp_shunt_res    = g_user_settings.ina228_rshunt;

    if (objects.lbl_limit_text != NULL)   lv_label_set_text(objects.lbl_limit_text, "SHUNT RESISTANCE ADJUSTMENT");
    if (objects.lbl_limit_unit != NULL)   lv_label_set_text(objects.lbl_limit_unit, "mOhms");
    if (objects.lbl_limit_min != NULL)    lv_label_set_text(objects.lbl_limit_min, "1 mOhm");
    if (objects.lbl_limit_max != NULL)    lv_label_set_text(objects.lbl_limit_max, "100 mOhms");

    if (objects.label_limit_dec != NULL)  lv_label_set_text(objects.label_limit_dec, "-0.5m");
    if (objects.label_limit_inc != NULL)  lv_label_set_text(objects.label_limit_inc, "+0.5m");

    if (objects.label_limit_preset1 != NULL) lv_label_set_text(objects.label_limit_preset1, "5m");
    if (objects.label_limit_preset2 != NULL) lv_label_set_text(objects.label_limit_preset2, "15m");
    if (objects.label_limit_preset3 != NULL) lv_label_set_text(objects.label_limit_preset3, "50m");

    if (objects.limit_slider != NULL) {
        lv_slider_set_range(objects.limit_slider, 2, 200);
    }

    update_shunt_display(g_temp_shunt_res);
}

/* Actualiza el valor del Shunt en pantalla sin usar %f */
static void update_shunt_display(float val)
{
    g_temp_shunt_res = val;
    
    if (objects.lbl_limit_val != NULL) {
        int r_int = (int)g_temp_shunt_res;
        int r_dec = (int)(abs((int)((g_temp_shunt_res - r_int) * 10.0f + 0.5f)));
        lv_label_set_text_fmt(objects.lbl_limit_val, "%d.%d", r_int, r_dec);
    }
    
    if (objects.limit_slider != NULL) {
        lv_slider_set_value(objects.limit_slider, (int32_t)(g_temp_shunt_res * 2.0f), LV_ANIM_OFF);
    }
}

static void setup_max_current_limit_screen(void)
{
    setup_limit_screen();
    g_active_limit_mode = MODE_MAX_CURRENT;
    g_temp_max_current  = g_user_settings.ina228_max_current;

    if (objects.lbl_limit_text != NULL) {
        lv_label_set_text(objects.lbl_limit_text, "SET MAX CURRENT");
    }
    if (objects.lbl_limit_unit != NULL) {
        lv_label_set_text(objects.lbl_limit_unit, "A");
    }
    if (objects.lbl_limit_min != NULL) {
        lv_label_set_text(objects.lbl_limit_min, "0.00 A");
    }
    if (objects.lbl_limit_max != NULL) {
        lv_label_set_text(objects.lbl_limit_max, "6.50 A");
        lv_obj_update_layout(objects.lbl_limit_max); 
        lv_obj_set_x(objects.lbl_limit_max, lv_obj_get_x(objects.lbl_limit_max));
    }

    if (objects.label_limit_dec != NULL) {
        lv_label_set_text(objects.label_limit_dec, "-0.5A");
    }
    if (objects.label_limit_inc != NULL) {
        lv_label_set_text(objects.label_limit_inc, "+0.5A");
    }
    if (objects.label_limit_preset1 != NULL) {
        lv_label_set_text(objects.label_limit_preset1, "1.0 A");
    }
    if (objects.label_limit_preset2 != NULL) {
        lv_label_set_text(objects.label_limit_preset2, "3.0 A");
    }
    if (objects.label_limit_preset3 != NULL) {
        lv_label_set_text(objects.label_limit_preset3, "5.0 A");
    }

    if (objects.limit_slider != NULL) {
        lv_slider_set_range(objects.limit_slider, 0, 650);
        int slider_val = (int)(g_temp_max_current * 100.0f);
        lv_slider_set_value(objects.limit_slider, slider_val, LV_ANIM_OFF);
    }
    
    if (objects.lbl_limit_val != NULL) {
        int i_int = (int)g_temp_max_current;
        int i_dec = (int)(abs((int)((g_temp_max_current - i_int) * 100.0f + 0.5f)));
        lv_label_set_text_fmt(objects.lbl_limit_val, "%d.%02d", i_int, i_dec);
    }
}

static void setup_thr_sovl_screen(void)
{
    setup_limit_screen();
    g_active_limit_mode = MODE_THR_SOVL;

    if (objects.lbl_limit_text != NULL)   lv_label_set_text(objects.lbl_limit_text, "SHUNT OVERVOLTAGE (SOVL)");
    if (objects.lbl_limit_unit != NULL)   lv_label_set_text(objects.lbl_limit_unit, "mV");
    if (objects.lbl_limit_min != NULL)    lv_label_set_text(objects.lbl_limit_min, "0.00 mV");
    if (objects.lbl_limit_max != NULL) {
        lv_label_set_text(objects.lbl_limit_max, "160.00 mV");
        lv_obj_update_layout(objects.lbl_limit_max); 
        lv_obj_set_x(objects.lbl_limit_max, 240);
    }

    if (objects.label_limit_dec != NULL)  lv_label_set_text(objects.label_limit_dec, "-1mV");
    if (objects.label_limit_inc != NULL)  lv_label_set_text(objects.label_limit_inc, "+1mV");

    if (objects.label_limit_preset1 != NULL) lv_label_set_text(objects.label_limit_preset1, "25 mV");
    if (objects.label_limit_preset2 != NULL) lv_label_set_text(objects.label_limit_preset2, "50 mV");
    if (objects.label_limit_preset3 != NULL) lv_label_set_text(objects.label_limit_preset3, "100 mV");

    if (objects.limit_slider != NULL) {
        lv_slider_set_range(objects.limit_slider, 0, 16000);
        int slider_val = (int)(g_temp_thr_sovl * 100.0f + 0.5f);
        lv_slider_set_value(objects.limit_slider, slider_val, LV_ANIM_OFF);
    }
    
    if (objects.lbl_limit_val != NULL) {
        int i_val = (int)g_temp_thr_sovl;
        int d_val = (int)(abs((int)((g_temp_thr_sovl - i_val) * 100.0f + 0.5f)));
        lv_label_set_text_fmt(objects.lbl_limit_val, "%d.%02d", i_val, d_val);
    }
}

static void setup_thr_suvl_screen(void)
{
    setup_limit_screen();
    g_active_limit_mode = MODE_THR_SUVL;

    if (objects.lbl_limit_text != NULL)   lv_label_set_text(objects.lbl_limit_text, "SHUNT UNDERVOLTAGE (SUVL)");
    if (objects.lbl_limit_unit != NULL)   lv_label_set_text(objects.lbl_limit_unit, "mV");
    if (objects.lbl_limit_min != NULL)    lv_label_set_text(objects.lbl_limit_min, "-100.00 mV");
    if (objects.lbl_limit_max != NULL) {
        lv_label_set_text(objects.lbl_limit_max, "50.00 mV");
        lv_obj_update_layout(objects.lbl_limit_max); 
        lv_obj_set_x(objects.lbl_limit_max, 250);
    }

    if (objects.label_limit_dec != NULL)  lv_label_set_text(objects.label_limit_dec, "-1mV");
    if (objects.label_limit_inc != NULL)  lv_label_set_text(objects.label_limit_inc, "+1mV");

    if (objects.label_limit_preset1 != NULL) lv_label_set_text(objects.label_limit_preset1, "-50 mV");
    if (objects.label_limit_preset2 != NULL) lv_label_set_text(objects.label_limit_preset2, "-10 mV");
    if (objects.label_limit_preset3 != NULL) lv_label_set_text(objects.label_limit_preset3, "0 mV");

    if (objects.limit_slider != NULL) {
        lv_slider_set_range(objects.limit_slider, -10000, 5000);
        int slider_val = (int)(g_temp_thr_suvl * 100.0f + (g_temp_thr_suvl >= 0 ? 0.5f : -0.5f));
        lv_slider_set_value(objects.limit_slider, slider_val, LV_ANIM_OFF);
    }
    
    if (objects.lbl_limit_val != NULL) {
        int i_val = (int)g_temp_thr_suvl;
        int d_val = (int)(abs((int)((g_temp_thr_suvl - i_val) * 100.0f + (g_temp_thr_suvl >= 0 ? 0.5f : -0.5f))));
        if (g_temp_thr_suvl < 0 && i_val == 0) {
            lv_label_set_text_fmt(objects.lbl_limit_val, "-%d.%02d", abs(i_val), d_val);
        } else {
            lv_label_set_text_fmt(objects.lbl_limit_val, "%d.%02d", i_val, d_val);
        }
    }
}

static void setup_thr_bovl_screen(void)
{
    setup_limit_screen();
    g_active_limit_mode = MODE_THR_BOVL;

    if (objects.lbl_limit_text != NULL)   lv_label_set_text(objects.lbl_limit_text, "BUS OVERVOLTAGE (BOVL)");
    if (objects.lbl_limit_unit != NULL)   lv_label_set_text(objects.lbl_limit_unit, "V");
    if (objects.lbl_limit_min != NULL)    lv_label_set_text(objects.lbl_limit_min, "1.00 V");
    if (objects.lbl_limit_max != NULL) {
        lv_label_set_text(objects.lbl_limit_max, "60.00 V");
        lv_obj_update_layout(objects.lbl_limit_max); 
        lv_obj_set_x(objects.lbl_limit_max, 255);
    }

    if (objects.label_limit_dec != NULL)  lv_label_set_text(objects.label_limit_dec, "-1.0V");
    if (objects.label_limit_inc != NULL)  lv_label_set_text(objects.label_limit_inc, "+1.0V");

    if (objects.label_limit_preset1 != NULL) lv_label_set_text(objects.label_limit_preset1, "12.0 V");
    if (objects.label_limit_preset2 != NULL) lv_label_set_text(objects.label_limit_preset2, "24.0 V");
    if (objects.label_limit_preset3 != NULL) lv_label_set_text(objects.label_limit_preset3, "52.0 V");

    if (objects.limit_slider != NULL) {
        lv_slider_set_range(objects.limit_slider, 100, 6000);
        int slider_val = (int)(g_temp_thr_bovl * 100.0f + 0.5f);
        lv_slider_set_value(objects.limit_slider, slider_val, LV_ANIM_OFF);
    }
    
    if (objects.lbl_limit_val != NULL) {
        int i_val = (int)g_temp_thr_bovl;
        int d_val = (int)(abs((int)((g_temp_thr_bovl - i_val) * 100.0f + 0.5f)));
        lv_label_set_text_fmt(objects.lbl_limit_val, "%d.%02d", i_val, d_val);
    }
}

static void setup_thr_buvl_screen(void)
{
    setup_limit_screen();
    g_active_limit_mode = MODE_THR_BUVL;

    if (objects.lbl_limit_text != NULL)   lv_label_set_text(objects.lbl_limit_text, "BUS UNDERVOLTAGE (BUVL)");
    if (objects.lbl_limit_unit != NULL)   lv_label_set_text(objects.lbl_limit_unit, "V");
    if (objects.lbl_limit_min != NULL)    lv_label_set_text(objects.lbl_limit_min, "0.00 V");
    if (objects.lbl_limit_max != NULL) {
        lv_label_set_text(objects.lbl_limit_max, "48.00 V");
        lv_obj_update_layout(objects.lbl_limit_max); 
        lv_obj_set_x(objects.lbl_limit_max, 255);
    }

    if (objects.label_limit_dec != NULL)  lv_label_set_text(objects.label_limit_dec, "-0.1V");
    if (objects.label_limit_inc != NULL)  lv_label_set_text(objects.label_limit_inc, "+0.1V");

    if (objects.label_limit_preset1 != NULL) lv_label_set_text(objects.label_limit_preset1, "3.3 V");
    if (objects.label_limit_preset2 != NULL) lv_label_set_text(objects.label_limit_preset2, "4.5 V");
    if (objects.label_limit_preset3 != NULL) lv_label_set_text(objects.label_limit_preset3, "10.8 V");

    if (objects.limit_slider != NULL) {
        lv_slider_set_range(objects.limit_slider, 0, 4800);
        int slider_val = (int)(g_temp_thr_buvl * 100.0f + 0.5f);
        lv_slider_set_value(objects.limit_slider, slider_val, LV_ANIM_OFF);
    }
    
    if (objects.lbl_limit_val != NULL) {
        int i_val = (int)g_temp_thr_buvl;
        int d_val = (int)(abs((int)((g_temp_thr_buvl - i_val) * 100.0f + 0.5f)));
        lv_label_set_text_fmt(objects.lbl_limit_val, "%d.%02d", i_val, d_val);
    }
}

static void setup_thr_temp_screen(void)
{
    setup_limit_screen();
    g_active_limit_mode = MODE_THR_TEMP;

    if (objects.lbl_limit_text != NULL)   lv_label_set_text(objects.lbl_limit_text, "OVER TEMPERATURE (TEMP)");
    if (objects.lbl_limit_unit != NULL)   lv_label_set_text(objects.lbl_limit_unit, "°C");
    if (objects.lbl_limit_min != NULL)    lv_label_set_text(objects.lbl_limit_min, "0.0 °C");
    if (objects.lbl_limit_max != NULL) {
        lv_label_set_text(objects.lbl_limit_max, "125.0 °C");
        lv_obj_update_layout(objects.lbl_limit_max); 
        lv_obj_set_x(objects.lbl_limit_max, 250);
    }

    if (objects.label_limit_dec != NULL)  lv_label_set_text(objects.label_limit_dec, "-5°C");
    if (objects.label_limit_inc != NULL)  lv_label_set_text(objects.label_limit_inc, "+5°C");

    if (objects.label_limit_preset1 != NULL) lv_label_set_text(objects.label_limit_preset1, "60 °C");
    if (objects.label_limit_preset2 != NULL) lv_label_set_text(objects.label_limit_preset2, "85 °C");
    if (objects.label_limit_preset3 != NULL) lv_label_set_text(objects.label_limit_preset3, "105 °C");

    if (objects.limit_slider != NULL) {
        lv_slider_set_range(objects.limit_slider, 0, 1250);
        int slider_val = (int)(g_temp_thr_temp * 10.0f + 0.5f);
        lv_slider_set_value(objects.limit_slider, slider_val, LV_ANIM_OFF);
    }
    
    if (objects.lbl_limit_val != NULL) {
        int i_val = (int)g_temp_thr_temp;
        int d_val = (int)(abs((int)((g_temp_thr_temp - i_val) * 10.0f + 0.5f)));
        lv_label_set_text_fmt(objects.lbl_limit_val, "%d.%d", i_val, d_val);
    }
}

static void setup_thr_pwr_screen(void)
{
    setup_limit_screen();
    g_active_limit_mode = MODE_THR_PWR;

    if (objects.lbl_limit_text != NULL)   lv_label_set_text(objects.lbl_limit_text, "OVER POWER (PWR)");
    if (objects.lbl_limit_unit != NULL)   lv_label_set_text(objects.lbl_limit_unit, "W");
    if (objects.lbl_limit_min != NULL)    lv_label_set_text(objects.lbl_limit_min, "0.0 W");
    if (objects.lbl_limit_max != NULL) {
        lv_label_set_text(objects.lbl_limit_max, "350.0 W");
        lv_obj_update_layout(objects.lbl_limit_max); 
        lv_obj_set_x(objects.lbl_limit_max, 255);
    }

    if (objects.label_limit_dec != NULL)  lv_label_set_text(objects.label_limit_dec, "-5W");
    if (objects.label_limit_inc != NULL)  lv_label_set_text(objects.label_limit_inc, "+5W");

    if (objects.label_limit_preset1 != NULL) lv_label_set_text(objects.label_limit_preset1, "25 W");
    if (objects.label_limit_preset2 != NULL) lv_label_set_text(objects.label_limit_preset2, "65 W");
    if (objects.label_limit_preset3 != NULL) lv_label_set_text(objects.label_limit_preset3, "100 W");

    if (objects.limit_slider != NULL) {
        lv_slider_set_range(objects.limit_slider, 0, 3500);
        int slider_val = (int)(g_temp_thr_pwr * 10.0f + 0.5f);
        lv_slider_set_value(objects.limit_slider, slider_val, LV_ANIM_OFF);
    }
    
    if (objects.lbl_limit_val != NULL) {
        int i_val = (int)g_temp_thr_pwr;
        int d_val = (int)(abs((int)((g_temp_thr_pwr - i_val) * 10.0f + 0.5f)));
        lv_label_set_text_fmt(objects.lbl_limit_val, "%d.%d", i_val, d_val);
    }
}

static void setup_convdelay_limit_screen(void)
{
    setup_limit_screen();
    g_active_limit_mode = MODE_INA_CONVDLY;

    if (objects.lbl_limit_text != NULL)   lv_label_set_text(objects.lbl_limit_text, "CONVERSION DELAY (CONVDLY)");
    if (objects.lbl_limit_unit != NULL)   lv_label_set_text(objects.lbl_limit_unit, "ms");
    if (objects.lbl_limit_min != NULL)    lv_label_set_text(objects.lbl_limit_min, "0 ms");
    if (objects.lbl_limit_max != NULL) {
        lv_label_set_text(objects.lbl_limit_max, "510 ms");
        lv_obj_update_layout(objects.lbl_limit_max); 
        lv_obj_set_x(objects.lbl_limit_max, 255);
    }

    if (objects.label_limit_dec != NULL)  lv_label_set_text(objects.label_limit_dec, "-2 ms");
    if (objects.label_limit_inc != NULL)  lv_label_set_text(objects.label_limit_inc, "+2 ms");

    if (objects.label_limit_preset1 != NULL) lv_label_set_text(objects.label_limit_preset1, "0 ms");
    if (objects.label_limit_preset2 != NULL) lv_label_set_text(objects.label_limit_preset2, "20 ms");
    if (objects.label_limit_preset3 != NULL) lv_label_set_text(objects.label_limit_preset3, "100 ms");

    if (objects.limit_slider != NULL) {
        lv_slider_set_range(objects.limit_slider, 0, 255);
        int slider_val = g_temp_adc_convdelay / 2;
        lv_slider_set_value(objects.limit_slider, slider_val, LV_ANIM_OFF);
    }
    
    if (objects.lbl_limit_val != NULL) {
        lv_label_set_text_fmt(objects.lbl_limit_val, "%d", g_temp_adc_convdelay);
    }
}

static void on_btn_ina228_thr_edit1_clicked(lv_event_t *e)
{
    if (lv_event_get_code(e) == LV_EVENT_CLICKED)
    {
        setup_limit_screen();
        loadScreen(SCREEN_ID_LIMIT_SCREEN);
        setup_thr_sovl_screen();
        if (objects.ina228_limits_screen != NULL) {
            lv_obj_del(objects.ina228_limits_screen);
            objects.ina228_limits_screen = NULL;
        }
    }
}

static void on_btn_ina228_thr_edit2_clicked(lv_event_t *e)
{
    if (lv_event_get_code(e) == LV_EVENT_CLICKED)
    {
        setup_limit_screen();
        loadScreen(SCREEN_ID_LIMIT_SCREEN);
        setup_thr_suvl_screen();
        if (objects.ina228_limits_screen != NULL) {
            lv_obj_del(objects.ina228_limits_screen);
            objects.ina228_limits_screen = NULL;
        }
    }
}

static void on_btn_ina228_thr_edit3_clicked(lv_event_t *e)
{
    if (lv_event_get_code(e) == LV_EVENT_CLICKED)
    {
        setup_limit_screen();
        loadScreen(SCREEN_ID_LIMIT_SCREEN);
        setup_thr_bovl_screen();
        if (objects.ina228_limits_screen != NULL) {
            lv_obj_del(objects.ina228_limits_screen);
            objects.ina228_limits_screen = NULL;
        }
    }
}

static void on_btn_ina228_thr_edit4_clicked(lv_event_t *e)
{
    if (lv_event_get_code(e) == LV_EVENT_CLICKED)
    {
        setup_limit_screen();
        loadScreen(SCREEN_ID_LIMIT_SCREEN);
        setup_thr_buvl_screen();
        if (objects.ina228_limits_screen != NULL) {
            lv_obj_del(objects.ina228_limits_screen);
            objects.ina228_limits_screen = NULL;
        }
    }
}

static void on_btn_ina228_thr_edit5_clicked(lv_event_t *e)
{
    if (lv_event_get_code(e) == LV_EVENT_CLICKED)
    {
        setup_limit_screen();
        loadScreen(SCREEN_ID_LIMIT_SCREEN);
        setup_thr_temp_screen();
        if (objects.ina228_limits_screen != NULL) {
            lv_obj_del(objects.ina228_limits_screen);
            objects.ina228_limits_screen = NULL;
        }
    }
}

static void on_btn_ina228_thr_edit6_clicked(lv_event_t *e)
{
    if (lv_event_get_code(e) == LV_EVENT_CLICKED)
    {
        setup_limit_screen();
        loadScreen(SCREEN_ID_LIMIT_SCREEN);
        setup_thr_pwr_screen();
        if (objects.ina228_limits_screen != NULL) {
            lv_obj_del(objects.ina228_limits_screen);
            objects.ina228_limits_screen = NULL;
        }
    }
}

/* ===================================================================
   CALLBACKS DE NAVEGACIÓN Y EVENTOS TÁCTILES
   =================================================================== */

static void on_btn_menu_select_clicked(lv_event_t *e)
{
    if (lv_event_get_code(e) == LV_EVENT_CLICKED)
    {
        g_selected_menu_index = (g_selected_menu_index + 1) % 5;
        update_menu_selection_highlight();
    }
}

static void on_btn_menu_enter_clicked(lv_event_t *e)
{
    if (lv_event_get_code(e) == LV_EVENT_CLICKED)
    {
        switch (g_selected_menu_index) {
            case 0: /* box_efuse (Límite de Corriente) */
                setup_limit_screen();
                loadScreen(SCREEN_ID_LIMIT_SCREEN);
                setup_current_limit_screen();
                if (objects.menu_screen != NULL) {
                    lv_obj_del(objects.menu_screen);
                    objects.menu_screen = NULL;
                }
                break;
            case 2: /* box_ovp (Límite OVP) */
                setup_limit_screen();
                loadScreen(SCREEN_ID_LIMIT_SCREEN);
                setup_ovp_limit_screen();
                if (objects.menu_screen != NULL) {
                    lv_obj_del(objects.menu_screen);
                    objects.menu_screen = NULL;
                }
                break;
            case 3: /* box_dispaly_bl (Brillo) */
                setup_limit_screen();
                loadScreen(SCREEN_ID_LIMIT_SCREEN);
                setup_brightness_screen();
                if (objects.menu_screen != NULL) {
                    lv_obj_del(objects.menu_screen);
                    objects.menu_screen = NULL;
                }
                break;
            case 4: /* box_ina_config (INA228 Wizard) */
                setup_ina228_adc_screen();
                loadScreen(SCREEN_ID_INA228_ADC_SCREEN);
                if (objects.menu_screen != NULL) {
                    lv_obj_del(objects.menu_screen);
                    objects.menu_screen = NULL;
                }
                break;
            default:
                break;
        }
    }
}

static void on_btn_main_config_clicked(lv_event_t *e) {
    if (lv_event_get_code(e) == LV_EVENT_CLICKED) {
        setup_menu_screen();
        loadScreen(SCREEN_ID_MENU_SCREEN);
        update_menu_screen_ui();
    }
}

static void on_btn_limit_cancel_clicked(lv_event_t *e) 
{
    if (lv_event_get_code(e) == LV_EVENT_CLICKED) 
    {
        if (g_active_limit_mode == MODE_SHUNT_RESISTOR || g_active_limit_mode == MODE_MAX_CURRENT) 
        {
            setup_ina228_cal_screen();
            loadScreen(SCREEN_ID_INA228_CAL_SCREEN);
            if (objects.limit_screen != NULL) {
                lv_obj_del(objects.limit_screen);
                objects.limit_screen = NULL;
            }
            return;
        }

        if (g_active_limit_mode >= MODE_THR_SOVL && g_active_limit_mode <= MODE_THR_PWR) 
        {
            setup_ina228_limits_screen();
            loadScreen(SCREEN_ID_INA228_LIMITS_SCREEN);
            if (objects.limit_screen != NULL) {
                lv_obj_del(objects.limit_screen);
                objects.limit_screen = NULL;
            }
            return;
        }

        if (g_active_limit_mode == MODE_INA_CONVDLY) 
        {
            setup_ina228_adc_screen();
            loadScreen(SCREEN_ID_INA228_ADC_SCREEN);
            if (objects.limit_screen != NULL) {
                lv_obj_del(objects.limit_screen);
                objects.limit_screen = NULL;
            }
            return;
        }

        if (g_active_limit_mode == MODE_BRIGHTNESS) {
            Display_SetBrightness(g_saved_brightness);
        }
        setup_menu_screen();
        loadScreen(SCREEN_ID_MENU_SCREEN);
        update_menu_screen_ui();
        if (objects.limit_screen != NULL) {
            lv_obj_del(objects.limit_screen);
            objects.limit_screen = NULL;
        }
    }
}

static void on_btn_limit_save_clicked(lv_event_t *e) 
{
    if (lv_event_get_code(e) == LV_EVENT_CLICKED) 
    {
        if (g_active_limit_mode == MODE_BRIGHTNESS) 
        {
            g_saved_brightness = g_temp_brightness;
            g_user_settings.brightness = g_saved_brightness;
            Settings_Save();
        } 
        else if (g_active_limit_mode == MODE_CURRENT_LIMIT) 
        {
            g_user_settings.ocp_limit = g_temp_current_limit;
            g_power_sim.ocp_limit = g_user_settings.ocp_limit;
            Settings_Save();
        }
        else if (g_active_limit_mode == MODE_OVP_LIMIT) 
        {
            g_user_settings.ovp_limit = g_temp_ovp_limit;
            g_power_sim.ovp_limit = g_user_settings.ovp_limit;
            Settings_Save();
        }
        else if (g_active_limit_mode == MODE_SHUNT_RESISTOR) 
        {
            g_user_settings.ina228_rshunt = g_temp_shunt_res;
            Settings_Save();
            // Recalibrar sensor con valor dinámico de corriente máxima
            PowerRead_SetCalibration(g_user_settings.ina228_rshunt, g_user_settings.ina228_max_current);
            
            // Regresar a la pantalla de calibración
            setup_ina228_cal_screen();
            loadScreen(SCREEN_ID_INA228_CAL_SCREEN);
            if (objects.limit_screen != NULL) {
                lv_obj_del(objects.limit_screen);
                objects.limit_screen = NULL;
            }
            return; 
        }
        else if (g_active_limit_mode == MODE_MAX_CURRENT) 
        {
            g_user_settings.ina228_max_current = g_temp_max_current;
            Settings_Save();
            // Recalibrar sensor con valor dinámico de shunt y corriente máxima
            PowerRead_SetCalibration(g_user_settings.ina228_rshunt, g_user_settings.ina228_max_current);
            
            // Regresar a la pantalla de calibración
            setup_ina228_cal_screen();
            loadScreen(SCREEN_ID_INA228_CAL_SCREEN);
            if (objects.limit_screen != NULL) {
                lv_obj_del(objects.limit_screen);
                objects.limit_screen = NULL;
            }
            return; 
        }
        else if (g_active_limit_mode >= MODE_THR_SOVL && g_active_limit_mode <= MODE_THR_PWR) 
        {
            // Los valores ya están actualizados en g_temp_thr_* por update_limit_preview
            setup_ina228_limits_screen();
            loadScreen(SCREEN_ID_INA228_LIMITS_SCREEN);
            if (objects.limit_screen != NULL) {
                lv_obj_del(objects.limit_screen);
                objects.limit_screen = NULL;
            }
            return; 
        }
        else if (g_active_limit_mode == MODE_INA_CONVDLY) 
        {
            // g_temp_adc_convdelay ya está actualizado por update_limit_preview
            setup_ina228_adc_screen();
            loadScreen(SCREEN_ID_INA228_ADC_SCREEN);
            if (objects.limit_screen != NULL) {
                lv_obj_del(objects.limit_screen);
                objects.limit_screen = NULL;
            }
            return; 
        }

        setup_menu_screen();
        loadScreen(SCREEN_ID_MENU_SCREEN);
        update_menu_screen_ui();
        if (objects.limit_screen != NULL) 
        {
            lv_obj_del(objects.limit_screen);
            objects.limit_screen = NULL;
        }
    }
}

static void on_btn_graph_clicked(lv_event_t *e) {
    if (lv_event_get_code(e) == LV_EVENT_CLICKED) {
        setup_graph_screen();
        loadScreen(SCREEN_ID_GRAPH_SCREEN);
        UI_Events_UpdateGraph();
    }
}

static void on_btn_graph_back_clicked(lv_event_t *e) {
    if (lv_event_get_code(e) == LV_EVENT_CLICKED) {
        loadScreen(SCREEN_ID_MAIN_SCREEN);
        if (objects.graph_screen != NULL) {
            lv_obj_del(objects.graph_screen);
            objects.graph_screen = NULL;
            s_ser_volt = NULL;
            s_ser_curr = NULL;
            s_v_step_idx = -1;
            s_i_step_idx = -1;
        }
        if (objects.main_screen != NULL) {
            lv_obj_invalidate(objects.main_screen);
        }
    }
}

static void on_btn_graph_pause_clicked(lv_event_t *e) {
    if (lv_event_get_code(e) == LV_EVENT_CLICKED) {
        s_graph_paused = !s_graph_paused;
        if (s_graph_paused) {
            if (objects.btn_label_graph_2 != NULL) {
                lv_label_set_text(objects.btn_label_graph_2, "RUN");
            }
            if (objects.btn_graph_pause != NULL) {
                lv_obj_set_style_bg_color(objects.btn_graph_pause, lv_color_hex(0xA2FF67), LV_PART_MAIN | LV_STATE_DEFAULT);
            }
        } else {
            if (objects.btn_label_graph_2 != NULL) {
                lv_label_set_text(objects.btn_label_graph_2, "PAUSE");
            }
            if (objects.btn_graph_pause != NULL) {
                lv_obj_set_style_bg_color(objects.btn_graph_pause, lv_color_hex(0xFFC241), LV_PART_MAIN | LV_STATE_DEFAULT);
            }
        }
    }
}

static void on_btn_graph_clear_clicked(lv_event_t *e) {
    if (lv_event_get_code(e) == LV_EVENT_CLICKED) {
        if (objects.obj3 != NULL) {
            if (s_ser_volt != NULL) lv_chart_set_all_values(objects.obj3, s_ser_volt, 0);
            if (s_ser_curr != NULL) lv_chart_set_all_values(objects.obj3, s_ser_curr, 0);
            lv_chart_refresh(objects.obj3);
        }
    }
}

void UI_Events_UpdateGraph(void)
{
    if (objects.graph_screen == NULL || lv_scr_act() != objects.graph_screen) {
        return;
    }

    /* 1. Actualizar etiquetas de valores instantáneos */
    if (objects.lbl_graph_volt != NULL) {
        float v = g_power_sim.voltage;
        if (v < 1.0f) {
            float v_mv = v * 1000.0f;
            int vi = (int)v_mv;
            int vd = (int)(abs((int)((v_mv - vi) * 100.0f + 0.5f)));
            lv_label_set_text_fmt(objects.lbl_graph_volt, "V: %d.%02d mV", vi, vd);
        } else {
            int vi = (int)v;
            int vd = (int)(abs((int)((v - vi) * 100.0f + 0.5f)));
            lv_label_set_text_fmt(objects.lbl_graph_volt, "V: %d.%02d V", vi, vd);
        }
    }

    if (objects.lbl_graph_current != NULL) {
        float i = g_power_sim.current;
        if (i < 1.0f) {
            float i_ma = i * 1000.0f;
            int ii = (int)i_ma;
            int id = (int)(abs((int)((i_ma - ii) * 10.0f + 0.5f)));
            lv_label_set_text_fmt(objects.lbl_graph_current, "I: %d.%d mA", ii, id);
        } else {
            int ii = (int)i;
            int id = (int)(abs((int)((i - ii) * 1000.0f + 0.5f)));
            lv_label_set_text_fmt(objects.lbl_graph_current, "I: %d.%03d A", ii, id);
        }
    }

    if (objects.lbl_graph_power != NULL) {
        float p = g_power_sim.power;
        if (p < 1.0f) {
            float p_mw = p * 1000.0f;
            int pi = (int)p_mw;
            int pd = (int)(abs((int)((p_mw - pi) * 10.0f + 0.5f)));
            lv_label_set_text_fmt(objects.lbl_graph_power, "P: %d.%d mW", pi, pd);
        } else {
            int pi = (int)p;
            int pd = (int)(abs((int)((p - pi) * 100.0f + 0.5f)));
            lv_label_set_text_fmt(objects.lbl_graph_power, "P: %d.%02d W", pi, pd);
        }
    }

    /* 2. Si no está en pausa, actualizar auto-rango dinámico y añadir muestra */
    if (!s_graph_paused && objects.obj3 != NULL) {
        update_auto_range(g_power_sim.voltage, g_power_sim.current);

        int32_t v_mv = (int32_t)(g_power_sim.voltage * 1000.0f);
        int32_t i_ma = (int32_t)(g_power_sim.current * 1000.0f);

        if (v_mv < 0) v_mv = 0;
        if (i_ma < 0) i_ma = 0;

        if (s_ser_volt != NULL) lv_chart_set_next_value(objects.obj3, s_ser_volt, v_mv);
        if (s_ser_curr != NULL) lv_chart_set_next_value(objects.obj3, s_ser_curr, i_ma);
    }
}

static void on_btn_main_rst_clicked(lv_event_t *e) {
    if (lv_event_get_code(e) == LV_EVENT_CLICKED) {
        setup_reset_modal();
        loadScreen(SCREEN_ID_RESET_MODAL);
    }
}

static void on_btn_modal_abort_clicked(lv_event_t *e) {
    if (lv_event_get_code(e) == LV_EVENT_CLICKED) {
        loadScreen(SCREEN_ID_MAIN_SCREEN);
        if (objects.reset_modal != NULL) {
            lv_obj_del(objects.reset_modal);
            objects.reset_modal = NULL;
        }
    }
}

static void on_btn_modal_reset_clicked(lv_event_t *e) {
    if (lv_event_get_code(e) == LV_EVENT_CLICKED) {
        PowerSim_ResetStats();         /* 1. Resetea variables locales y tiempo */
        PowerRead_ResetAccumulators();  /* 2. Resetea los registros de 40 bits del sensor físico */
        PowerSim_UpdateUI();            /* 3. Refresca la pantalla a ceros inmediatamente */
        
        loadScreen(SCREEN_ID_MAIN_SCREEN);
        if (objects.reset_modal != NULL) {
            lv_obj_del(objects.reset_modal);
            objects.reset_modal = NULL;
        }
    }
}

static void on_btn_menu_back_clicked(lv_event_t *e) {
    if (lv_event_get_code(e) == LV_EVENT_CLICKED) {
        loadScreen(SCREEN_ID_MAIN_SCREEN);
        if (objects.menu_screen != NULL) {
            lv_obj_del(objects.menu_screen);
            objects.menu_screen = NULL;
        }
    }
}

static void on_btn_menu_edit1_clicked(lv_event_t *e) {
    if (lv_event_get_code(e) == LV_EVENT_CLICKED) {
        g_selected_menu_index = 0; /* Opción 1: eFuse */
        setup_limit_screen();
        loadScreen(SCREEN_ID_LIMIT_SCREEN);
        setup_current_limit_screen();
        if (objects.menu_screen != NULL) {
            lv_obj_del(objects.menu_screen);
            objects.menu_screen = NULL;
        }
    }
}

static void on_btn_menu_edit3_clicked(lv_event_t *e) {
    if (lv_event_get_code(e) == LV_EVENT_CLICKED) {
        g_selected_menu_index = 2; /* Opción 3: OVP */
        setup_limit_screen();
        loadScreen(SCREEN_ID_LIMIT_SCREEN);
        setup_ovp_limit_screen();
        if (objects.menu_screen != NULL) {
            lv_obj_del(objects.menu_screen);
            objects.menu_screen = NULL;
        }
    }
}

static void on_btn_menu_edit4_clicked(lv_event_t *e) {
    if (lv_event_get_code(e) == LV_EVENT_CLICKED) {
        g_selected_menu_index = 3;
        setup_limit_screen();
        loadScreen(SCREEN_ID_LIMIT_SCREEN);
        setup_brightness_screen();
        if (objects.menu_screen != NULL) {
            lv_obj_del(objects.menu_screen);
            objects.menu_screen = NULL;
        }
    }
}

static void on_btn_menu_edit5_clicked(lv_event_t *e) {
    if (lv_event_get_code(e) == LV_EVENT_CLICKED) {
        g_selected_menu_index = 4; /* Opción 5: INA Config */
        setup_ina228_adc_screen();
        loadScreen(SCREEN_ID_INA228_ADC_SCREEN);
        if (objects.menu_screen != NULL) {
            lv_obj_del(objects.menu_screen);
            objects.menu_screen = NULL;
        }
    }
}

/* Eventos de Switch CC */
static void on_btn_menu_edit2_changed(lv_event_t *e) {
    if (lv_event_get_code(e) == LV_EVENT_VALUE_CHANGED) {
        bool is_on = lv_obj_has_state(objects.btn_menu_edit2, LV_STATE_CHECKED);
        if (objects.lbl_cc_state != NULL) {
            lv_label_set_text(objects.lbl_cc_state, is_on ? "ON" : "OFF");
        }
        g_user_settings.cc_enabled = is_on ? 1 : 0;
        Settings_Save();
    }
}

/* Eventos de Slider y Controles Multimodo */
static void on_limit_slider_changed(lv_event_t *e) 
{
    if (lv_event_get_code(e) == LV_EVENT_VALUE_CHANGED) 
    {
        int val = lv_slider_get_value(objects.limit_slider);
        
        if (g_active_limit_mode == MODE_SHUNT_RESISTOR) 
        {
            update_shunt_display((float)val / 2.0f);
        }
        else 
        {
            update_limit_preview(val);
        }
    }
}

static void on_btn_limit_dec_clicked(lv_event_t *e) {
    if (lv_event_get_code(e) == LV_EVENT_CLICKED) 
    {
        if (g_active_limit_mode == MODE_BRIGHTNESS) 
        {
            update_limit_preview(g_temp_brightness - 10);
        } 
        else if (g_active_limit_mode == MODE_CURRENT_LIMIT || g_active_limit_mode == MODE_MAX_CURRENT) 
        {
            float cur = (g_active_limit_mode == MODE_CURRENT_LIMIT) ? g_temp_current_limit : g_temp_max_current;
            int slider_val = (int)(cur * 100.0f) - 50;
            update_limit_preview(slider_val);
        } 
        else if (g_active_limit_mode == MODE_OVP_LIMIT) 
        {
            int slider_val = (int)(g_temp_ovp_limit * 100.0f) - 10;
            update_limit_preview(slider_val);
        }
        else if (g_active_limit_mode == MODE_SHUNT_RESISTOR) 
        {
            float nuevo_val = g_temp_shunt_res - 0.5f;
            if (nuevo_val < 1.0f) nuevo_val = 1.0f;
            update_shunt_display(nuevo_val);
        }
        else if (g_active_limit_mode == MODE_THR_SOVL) 
        {
            update_limit_preview((int)(g_temp_thr_sovl * 100.0f + 0.5f) - 100);
        }
        else if (g_active_limit_mode == MODE_THR_SUVL) 
        {
            update_limit_preview((int)(g_temp_thr_suvl * 100.0f + (g_temp_thr_suvl >= 0 ? 0.5f : -0.5f)) - 100);
        }
        else if (g_active_limit_mode == MODE_THR_BOVL) 
        {
            update_limit_preview((int)(g_temp_thr_bovl * 100.0f + 0.5f) - 100);
        }
        else if (g_active_limit_mode == MODE_THR_BUVL) 
        {
            update_limit_preview((int)(g_temp_thr_buvl * 100.0f + 0.5f) - 10);
        }
        else if (g_active_limit_mode == MODE_THR_TEMP) 
        {
            update_limit_preview((int)(g_temp_thr_temp * 10.0f + 0.5f) - 50);
        }
        else if (g_active_limit_mode == MODE_THR_PWR) 
        {
            update_limit_preview((int)(g_temp_thr_pwr * 10.0f + 0.5f) - 50);
        }
        else if (g_active_limit_mode == MODE_INA_CONVDLY) 
        {
            int slider_val = (g_temp_adc_convdelay / 2) - 1;
            update_limit_preview(slider_val);
        }
    }
}

static void on_btn_limit_inc_clicked(lv_event_t *e) {
    if (lv_event_get_code(e) == LV_EVENT_CLICKED) 
    {
        if (g_active_limit_mode == MODE_BRIGHTNESS) 
        {
            update_limit_preview(g_temp_brightness + 10);
        } 
        else if (g_active_limit_mode == MODE_CURRENT_LIMIT || g_active_limit_mode == MODE_MAX_CURRENT) 
        {
            float cur = (g_active_limit_mode == MODE_CURRENT_LIMIT) ? g_temp_current_limit : g_temp_max_current;
            int slider_val = (int)(cur * 100.0f) + 50;
            update_limit_preview(slider_val);
        } 
        else if (g_active_limit_mode == MODE_OVP_LIMIT) 
        {
            int slider_val = (int)(g_temp_ovp_limit * 100.0f) + 10;
            update_limit_preview(slider_val);
        }
        else if (g_active_limit_mode == MODE_SHUNT_RESISTOR) 
        {
            float nuevo_val = g_temp_shunt_res + 0.5f;
            if (nuevo_val > 100.0f) nuevo_val = 100.0f;
            update_shunt_display(nuevo_val);
        }
        else if (g_active_limit_mode == MODE_THR_SOVL) 
        {
            update_limit_preview((int)(g_temp_thr_sovl * 100.0f + 0.5f) + 100);
        }
        else if (g_active_limit_mode == MODE_THR_SUVL) 
        {
            update_limit_preview((int)(g_temp_thr_suvl * 100.0f + (g_temp_thr_suvl >= 0 ? 0.5f : -0.5f)) + 100);
        }
        else if (g_active_limit_mode == MODE_THR_BOVL) 
        {
            update_limit_preview((int)(g_temp_thr_bovl * 100.0f + 0.5f) + 100);
        }
        else if (g_active_limit_mode == MODE_THR_BUVL) 
        {
            update_limit_preview((int)(g_temp_thr_buvl * 100.0f + 0.5f) + 10);
        }
        else if (g_active_limit_mode == MODE_THR_TEMP) 
        {
            update_limit_preview((int)(g_temp_thr_temp * 10.0f + 0.5f) + 50);
        }
        else if (g_active_limit_mode == MODE_THR_PWR) 
        {
            update_limit_preview((int)(g_temp_thr_pwr * 10.0f + 0.5f) + 50);
        }
        else if (g_active_limit_mode == MODE_INA_CONVDLY) 
        {
            int slider_val = (g_temp_adc_convdelay / 2) + 1;
            update_limit_preview(slider_val);
        }
    }
}

static void on_btn_limit_preset1_clicked(lv_event_t *e) 
{
    if (lv_event_get_code(e) == LV_EVENT_CLICKED) 
    {
        if (g_active_limit_mode == MODE_BRIGHTNESS) 
        {
            update_limit_preview(10);
        } 
        else if (g_active_limit_mode == MODE_CURRENT_LIMIT || g_active_limit_mode == MODE_MAX_CURRENT) 
        {
            update_limit_preview(100);
        } 
        else if (g_active_limit_mode == MODE_OVP_LIMIT) 
        {
            update_limit_preview(550);
        }
        else if (g_active_limit_mode == MODE_SHUNT_RESISTOR)
        {
            update_shunt_display(5.0f);
        } 
        else if (g_active_limit_mode == MODE_THR_SOVL)
        {
            update_limit_preview(2500);
        }
        else if (g_active_limit_mode == MODE_THR_SUVL)
        {
            update_limit_preview(-5000);
        }
        else if (g_active_limit_mode == MODE_THR_BOVL)
        {
            update_limit_preview(1200);
        }
        else if (g_active_limit_mode == MODE_THR_BUVL)
        {
            update_limit_preview(330);
        }
        else if (g_active_limit_mode == MODE_THR_TEMP)
        {
            update_limit_preview(600);
        }
        else if (g_active_limit_mode == MODE_THR_PWR)
        {
            update_limit_preview(250);
        }
        else if (g_active_limit_mode == MODE_INA_CONVDLY)
        {
            update_limit_preview(0); /* 0 ms */
        }
    }
}

static void on_btn_limit_preset2_clicked(lv_event_t *e) {
    if (lv_event_get_code(e) == LV_EVENT_CLICKED) 
    {
        if (g_active_limit_mode == MODE_BRIGHTNESS) 
        {
            update_limit_preview(50);
        } else if (g_active_limit_mode == MODE_CURRENT_LIMIT || g_active_limit_mode == MODE_MAX_CURRENT) 
        {
            update_limit_preview(300);
        } else if (g_active_limit_mode == MODE_OVP_LIMIT) 
        {
            update_limit_preview(1200);
        }
        else if (g_active_limit_mode == MODE_SHUNT_RESISTOR)
        {
            update_shunt_display(15.0f);
        } 
        else if (g_active_limit_mode == MODE_THR_SOVL)
        {
            update_limit_preview(5000);
        }
        else if (g_active_limit_mode == MODE_THR_SUVL)
        {
            update_limit_preview(-1000);
        }
        else if (g_active_limit_mode == MODE_THR_BOVL)
        {
            update_limit_preview(2400);
        }
        else if (g_active_limit_mode == MODE_THR_BUVL)
        {
            update_limit_preview(450);
        }
        else if (g_active_limit_mode == MODE_THR_TEMP)
        {
            update_limit_preview(850);
        }
        else if (g_active_limit_mode == MODE_THR_PWR)
        {
            update_limit_preview(650);
        }
        else if (g_active_limit_mode == MODE_INA_CONVDLY)
        {
            update_limit_preview(10); /* 20 ms */
        }
    }
}

static void on_btn_limit_preset3_clicked(lv_event_t *e) 
{
    if (lv_event_get_code(e) == LV_EVENT_CLICKED) 
    {
        if (g_active_limit_mode == MODE_BRIGHTNESS) 
        {
            update_limit_preview(100);
        } 
        else if (g_active_limit_mode == MODE_CURRENT_LIMIT || g_active_limit_mode == MODE_MAX_CURRENT) 
        {
            update_limit_preview(500);
        } 
        else if (g_active_limit_mode == MODE_OVP_LIMIT) 
        {
            update_limit_preview(2400);
        }
        else if (g_active_limit_mode == MODE_SHUNT_RESISTOR)
        {
            update_shunt_display(50.0f);
        }
        else if (g_active_limit_mode == MODE_THR_SOVL)
        {
            update_limit_preview(10000);
        }
        else if (g_active_limit_mode == MODE_THR_SUVL)
        {
            update_limit_preview(0);
        }
        else if (g_active_limit_mode == MODE_THR_BOVL)
        {
            update_limit_preview(5200);
        }
        else if (g_active_limit_mode == MODE_THR_BUVL)
        {
            update_limit_preview(1080);
        }
        else if (g_active_limit_mode == MODE_THR_TEMP)
        {
            update_limit_preview(1050);
        }
        else if (g_active_limit_mode == MODE_THR_PWR)
        {
            update_limit_preview(1000);
        }
        else if (g_active_limit_mode == MODE_INA_CONVDLY)
        {
            update_limit_preview(50); /* 100 ms */
        }
    }
}

static void on_box_clicked(lv_event_t *e)
{
    if (lv_event_get_code(e) == LV_EVENT_CLICKED)
    {
        lv_obj_t *target = lv_event_get_target(e);
        if (target == objects.box_efuse) g_selected_menu_index = 0;
        else if (target == objects.box_cc) g_selected_menu_index = 1;
        else if (target == objects.box_ovp) g_selected_menu_index = 2;
        else if (target == objects.box_dispaly_bl) g_selected_menu_index = 3;
        else if (target == objects.box_ina_config) g_selected_menu_index = 4;
        update_menu_selection_highlight();
    }
}

/* NAVEGACIÓN Y LIBERACIÓN DINÁMICA DEL WIZARD INA228 */

static void on_btn_adc_cancel_clicked(lv_event_t *e) 
{
    if (lv_event_get_code(e) == LV_EVENT_CLICKED) 
    {
        s_adc_settings_loaded   = false;
        s_alert_settings_loaded = false;
        s_thr_settings_loaded   = false;
        setup_menu_screen();
        loadScreen(SCREEN_ID_MENU_SCREEN);
        if (objects.ina228_adc_screen != NULL) {
            lv_obj_del(objects.ina228_adc_screen);
            objects.ina228_adc_screen = NULL;
        }
    }
}

static void on_btn_ina228_cal_edit1_clicked(lv_event_t *e)
{
    if (lv_event_get_code(e) == LV_EVENT_CLICKED)
    {
        setup_limit_screen();
        loadScreen(SCREEN_ID_LIMIT_SCREEN);
        setup_shunt_limit_screen();
        if (objects.ina228_cal_screen != NULL) {
            lv_obj_del(objects.ina228_cal_screen);
            objects.ina228_cal_screen = NULL;
        }
    }
}

static void on_btn_ina228_cal_edit2_clicked(lv_event_t *e)
{
    if (lv_event_get_code(e) == LV_EVENT_CLICKED)
    {
        setup_limit_screen();
        loadScreen(SCREEN_ID_LIMIT_SCREEN);
        setup_max_current_limit_screen();
        if (objects.ina228_cal_screen != NULL) {
            lv_obj_del(objects.ina228_cal_screen);
            objects.ina228_cal_screen = NULL;
        }
    }
}

static void on_btn_adc_save_clicked(lv_event_t *e) 
{
    if (lv_event_get_code(e) == LV_EVENT_CLICKED) 
    {
        uint8_t  readback_range   = 99; 
        uint8_t  readback_mode    = 99;
        uint8_t  readback_sample  = 99;
        uint16_t readback_delay   = 999;
        uint8_t  readback_vbus_ct = 99;
        uint8_t  readback_vsh_ct  = 99;
        uint8_t  readback_temp_ct = 99;

        // 1. Guardar en memoria Flash para que persista tras reinicios
        g_user_settings.ina228_adc_range   = g_temp_adc_range;
        g_user_settings.ina228_mode        = g_temp_adc_mode;
        g_user_settings.ina228_samples     = g_temp_adc_sample;
        g_user_settings.ina228_conv_delay  = g_temp_adc_convdelay;
        g_user_settings.ina228_vbus_ct     = g_temp_adc_vbus_ct;
        g_user_settings.ina228_vsh_ct      = g_temp_adc_vshunt_ct;
        g_user_settings.ina228_temp_ct     = g_temp_adc_temp_ct;

        Settings_Save();

        // 2. Aplicar la configuración al INA228 físico
        PowerRead_SetAdcRange(g_user_settings.ina228_adc_range);
        PowerRead_SetMode(g_user_settings.ina228_mode);
        PowerRead_SetAverage(g_user_settings.ina228_samples);
        PowerRead_SetConversionDelay(g_user_settings.ina228_conv_delay);
        PowerRead_SetBusConvTime(g_user_settings.ina228_vbus_ct);
        PowerRead_SetShuntConvTime(g_user_settings.ina228_vsh_ct);
        PowerRead_SetTempConvTime(g_user_settings.ina228_temp_ct);

        // 3. Volver al menú principal y destruir la pantalla
        s_adc_settings_loaded   = false;
        s_alert_settings_loaded = false;
        s_thr_settings_loaded   = false;
        setup_menu_screen();
        loadScreen(SCREEN_ID_MENU_SCREEN);
        update_menu_screen_ui();
        if (objects.ina228_adc_screen != NULL) 
        {
            lv_obj_del(objects.ina228_adc_screen);
            objects.ina228_adc_screen = NULL;
        }

        // 4. Lectura de verificación (Readback) directamente del hardware
        HAL_StatusTypeDef st_range = PowerRead_GetAdcRange(&readback_range);
        HAL_StatusTypeDef st_mode  = PowerRead_GetMode(&readback_mode);
        HAL_StatusTypeDef st_avg   = PowerRead_GetAverage(&readback_sample);
        HAL_StatusTypeDef st_delay = PowerRead_GetConversionDelay(&readback_delay);
        HAL_StatusTypeDef st_vbus  = PowerRead_GetBusConvTime(&readback_vbus_ct);
        HAL_StatusTypeDef st_vsh   = PowerRead_GetShuntConvTime(&readback_vsh_ct);
        HAL_StatusTypeDef st_temp  = PowerRead_GetTempConvTime(&readback_temp_ct);

        if (st_range == HAL_OK && st_mode == HAL_OK && st_avg == HAL_OK &&
            st_delay == HAL_OK && st_vbus == HAL_OK && st_vsh == HAL_OK && st_temp == HAL_OK)
        {
            Console_Printf("\r\n--- INA228 ADC CONFIG (Readback Hardware) ---\r\n");
            Console_Printf("  * 1. ADC Range   : %s (%d)\r\n", (readback_range == 1) ? "+-40.96 mV" : "+-163.84 mV", readback_range);
            Console_Printf("  * 2. Mode        : 0x%02X (%s)\r\n", readback_mode, get_mode_text(readback_mode));
            Console_Printf("  * 3. Average     : 0x%02X (%s)\r\n", readback_sample, get_sample_text(readback_sample));
            Console_Printf("  * 4. Conv Delay  : %d ms\r\n", readback_delay);
            Console_Printf("  * 5. VBUS Time   : %s (idx: %d)\r\n", get_conv_time_text(readback_vbus_ct), readback_vbus_ct);
            Console_Printf("  * 6. VSHUNT Time : %s (idx: %d)\r\n", get_conv_time_text(readback_vsh_ct), readback_vsh_ct);
            Console_Printf("  * 7. TEMP Time   : %s (idx: %d)\r\n", get_conv_time_text(readback_temp_ct), readback_temp_ct);
            Console_Printf("----------------------------------------------\r\n");
        }
        else
        {
            Console_Printf("[INA228 ADC] Guardado en Flash, pero fallo la lectura I2C del sensor.\r\n");
        }
    }
}

static void on_btn_adc_next_clicked(lv_event_t *e) 
{
    if (lv_event_get_code(e) == LV_EVENT_CLICKED) 
    {
        lv_obj_t *old_scr = objects.ina228_adc_screen;
        setup_ina228_cal_screen();
        loadScreen(SCREEN_ID_INA228_CAL_SCREEN);
        if (old_scr != NULL) {
            lv_obj_del(old_scr);
            objects.ina228_adc_screen = NULL;
        }
    }
}

static void on_btn_cal_prev_clicked(lv_event_t *e) 
{
    if (lv_event_get_code(e) == LV_EVENT_CLICKED) 
    {
        lv_obj_t *old_scr = objects.ina228_cal_screen;
        setup_ina228_adc_screen();
        loadScreen(SCREEN_ID_INA228_ADC_SCREEN);
        if (old_scr != NULL) {
            lv_obj_del(old_scr);
            objects.ina228_cal_screen = NULL;
        }
    }
}

static void on_btn_cal_save_clicked(lv_event_t *e) 
{
    if (lv_event_get_code(e) == LV_EVENT_CLICKED) 
    {
        s_alert_settings_loaded = false;
        s_thr_settings_loaded   = false;
        setup_menu_screen();
        loadScreen(SCREEN_ID_MENU_SCREEN);
        if (objects.ina228_cal_screen != NULL) {
            lv_obj_del(objects.ina228_cal_screen);
            objects.ina228_cal_screen = NULL;
        }
    }
}

static void on_btn_cal_next_clicked(lv_event_t *e) 
{
    if (lv_event_get_code(e) == LV_EVENT_CLICKED) 
    {
        lv_obj_t *old_scr = objects.ina228_cal_screen;
        setup_ina228_alert_screen();
        loadScreen(SCREEN_ID_INA228_ALERT_SCREEN);
        if (old_scr != NULL) {
            lv_obj_del(old_scr);
            objects.ina228_cal_screen = NULL;
        }
    }
}

static void on_btn_alert_prev_clicked(lv_event_t *e) 
{
    if (lv_event_get_code(e) == LV_EVENT_CLICKED) 
    {
        lv_obj_t *old_scr = objects.ina228_alert_screen;
        setup_ina228_cal_screen();
        loadScreen(SCREEN_ID_INA228_CAL_SCREEN);
        if (old_scr != NULL) {
            lv_obj_del(old_scr);
            objects.ina228_alert_screen = NULL;
        }
    }
}

static void on_switch_alert1_changed(lv_event_t *e)
{
    if (lv_event_get_code(e) == LV_EVENT_VALUE_CHANGED)
    {
        bool is_checked = lv_obj_has_state(objects.switch_alert1, LV_STATE_CHECKED);
        g_temp_alert_latch = is_checked ? 1 : 0;
        if (objects.lbl_alert_val1 != NULL)
        {
            lv_label_set_text(objects.lbl_alert_val1, is_checked ? "TRANSPARENT" : "LATCHED");
        }
    }
}

static void on_switch_alert2_changed(lv_event_t *e)
{
    if (lv_event_get_code(e) == LV_EVENT_VALUE_CHANGED)
    {
        bool is_checked = lv_obj_has_state(objects.switch_alert2, LV_STATE_CHECKED);
        g_temp_alert_cnvr = is_checked ? 1 : 0;
        if (objects.lbl_alert_val2 != NULL)
        {
            lv_label_set_text(objects.lbl_alert_val2, is_checked ? "ON" : "OFF");
        }
    }
}

static void on_switch_alert3_changed(lv_event_t *e)
{
    if (lv_event_get_code(e) == LV_EVENT_VALUE_CHANGED)
    {
        bool is_checked = lv_obj_has_state(objects.switch_alert3, LV_STATE_CHECKED);
        g_temp_alert_pol = is_checked ? 1 : 0;
        if (objects.lbl_alert_val3 != NULL)
        {
            lv_label_set_text(objects.lbl_alert_val3, is_checked ? "ACTIVE HIGH" : "ACTIVE LOW");
        }
    }
}

static void on_switch_alert4_changed(lv_event_t *e)
{
    if (lv_event_get_code(e) == LV_EVENT_VALUE_CHANGED)
    {
        bool is_checked = lv_obj_has_state(objects.switch_alert4, LV_STATE_CHECKED);
        g_temp_alert_filter = is_checked ? 1 : 0;
        if (objects.lbl_alert_val4 != NULL)
        {
            lv_label_set_text(objects.lbl_alert_val4, is_checked ? "AVERAGED" : "NON AVERAGED");
        }
    }
}

static void on_btn_alert_save_clicked(lv_event_t *e) 
{
    if (lv_event_get_code(e) == LV_EVENT_CLICKED) 
    {
        /* 1. Guardar en memoria Flash para que persista tras reinicios */
        g_user_settings.ina228_alert_latch  = g_temp_alert_latch;
        g_user_settings.ina228_alert_cnvr   = g_temp_alert_cnvr;
        g_user_settings.ina228_alert_pol    = g_temp_alert_pol;
        g_user_settings.ina228_alert_filter = g_temp_alert_filter;
        Settings_Save();

        /* 2. Aplicar la configuración al sensor INA228 físico */
        PowerRead_SetAlertLatch(g_user_settings.ina228_alert_latch);
        PowerRead_SetAlertPin(g_user_settings.ina228_alert_cnvr);
        PowerRead_SetAlertPinPolarity(g_user_settings.ina228_alert_pol);
        PowerRead_SetSlowAlert(g_user_settings.ina228_alert_filter);

        s_alert_settings_loaded = false;
        s_thr_settings_loaded   = false;

        /* 3. Lectura de verificación directamente desde el hardware INA228 */
        uint8_t rb_latch = 99, rb_cnvr = 99, rb_pol = 99, rb_filter = 99;
        uint16_t rb_diag = 0;

        HAL_StatusTypeDef st_latch  = PowerRead_GetAlertLatch(&rb_latch);
        HAL_StatusTypeDef st_cnvr   = PowerRead_GetAlertPin(&rb_cnvr);
        HAL_StatusTypeDef st_pol    = PowerRead_GetAlertPinPolarity(&rb_pol);
        HAL_StatusTypeDef st_filter = PowerRead_GetSlowAlert(&rb_filter);
        HAL_StatusTypeDef st_diag   = PowerRead_GetDiagAlert(&rb_diag);

        if (st_latch == HAL_OK && st_cnvr == HAL_OK && st_pol == HAL_OK && st_filter == HAL_OK) 
        {
            Console_Printf("\r\n--- INA228 ALERT CONFIG (Readback Hardware) ---\r\n");
            Console_Printf("  * ALATCH  : %s\r\n", (rb_latch == 1) ? "TRANSPARENT" : "LATCHED");
            Console_Printf("  * CNVR PIN: %s\r\n", (rb_cnvr == 1) ? "ON" : "OFF");
            Console_Printf("  * POLARITY: %s\r\n", (rb_pol == 1) ? "ACTIVE HIGH" : "ACTIVE LOW");
            Console_Printf("  * FILTER  : %s\r\n", (rb_filter == 1) ? "AVERAGED" : "NON AVERAGED");
            if (st_diag == HAL_OK) 
            {
                Console_Printf("  * DIAG_ALRT Reg (0x0B): 0x%04X\r\n", rb_diag);
            }
            Console_Printf("-----------------------------------------------\r\n");
        }
        else
        {
            Console_Printf("[INA228 Alert] Guardado en Flash, pero fallo la lectura I2C del sensor.\r\n");
        }

        setup_menu_screen();
        loadScreen(SCREEN_ID_MENU_SCREEN);
        update_menu_screen_ui();
        if (objects.ina228_alert_screen != NULL) {
            lv_obj_del(objects.ina228_alert_screen);
            objects.ina228_alert_screen = NULL;
        }
    }
}

static void on_btn_alert_next_clicked(lv_event_t *e) 
{
    if (lv_event_get_code(e) == LV_EVENT_CLICKED) 
    {
        lv_obj_t *old_scr = objects.ina228_alert_screen;
        setup_ina228_limits_screen();
        loadScreen(SCREEN_ID_INA228_LIMITS_SCREEN);
        if (old_scr != NULL) {
            lv_obj_del(old_scr);
            objects.ina228_alert_screen = NULL;
        }
    }
}

static void on_btn_thr_prev_clicked(lv_event_t *e) 
{
    if (lv_event_get_code(e) == LV_EVENT_CLICKED) 
    {
        lv_obj_t *old_scr = objects.ina228_limits_screen;
        setup_ina228_alert_screen();
        loadScreen(SCREEN_ID_INA228_ALERT_SCREEN);
        if (old_scr != NULL) {
            lv_obj_del(old_scr);
            objects.ina228_limits_screen = NULL;
        }
    }
}

static void on_btn_thr_save_clicked(lv_event_t *e) 
{
    if (lv_event_get_code(e) == LV_EVENT_CLICKED) 
    {
        /* 1. Guardar en memoria Flash para que persista tras reinicios */
        g_user_settings.ina228_thr_sovl = g_temp_thr_sovl;
        g_user_settings.ina228_thr_suvl = g_temp_thr_suvl;
        g_user_settings.ina228_thr_bovl = g_temp_thr_bovl;
        g_user_settings.ina228_thr_buvl = g_temp_thr_buvl;
        g_user_settings.ina228_thr_temp = g_temp_thr_temp;
        g_user_settings.ina228_thr_pwr  = g_temp_thr_pwr;
        Settings_Save();

        /* 2. Aplicar los 6 umbrales directamente al sensor INA228 físico */
        PowerRead_SetShuntOverVoltage(g_user_settings.ina228_thr_sovl);
        PowerRead_SetShuntUnderVoltage(g_user_settings.ina228_thr_suvl);
        PowerRead_SetBusOverVoltage(g_user_settings.ina228_thr_bovl);
        PowerRead_SetBusUnderVoltage(g_user_settings.ina228_thr_buvl);
        PowerRead_SetTempLimit(g_user_settings.ina228_thr_temp);
        PowerRead_SetPowerLimit(g_user_settings.ina228_thr_pwr);

        s_thr_settings_loaded   = false;
        s_alert_settings_loaded = false;

        /* 3. Lectura de verificación directamente desde el hardware INA228 */
        float rb_sovl = 0.0f, rb_suvl = 0.0f, rb_bovl = 0.0f, rb_buvl = 0.0f, rb_temp = 0.0f, rb_pwr = 0.0f;
        uint16_t rb_diag = 0;

        HAL_StatusTypeDef st_sovl = PowerRead_GetShuntOverVoltage(&rb_sovl);
        HAL_StatusTypeDef st_suvl = PowerRead_GetShuntUnderVoltage(&rb_suvl);
        HAL_StatusTypeDef st_bovl = PowerRead_GetBusOverVoltage(&rb_bovl);
        HAL_StatusTypeDef st_buvl = PowerRead_GetBusUnderVoltage(&rb_buvl);
        HAL_StatusTypeDef st_temp = PowerRead_GetTempLimit(&rb_temp);
        HAL_StatusTypeDef st_pwr  = PowerRead_GetPowerLimit(&rb_pwr);
        HAL_StatusTypeDef st_diag = PowerRead_GetDiagAlert(&rb_diag);

        Console_Printf("\r\n--- INA228 THRESHOLDS (Readback Hardware) ---\r\n");
        if (st_sovl == HAL_OK) {
            int i_val = (int)rb_sovl;
            int d_val = (int)(abs((int)((rb_sovl - i_val) * 100.0f + 0.5f)));
            Console_Printf("  * 1. SOVL (Shunt Over) : %d.%02d mV\r\n", i_val, d_val);
        } else {
            Console_Printf("  * 1. SOVL : ERROR I2C\r\n");
        }

        if (st_suvl == HAL_OK) {
            int i_val = (int)rb_suvl;
            int d_val = (int)(abs((int)((rb_suvl - i_val) * 100.0f + (rb_suvl >= 0 ? 0.5f : -0.5f))));
            if (rb_suvl < 0 && i_val == 0) {
                Console_Printf("  * 2. SUVL (Shunt Under): -%d.%02d mV\r\n", abs(i_val), d_val);
            } else {
                Console_Printf("  * 2. SUVL (Shunt Under): %d.%02d mV\r\n", i_val, d_val);
            }
        } else {
            Console_Printf("  * 2. SUVL : ERROR I2C\r\n");
        }

        if (st_bovl == HAL_OK) {
            int i_val = (int)rb_bovl;
            int d_val = (int)(abs((int)((rb_bovl - i_val) * 100.0f + 0.5f)));
            Console_Printf("  * 3. BOVL (Bus Over)   : %d.%02d V\r\n", i_val, d_val);
        } else {
            Console_Printf("  * 3. BOVL : ERROR I2C\r\n");
        }

        if (st_buvl == HAL_OK) {
            int i_val = (int)rb_buvl;
            int d_val = (int)(abs((int)((rb_buvl - i_val) * 100.0f + 0.5f)));
            Console_Printf("  * 4. BUVL (Bus Under)  : %d.%02d V\r\n", i_val, d_val);
        } else {
            Console_Printf("  * 4. BUVL : ERROR I2C\r\n");
        }

        if (st_temp == HAL_OK) {
            int i_val = (int)rb_temp;
            int d_val = (int)(abs((int)((rb_temp - i_val) * 10.0f + 0.5f)));
            Console_Printf("  * 5. TEMP (Over Temp)  : %d.%d degC\r\n", i_val, d_val);
        } else {
            Console_Printf("  * 5. TEMP : ERROR I2C\r\n");
        }

        if (st_pwr == HAL_OK) {
            int i_val = (int)rb_pwr;
            int d_val = (int)(abs((int)((rb_pwr - i_val) * 10.0f + 0.5f)));
            Console_Printf("  * 6. PWR  (Over Power) : %d.%d W\r\n", i_val, d_val);
        } else {
            Console_Printf("  * 6. PWR  : ERROR I2C\r\n");
        }

        if (st_diag == HAL_OK) {
            Console_Printf("  * DIAG_ALRT Reg (0x0B) : 0x%04X\r\n", rb_diag);
        }
        Console_Printf("---------------------------------------------\r\n");

        setup_menu_screen();
        loadScreen(SCREEN_ID_MENU_SCREEN);
        update_menu_screen_ui();
        if (objects.ina228_limits_screen != NULL) {
            lv_obj_del(objects.ina228_limits_screen);
            objects.ina228_limits_screen = NULL;
        }
    }
}

/* Callback cuando se presiona el botón Edit (btn_ina228_adc_edit1) */
static void on_btn_ina228_adc_edit1_clicked(lv_event_t *e) 
{
    if (lv_event_get_code(e) == LV_EVENT_CLICKED) 
    {
        // Alternar entre 0 (+- 163.84 mV) y 1 (+-40.96 mV)
        g_temp_adc_range = (g_temp_adc_range == 0) ? 1 : 0;
        if (objects.lbl_val_adc_range != NULL) {
            if (g_temp_adc_range == 1) 
            {
                lv_label_set_text(objects.lbl_val_adc_range, "+-40.96 mV");
            } 
            else 
            {
                lv_label_set_text(objects.lbl_val_adc_range, "+- 163.84 mV");
            }
        }
    }
}

static void on_btn_ina228_adc_edit2_clicked(lv_event_t *e)
{
    if (lv_event_get_code(e) == LV_EVENT_CLICKED)
    {
        int current_idx = 0;
        for (int i = 0; i < ADC_MODE_COUNT; i++) {
            if (s_adc_modes[i].mode_val == g_temp_adc_mode) 
            {
                current_idx = i;
                break;
            }
        }

        // Rotar al siguiente modo cíclicamente
        current_idx = (current_idx + 1) % ADC_MODE_COUNT;
        g_temp_adc_mode = s_adc_modes[current_idx].mode_val;

        if (objects.lbl_val_adc_mode != NULL) 
        {
            lv_label_set_text(objects.lbl_val_adc_mode, s_adc_modes[current_idx].text);
        }
    }
}

static void on_btn_ina228_adc_edit3_clicked(lv_event_t *e)
{
    if (lv_event_get_code(e) == LV_EVENT_CLICKED)
    {
        int current_idx = 0;
        for (int i = 0; i < ADC_SAMPLES_COUNT; i++) 
        {
            if (s_adc_samples[i].sample_val == g_temp_adc_sample) 
            {
                current_idx = i;
                break;
            }
        }

        // Rotar al siguiente modo cíclicamente
        current_idx = (current_idx + 1) % ADC_SAMPLES_COUNT;
        g_temp_adc_sample = s_adc_samples[current_idx].sample_val;

        if (objects.lbl_val_adc_samples != NULL) 
        {
            lv_label_set_text(objects.lbl_val_adc_samples, s_adc_samples[current_idx].text);
        }
    }
}

static void on_btn_ina228_adc_edit4_clicked(lv_event_t *e)
{
    if (lv_event_get_code(e) == LV_EVENT_CLICKED)
    {
        setup_limit_screen();
        loadScreen(SCREEN_ID_LIMIT_SCREEN);
        setup_convdelay_limit_screen();
        if (objects.ina228_adc_screen != NULL) {
            lv_obj_del(objects.ina228_adc_screen);
            objects.ina228_adc_screen = NULL;
        }
    }
}

static void on_btn_ina228_adc_edit5_clicked(lv_event_t *e)
{
    if (lv_event_get_code(e) == LV_EVENT_CLICKED)
    {
        g_temp_adc_vbus_ct = (g_temp_adc_vbus_ct + 1) % CONV_TIME_COUNT;
        if (objects.lbl_val_adc_vbus_time != NULL)
        {
            lv_label_set_text(objects.lbl_val_adc_vbus_time, get_conv_time_text(g_temp_adc_vbus_ct));
        }
    }
}

static void on_btn_ina228_adc_edit6_clicked(lv_event_t *e)
{
    if (lv_event_get_code(e) == LV_EVENT_CLICKED)
    {
        g_temp_adc_vshunt_ct = (g_temp_adc_vshunt_ct + 1) % CONV_TIME_COUNT;
        if (objects.lbl_val_adc_vshunt_time != NULL)
        {
            lv_label_set_text(objects.lbl_val_adc_vshunt_time, get_conv_time_text(g_temp_adc_vshunt_ct));
        }
    }
}

static void on_btn_ina228_adc_edit7_clicked(lv_event_t *e)
{
    if (lv_event_get_code(e) == LV_EVENT_CLICKED)
    {
        g_temp_adc_temp_ct = (g_temp_adc_temp_ct + 1) % CONV_TIME_COUNT;
        if (objects.lbl_val_adc_temp_time != NULL)
        {
            lv_label_set_text(objects.lbl_val_adc_temp_time, get_conv_time_text(g_temp_adc_temp_ct));
        }
    }
}

/* Vinculación General de Eventos para la pantalla principal (main_screen) */
void UI_Events_Init(void)
{
    g_saved_brightness = g_user_settings.brightness;
    g_temp_brightness  = g_saved_brightness;

    if (objects.btn_main_config != NULL) 
    {
        lv_obj_add_event_cb(objects.btn_main_config, on_btn_main_config_clicked, LV_EVENT_CLICKED, NULL);
    }

    if (objects.btn_main_graph != NULL) 
    {
        lv_obj_add_event_cb(objects.btn_main_graph, on_btn_graph_clicked, LV_EVENT_CLICKED, NULL);
    }

    if (objects.btn_main_rst != NULL) 
    {
        lv_obj_add_event_cb(objects.btn_main_rst, on_btn_main_rst_clicked, LV_EVENT_CLICKED, NULL);
    }
}
