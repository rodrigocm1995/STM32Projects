#include "ui_events.h"
#include "screens.h"
#include "ui.h"
#include "lvgl.h"
#include "power_sim.h"
#include "Display_Driver.h"
#include "settings_mgr.h"
#include <stdlib.h>

typedef enum {
    MODE_BRIGHTNESS,
    MODE_CURRENT_LIMIT,
    MODE_OVP_LIMIT
} LimitMode_t;

static LimitMode_t g_active_limit_mode = MODE_BRIGHTNESS;
static float g_temp_current_limit = 5.0f;
static float g_temp_ovp_limit     = 50.0f;

/* Variables de estado de Brillo */
static uint8_t g_saved_brightness = 100;
static uint8_t g_temp_brightness  = 100;

/* Índice de opción seleccionada en menu_screen (0: efuse, 1: cc, 2: ovp, 3: backlight, 4: ina) */
static int g_selected_menu_index = 0;

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
            
            /* Forzar el color en TODOS los estados táctiles para evitar que queden encendidos al tocar */
            lv_obj_set_style_bg_color(boxes[i], color, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_bg_color(boxes[i], color, LV_PART_MAIN | LV_STATE_FOCUSED);
            lv_obj_set_style_bg_color(boxes[i], color, LV_PART_MAIN | LV_STATE_HOVERED);
            lv_obj_set_style_bg_color(boxes[i], color, LV_PART_MAIN | LV_STATE_PRESSED);
            
            /* Limpiar estados retenidos */
            lv_obj_remove_state(boxes[i], LV_STATE_FOCUSED | LV_STATE_HOVERED | LV_STATE_PRESSED);
        }
    }
}

/* Actualizar etiquetas en la pantalla del menú */
static void update_menu_screen_ui(void)
{
    /* 1. Brillo */
    if (objects.lbl_menu_bl != NULL) {
        lv_label_set_text_fmt(objects.lbl_menu_bl, "%d %%", g_saved_brightness);
    }
    
    /* 2. Límite eFuse Corriente */
    if (objects.lbl_val_efuse_limit != NULL) {
        int i_int = (int)g_user_settings.ocp_limit;
        int i_dec = (int)(abs((int)((g_user_settings.ocp_limit - i_int) * 100)));
        lv_label_set_text_fmt(objects.lbl_val_efuse_limit, "%d.%02d A", i_int, i_dec);
    }

    /* 3. Límite OVP Sobrevoltaje */
    if (objects.lbl_val_ovp != NULL) {
        int v_int = (int)g_user_settings.ovp_limit;
        int v_dec = (int)(abs((int)((g_user_settings.ovp_limit - v_int) * 100)));
        lv_label_set_text_fmt(objects.lbl_val_ovp, "%d.%02d V", v_int, v_dec);
    }

    /* 4. Switch CC */
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
        if (slider_val > 650) slider_val = 650; /* Max 6.50 A */
        
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
        if (slider_val < 100) slider_val = 100;   /* Min 1.00 V */
        if (slider_val > 5400) slider_val = 5400; /* Max 54.00 V */
        
        g_temp_ovp_limit = slider_val / 100.0f;

        if (objects.limit_slider != NULL && lv_slider_get_value(objects.limit_slider) != slider_val) 
        {
            lv_slider_set_value(objects.limit_slider, slider_val, LV_ANIM_OFF);
        }

        /* 
         * MATEMÁTICA ENTERA EXACTA SIN IMPRECISIÓN DE FLOTANTES:
         * slider_val / 100 da los voltios enteros (ej. 560 / 100 = 5)
         * slider_val % 100 da los decimales exactos (ej. 560 % 100 = 60)
         */
        if (objects.lbl_limit_val != NULL) 
        {
            int v_int = slider_val / 100;
            int v_dec = slider_val % 100;
            lv_label_set_text_fmt(objects.lbl_limit_val, "%d.%02d", v_int, v_dec);
        }
    }
}

/* Prepara la pantalla limit_screen para el Límite de Corriente (eFuse) */
static void setup_current_limit_screen(void)
{
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

    //update_menu_screen_ui();
}

/* Prepara la pantalla limit_screen para el Límite OVP (Sobrevoltaje) */
static void setup_ovp_limit_screen(void)
{
    g_active_limit_mode = MODE_OVP_LIMIT;
    g_temp_ovp_limit = g_user_settings.ovp_limit;

    if (objects.lbl_limit_text != NULL) {
        lv_label_set_text(objects.lbl_limit_text, "OVP LIMIT ADJUSTMENT");
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
        lv_obj_set_x(objects.lbl_limit_max, lv_obj_get_x(objects.lbl_limit_max));
    }

    if (objects.label_limit_dec != NULL) {
        lv_label_set_text(objects.label_limit_dec, "-0.1V");
    }
    if (objects.label_limit_inc != NULL) {
        lv_label_set_text(objects.label_limit_inc, "+0.1V");
    }
    if (objects.label_limit_preset1 != NULL) {
        lv_label_set_text(objects.label_limit_preset1, "5.5V");
    }
    if (objects.label_limit_preset2 != NULL) {
        lv_label_set_text(objects.label_limit_preset2, "12V");
    }
    if (objects.label_limit_preset3 != NULL) {
        lv_label_set_text(objects.label_limit_preset3, "24V");
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

    //update_menu_screen_ui();
}

/* Configura la pantalla limit_screen para el modo de Ajuste de Brillo */
static void setup_brightness_screen(void)
{
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

    //update_menu_screen_ui();
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
                loadScreen(SCREEN_ID_LIMIT_SCREEN);
                setup_current_limit_screen();
                break;
            case 2: /* box_ovp (Límite OVP) */
                loadScreen(SCREEN_ID_LIMIT_SCREEN);
                setup_ovp_limit_screen();
                break;
            case 3: /* box_dispaly_bl (Brillo) */
                loadScreen(SCREEN_ID_LIMIT_SCREEN);
                setup_brightness_screen();
                break;
            default:
                break;
        }
    }
}

static void on_btn_main_config_clicked(lv_event_t *e) {
    if (lv_event_get_code(e) == LV_EVENT_CLICKED) {
        loadScreen(SCREEN_ID_MENU_SCREEN);
        update_menu_screen_ui();
    }
}

static void on_btn_limit_cancel_clicked(lv_event_t *e) {
    if (lv_event_get_code(e) == LV_EVENT_CLICKED) {
        if (g_active_limit_mode == MODE_BRIGHTNESS) {
            Display_SetBrightness(g_saved_brightness);
        }
        loadScreen(SCREEN_ID_MENU_SCREEN);
        update_menu_screen_ui();
    }
}

static void on_btn_limit_save_clicked(lv_event_t *e) {
    if (lv_event_get_code(e) == LV_EVENT_CLICKED) {
        if (g_active_limit_mode == MODE_BRIGHTNESS) {
            g_saved_brightness = g_temp_brightness;
            g_user_settings.brightness = g_saved_brightness;
            Settings_Save();
        } 
        else if (g_active_limit_mode == MODE_CURRENT_LIMIT) {
            g_user_settings.ocp_limit = g_temp_current_limit;
            g_power_sim.ocp_limit = g_user_settings.ocp_limit;
            Settings_Save();
        }
        else if (g_active_limit_mode == MODE_OVP_LIMIT) {
            g_user_settings.ovp_limit = g_temp_ovp_limit;
            g_power_sim.ovp_limit = g_user_settings.ovp_limit;
            Settings_Save();
        }

        loadScreen(SCREEN_ID_MENU_SCREEN);
        update_menu_screen_ui();
    }
}

static void on_btn_graph_clicked(lv_event_t *e) {
    if (lv_event_get_code(e) == LV_EVENT_CLICKED) {
        loadScreen(SCREEN_ID_MENU_SCREEN);
        update_menu_screen_ui();
    }
}

static void on_btn_main_rst_clicked(lv_event_t *e) {
    if (lv_event_get_code(e) == LV_EVENT_CLICKED) {
        loadScreen(SCREEN_ID_RESET_MODAL);
    }
}

static void on_btn_modal_reset_clicked(lv_event_t *e) {
    if (lv_event_get_code(e) == LV_EVENT_CLICKED) {
        PowerSim_ResetStats();
        PowerSim_UpdateUI();
        loadScreen(SCREEN_ID_MAIN_SCREEN);
    }
}

static void on_btn_modal_abort_clicked(lv_event_t *e) {
    if (lv_event_get_code(e) == LV_EVENT_CLICKED) {
        loadScreen(SCREEN_ID_MAIN_SCREEN);
    }
}

static void on_btn_menu_back_clicked(lv_event_t *e) {
    if (lv_event_get_code(e) == LV_EVENT_CLICKED) {
        loadScreen(SCREEN_ID_MAIN_SCREEN);
    }
}

static void on_btn_menu_edit1_clicked(lv_event_t *e) {
    if (lv_event_get_code(e) == LV_EVENT_CLICKED) {
        g_selected_menu_index = 0;
        loadScreen(SCREEN_ID_LIMIT_SCREEN);
        setup_current_limit_screen();
    }
}

static void on_btn_menu_edit3_clicked(lv_event_t *e) {
    if (lv_event_get_code(e) == LV_EVENT_CLICKED) {
        g_selected_menu_index = 2; /* Opción 3: OVP */
        loadScreen(SCREEN_ID_LIMIT_SCREEN);
        setup_ovp_limit_screen();
    }
}

static void on_btn_menu_edit4_clicked(lv_event_t *e) {
    if (lv_event_get_code(e) == LV_EVENT_CLICKED) {
        g_selected_menu_index = 3;
        loadScreen(SCREEN_ID_LIMIT_SCREEN);
        setup_brightness_screen();
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
static void on_limit_slider_changed(lv_event_t *e) {
    if (lv_event_get_code(e) == LV_EVENT_VALUE_CHANGED) {
        int val = lv_slider_get_value(objects.limit_slider);
        update_limit_preview(val);
    }
}

static void on_btn_limit_dec_clicked(lv_event_t *e) {
    if (lv_event_get_code(e) == LV_EVENT_CLICKED) {
        if (g_active_limit_mode == MODE_BRIGHTNESS) {
            update_limit_preview(g_temp_brightness - 10);
        } else if (g_active_limit_mode == MODE_CURRENT_LIMIT) {
            int slider_val = (int)(g_temp_current_limit * 100.0f) - 50; /* -0.5A */
            update_limit_preview(slider_val);
        } else if (g_active_limit_mode == MODE_OVP_LIMIT) {
            int slider_val = (int)(g_temp_ovp_limit * 100.0f) - 10;     /* -0.1V */
            update_limit_preview(slider_val);
        }
    }
}

static void on_btn_limit_inc_clicked(lv_event_t *e) {
    if (lv_event_get_code(e) == LV_EVENT_CLICKED) {
        if (g_active_limit_mode == MODE_BRIGHTNESS) {
            update_limit_preview(g_temp_brightness + 10);
        } else if (g_active_limit_mode == MODE_CURRENT_LIMIT) {
            int slider_val = (int)(g_temp_current_limit * 100.0f) + 50; /* +0.5A */
            update_limit_preview(slider_val);
        } else if (g_active_limit_mode == MODE_OVP_LIMIT) {
            int slider_val = (int)(g_temp_ovp_limit * 100.0f) + 10;     /* +0.1V */
            update_limit_preview(slider_val);
        }
    }
}

static void on_btn_limit_preset1_clicked(lv_event_t *e) {
    if (lv_event_get_code(e) == LV_EVENT_CLICKED) {
        if (g_active_limit_mode == MODE_BRIGHTNESS) {
            update_limit_preview(10);
        } else if (g_active_limit_mode == MODE_CURRENT_LIMIT) {
            update_limit_preview(100); /* 1.0 A */
        } else if (g_active_limit_mode == MODE_OVP_LIMIT) {
            update_limit_preview(550); /* 5.5 V */
        }
    }
}

static void on_btn_limit_preset2_clicked(lv_event_t *e) {
    if (lv_event_get_code(e) == LV_EVENT_CLICKED) {
        if (g_active_limit_mode == MODE_BRIGHTNESS) {
            update_limit_preview(50);
        } else if (g_active_limit_mode == MODE_CURRENT_LIMIT) {
            update_limit_preview(300); /* 3.0 A */
        } else if (g_active_limit_mode == MODE_OVP_LIMIT) {
            update_limit_preview(1200); /* 12.0 V */
        }
    }
}

static void on_btn_limit_preset3_clicked(lv_event_t *e) {
    if (lv_event_get_code(e) == LV_EVENT_CLICKED) {
        if (g_active_limit_mode == MODE_BRIGHTNESS) {
            update_limit_preview(100);
        } else if (g_active_limit_mode == MODE_CURRENT_LIMIT) {
            update_limit_preview(500); /* 5.0 A */
        } else if (g_active_limit_mode == MODE_OVP_LIMIT) {
            update_limit_preview(2400); /* 24.0 V */
        }
    }
}

/* Callback para seleccionar un box al tocarlo directamente con el dedo */
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

/* ===================================================================
   VINCULACIÓN GENERAL DE EVENTOS
   =================================================================== */
void UI_Events_Init(void)
{
    g_saved_brightness = g_user_settings.brightness;
    g_temp_brightness  = g_saved_brightness;

    if (objects.btn_menu_select != NULL) 
    {
        lv_obj_add_event_cb(objects.btn_menu_select, on_btn_menu_select_clicked, LV_EVENT_CLICKED, NULL);
    }

    if (objects.btn_menu_enter != NULL) 
    {
        lv_obj_add_event_cb(objects.btn_menu_enter, on_btn_menu_enter_clicked, LV_EVENT_CLICKED, NULL);
    }

    if (objects.btn_main_config != NULL) 
    {
        lv_obj_add_event_cb(objects.btn_main_config, on_btn_main_config_clicked, LV_EVENT_CLICKED, NULL);
    }

    if (objects.btn_limit_cancel != NULL) 
    {
        lv_obj_add_event_cb(objects.btn_limit_cancel, on_btn_limit_cancel_clicked, LV_EVENT_CLICKED, NULL);
    }

    if (objects.btn_limit_save != NULL) 
    {
        lv_obj_add_event_cb(objects.btn_limit_save, on_btn_limit_save_clicked, LV_EVENT_CLICKED, NULL);
    }

    if (objects.btn_main_graph != NULL) 
    {
        lv_obj_add_event_cb(objects.btn_main_graph, on_btn_graph_clicked, LV_EVENT_CLICKED, NULL);
    }

    if (objects.btn_main_rst != NULL) 
    {
        lv_obj_add_event_cb(objects.btn_main_rst, on_btn_main_rst_clicked, LV_EVENT_CLICKED, NULL);
    }

    if (objects.btn_modal_reset != NULL) 
    {
        lv_obj_add_event_cb(objects.btn_modal_reset, on_btn_modal_reset_clicked, LV_EVENT_CLICKED, NULL);
    }

    if (objects.btn_modal_abort != NULL) 
    {
        lv_obj_add_event_cb(objects.btn_modal_abort, on_btn_modal_abort_clicked, LV_EVENT_CLICKED, NULL);
    }

    if (objects.btn_menu_back != NULL) 
    {
        lv_obj_add_event_cb(objects.btn_menu_back, on_btn_menu_back_clicked, LV_EVENT_CLICKED, NULL);
    }

    if (objects.btn_menu_edit1 != NULL) 
    {
        lv_obj_add_event_cb(objects.btn_menu_edit1, on_btn_menu_edit1_clicked, LV_EVENT_CLICKED, NULL);
    }

    if (objects.btn_menu_edit2 != NULL) 
    {
        lv_obj_add_event_cb(objects.btn_menu_edit2, on_btn_menu_edit2_changed, LV_EVENT_VALUE_CHANGED, NULL);
    }

    if (objects.btn_menu_edit3 != NULL) 
    {
        lv_obj_add_event_cb(objects.btn_menu_edit3, on_btn_menu_edit3_clicked, LV_EVENT_CLICKED, NULL);
    }

    if (objects.btn_menu_edit4 != NULL) 
    {
        lv_obj_add_event_cb(objects.btn_menu_edit4, on_btn_menu_edit4_clicked, LV_EVENT_CLICKED, NULL);
    }

    if (objects.limit_slider != NULL) 
    {
        lv_obj_add_event_cb(objects.limit_slider, on_limit_slider_changed, LV_EVENT_VALUE_CHANGED, NULL);
    }

    if (objects.btn_limit_dec != NULL) 
    {
        lv_obj_add_event_cb(objects.btn_limit_dec, on_btn_limit_dec_clicked, LV_EVENT_CLICKED, NULL);
    }

    if (objects.btn_limit_inc != NULL) {
        lv_obj_add_event_cb(objects.btn_limit_inc, on_btn_limit_inc_clicked, LV_EVENT_CLICKED, NULL);
    }

    if (objects.btn_limit_preset1 != NULL) 
    {
        lv_obj_add_event_cb(objects.btn_limit_preset1, on_btn_limit_preset1_clicked, LV_EVENT_CLICKED, NULL);
    }

    if (objects.btn_limit_preset2 != NULL) {
        lv_obj_add_event_cb(objects.btn_limit_preset2, on_btn_limit_preset2_clicked, LV_EVENT_CLICKED, NULL);
    }

    if (objects.btn_limit_preset3 != NULL)
    {
        lv_obj_add_event_cb(objects.btn_limit_preset3, on_btn_limit_preset3_clicked, LV_EVENT_CLICKED, NULL);
    }

    /* Registrar eventos táctiles directos en los contenedores del menú */
    if (objects.box_efuse != NULL)
    {
        lv_obj_add_event_cb(objects.box_efuse, on_box_clicked, LV_EVENT_CLICKED, NULL);
    }

    if (objects.box_cc != NULL)
    {
        lv_obj_add_event_cb(objects.box_cc, on_box_clicked, LV_EVENT_CLICKED, NULL);
    }         

    if (objects.box_ovp != NULL)
    {
        lv_obj_add_event_cb(objects.box_ovp, on_box_clicked, LV_EVENT_CLICKED, NULL);
    }        

    if (objects.box_dispaly_bl != NULL)
    {
        lv_obj_add_event_cb(objects.box_dispaly_bl, on_box_clicked, LV_EVENT_CLICKED, NULL);
    } 

    if (objects.box_ina_config != NULL)
    {
        lv_obj_add_event_cb(objects.box_ina_config, on_box_clicked, LV_EVENT_CLICKED, NULL);
    } 
}