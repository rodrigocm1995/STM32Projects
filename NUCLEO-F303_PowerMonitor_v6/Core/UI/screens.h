#ifndef EEZ_LVGL_UI_SCREENS_H
#define EEZ_LVGL_UI_SCREENS_H

#include <lvgl.h>

#ifdef __cplusplus
extern "C" {
#endif

// Screens

enum ScreensEnum {
    _SCREEN_ID_FIRST = 1,
    SCREEN_ID_MAIN_SCREEN = 1,
    SCREEN_ID_MENU_SCREEN = 2,
    SCREEN_ID_LIMIT_SCREEN = 3,
    SCREEN_ID_RESET_MODAL = 4,
    SCREEN_ID_INA228_ADC_SCREEN = 5,
    SCREEN_ID_INA228_CAL_SCREEN = 6,
    SCREEN_ID_INA228_ALERT_SCREEN = 7,
    SCREEN_ID_INA228_LIMITS_SCREEN = 8,
    SCREEN_ID_GRAPH_SCREEN = 9,
    _SCREEN_ID_LAST = 9
};

typedef struct _objects_t {
    lv_obj_t *main_screen;
    lv_obj_t *menu_screen;
    lv_obj_t *limit_screen;
    lv_obj_t *reset_modal;
    lv_obj_t *ina228_adc_screen;
    lv_obj_t *ina228_cal_screen;
    lv_obj_t *ina228_alert_screen;
    lv_obj_t *ina228_limits_screen;
    lv_obj_t *graph_screen;
    lv_obj_t *main_hdr;
    lv_obj_t *lbl_temp;
    lv_obj_t *lbl_protocol;
    lv_obj_t *lbl_main_title;
    lv_obj_t *box_volt;
    lv_obj_t *lbl_unit_volt;
    lv_obj_t *lbl_val_volt;
    lv_obj_t *lbl_title_volt;
    lv_obj_t *box_curr;
    lv_obj_t *lbl_unit_curr;
    lv_obj_t *lbl_val_curr;
    lv_obj_t *lbl_title_curr;
    lv_obj_t *box_pwr;
    lv_obj_t *lbl_unit_pwr;
    lv_obj_t *lbl_val_pwr;
    lv_obj_t *lbl_title_pwr;
    lv_obj_t *box_stats_energy;
    lv_obj_t *lbl_unit_energy;
    lv_obj_t *lbl_val_energy;
    lv_obj_t *lbl_title_energy;
    lv_obj_t *box_stats_capacity;
    lv_obj_t *lbl_unit_capacity;
    lv_obj_t *lbl_val_capacity;
    lv_obj_t *lbl_title_capacity;
    lv_obj_t *box_stats_runtime;
    lv_obj_t *lbl_val_runtime;
    lv_obj_t *lbl_title_runtime;
    lv_obj_t *btn_main_config;
    lv_obj_t *btn_label_config;
    lv_obj_t *btn_main_graph;
    lv_obj_t *btn_label_graph;
    lv_obj_t *btn_main_rst;
    lv_obj_t *btn_label_rst;
    lv_obj_t *obj0;
    lv_obj_t *obj1;
    lv_obj_t *obj2;
    lv_obj_t *menu_hdr;
    lv_obj_t *lbl_menu_title;
    lv_obj_t *box_efuse;
    lv_obj_t *btn_menu_edit1;
    lv_obj_t *lbl_btn_edit1;
    lv_obj_t *lbl_val_efuse_limit;
    lv_obj_t *lbl_menu_text1;
    lv_obj_t *btn_menu_back;
    lv_obj_t *btn_label_config_1;
    lv_obj_t *btn_menu_select;
    lv_obj_t *btn_label_graph_1;
    lv_obj_t *btn_menu_enter;
    lv_obj_t *btn_label_rst_1;
    lv_obj_t *box_cc;
    lv_obj_t *btn_menu_edit2;
    lv_obj_t *lbl_cc_state;
    lv_obj_t *lbl_menu_text2;
    lv_obj_t *box_ovp;
    lv_obj_t *btn_menu_edit3;
    lv_obj_t *lbl_btn_edit3;
    lv_obj_t *lbl_val_ovp;
    lv_obj_t *lbl_menu_text3;
    lv_obj_t *box_dispaly_bl;
    lv_obj_t *btn_menu_edit4;
    lv_obj_t *lbl_btn_edit4;
    lv_obj_t *lbl_menu_bl;
    lv_obj_t *lbl_menu_text4;
    lv_obj_t *box_ina_config;
    lv_obj_t *lbl_menu_text5;
    lv_obj_t *btn_menu_edit5;
    lv_obj_t *lbl_btn_edit5;
    lv_obj_t *main_hdr_2;
    lv_obj_t *lbl_limit_text;
    lv_obj_t *box_volt_1;
    lv_obj_t *lbl_limit_unit;
    lv_obj_t *lbl_limit_val;
    lv_obj_t *lbl_limit_max;
    lv_obj_t *lbl_limit_min;
    lv_obj_t *limit_slider;
    lv_obj_t *btn_limit_save;
    lv_obj_t *lbl_limit_save;
    lv_obj_t *btn_limit_cancel;
    lv_obj_t *lbl_limit_cancel;
    lv_obj_t *btn_limit_dec;
    lv_obj_t *label_limit_dec;
    lv_obj_t *btn_limit_inc;
    lv_obj_t *label_limit_inc;
    lv_obj_t *btn_limit_preset1;
    lv_obj_t *label_limit_preset1;
    lv_obj_t *btn_limit_preset2;
    lv_obj_t *label_limit_preset2;
    lv_obj_t *btn_limit_preset3;
    lv_obj_t *label_limit_preset3;
    lv_obj_t *modal_rst_box;
    lv_obj_t *btn_modal_abort;
    lv_obj_t *btn_label_rst_3;
    lv_obj_t *btn_modal_reset;
    lv_obj_t *btn_label_config_12;
    lv_obj_t *lbl_modal_msg;
    lv_obj_t *modal_rst_hdr;
    lv_obj_t *ina228_adc_hdr;
    lv_obj_t *lbl_ina228_adc_title;
    lv_obj_t *lbl_ina228_adc_page;
    lv_obj_t *box_adc_range;
    lv_obj_t *btn_ina228_adc_edit1;
    lv_obj_t *lbl_btn_adc_edit1;
    lv_obj_t *lbl_val_adc_range;
    lv_obj_t *lbl_ina228_adc_text1;
    lv_obj_t *btn_adc_cancel;
    lv_obj_t *btn_label_adc_back;
    lv_obj_t *btn_adc_next;
    lv_obj_t *btn_label_adc_select;
    lv_obj_t *btn_adc_save;
    lv_obj_t *btn_label_adc_enter;
    lv_obj_t *box_adc_mode;
    lv_obj_t *btn_ina228_adc_edit2;
    lv_obj_t *lbl_btn_adc_edit2;
    lv_obj_t *lbl_val_adc_mode;
    lv_obj_t *lbl_ina228_adc_text2;
    lv_obj_t *box_adc_avg;
    lv_obj_t *btn_ina228_adc_edit3;
    lv_obj_t *lbl_btn_adc_edit3;
    lv_obj_t *lbl_val_adc_samples;
    lv_obj_t *lbl_ina228_adc_text3;
    lv_obj_t *box_adc_convdelay;
    lv_obj_t *btn_ina228_adc_edit4;
    lv_obj_t *lbl_btn_adc_edit4;
    lv_obj_t *lbl_val_adc_convdelay;
    lv_obj_t *lbl_ina228_adc_text4;
    lv_obj_t *box_adc_vbus_time;
    lv_obj_t *btn_ina228_adc_edit5;
    lv_obj_t *lbl_btn_adc_edit5;
    lv_obj_t *lbl_val_adc_vbus_time;
    lv_obj_t *lbl_ina228_adc_text5;
    lv_obj_t *box_adc_vshunt_time;
    lv_obj_t *btn_ina228_adc_edit6;
    lv_obj_t *lbl_btn_adc_edit6;
    lv_obj_t *lbl_val_adc_vshunt_time;
    lv_obj_t *lbl_ina228_adc_text6;
    lv_obj_t *box_adc_temp_time;
    lv_obj_t *btn_ina228_adc_edit7;
    lv_obj_t *lbl_btn_adc_edit7;
    lv_obj_t *lbl_val_adc_temp_time;
    lv_obj_t *lbl_ina228_adc_text7;
    lv_obj_t *ina228_cal_hdr;
    lv_obj_t *lbl_ina228_cal_title;
    lv_obj_t *lbl_ina228_cal_page;
    lv_obj_t *box_cal_shunt_res;
    lv_obj_t *lbl_unit_shunt_res;
    lv_obj_t *btn_ina228_cal_edit1;
    lv_obj_t *lbl_btn_adc_edit1_1;
    lv_obj_t *lbl_val_shunt_res;
    lv_obj_t *lbl_ina228_cal_text1;
    lv_obj_t *btn_cal_prev;
    lv_obj_t *btn_label_adc_back_1;
    lv_obj_t *btn_cal_next;
    lv_obj_t *btn_label_adc_select_1;
    lv_obj_t *btn_cal_save;
    lv_obj_t *btn_label_adc_enter_1;
    lv_obj_t *box_cal_max_current;
    lv_obj_t *lbl_unit_max_curr;
    lv_obj_t *btn_ina228_cal_edit2;
    lv_obj_t *lbl_btn_adc_edit1_2;
    lv_obj_t *lbl_val_max_curr;
    lv_obj_t *lbl_ina228_cal_text2;
    lv_obj_t *box_cal_temp_comp;
    lv_obj_t *lbl_val_temp_comp;
    lv_obj_t *lbl_ina228_cal_text3;
    lv_obj_t *switch_cal;
    lv_obj_t *box_cal_shunt_cal;
    lv_obj_t *lbl_val_shunt_cal;
    lv_obj_t *lbl_ina228_cal_text4;
    lv_obj_t *box_cal_manuf_id;
    lv_obj_t *lbl_val_manuf_id;
    lv_obj_t *lbl_ina228_cal_text5;
    lv_obj_t *box_cal_device_id;
    lv_obj_t *lbl_val_device_id;
    lv_obj_t *lbl_ina228_cal_text6;
    lv_obj_t *ina228_alert_hdr;
    lv_obj_t *lbl_ina228_alert_title;
    lv_obj_t *lbl_ina228_alert_page;
    lv_obj_t *box_alert_latch;
    lv_obj_t *lbl_alert_val1;
    lv_obj_t *lbl_ina228_alert_text1;
    lv_obj_t *btn_alert_prev;
    lv_obj_t *btn_label_adc_back_2;
    lv_obj_t *btn_alert_next;
    lv_obj_t *btn_label_adc_select_2;
    lv_obj_t *btn_alert_save;
    lv_obj_t *btn_label_adc_enter_2;
    lv_obj_t *box_alert_cnvr;
    lv_obj_t *lbl_alert_val2;
    lv_obj_t *lbl_ina228_alert_text2;
    lv_obj_t *box_alert_apol;
    lv_obj_t *lbl_alert_val3;
    lv_obj_t *lbl_ina228_alert_text3;
    lv_obj_t *switch_alert3;
    lv_obj_t *box_alert_filter;
    lv_obj_t *lbl_alert_val4;
    lv_obj_t *lbl_ina228_alert_text4;
    lv_obj_t *switch_alert4;
    lv_obj_t *switch_alert1;
    lv_obj_t *switch_alert2;
    lv_obj_t *ina228_thr_hdr;
    lv_obj_t *lbl_ina228_thr_title;
    lv_obj_t *lbl_ina228_thr_page;
    lv_obj_t *box_adc_range_1;
    lv_obj_t *btn_ina228_thr_edit1;
    lv_obj_t *lbl_btn_adc_edit1_3;
    lv_obj_t *lbl_thr_text1;
    lv_obj_t *lbl_val_thr1;
    lv_obj_t *btn_thr_prev;
    lv_obj_t *btn_label_adc_back_3;
    lv_obj_t *btn_thr_save;
    lv_obj_t *btn_label_adc_enter_3;
    lv_obj_t *box_adc_mode_1;
    lv_obj_t *btn_ina228_thr_edit2;
    lv_obj_t *lbl_btn_adc_edit2_1;
    lv_obj_t *lbl_thr_text2;
    lv_obj_t *lbl_val_thr2;
    lv_obj_t *box_adc_avg_1;
    lv_obj_t *lbl_val_thr3;
    lv_obj_t *btn_ina228_thr_edit3;
    lv_obj_t *lbl_btn_adc_edit3_1;
    lv_obj_t *lbl_thr_text3;
    lv_obj_t *box_adc_convdelay_1;
    lv_obj_t *btn_ina228_thr_edit4;
    lv_obj_t *lbl_btn_adc_edit4_1;
    lv_obj_t *lbl_thr_text4;
    lv_obj_t *lbl_val_thr4;
    lv_obj_t *box_adc_vbus_time_1;
    lv_obj_t *btn_ina228_thr_edit5;
    lv_obj_t *lbl_btn_adc_edit5_1;
    lv_obj_t *lbl_val_thr5;
    lv_obj_t *lbl_thr_text5;
    lv_obj_t *box_adc_vshunt_time_1;
    lv_obj_t *btn_ina228_thr_edit6;
    lv_obj_t *lbl_btn_adc_edit6_1;
    lv_obj_t *lbl_val_thr6;
    lv_obj_t *lbl_thr_text6;
    lv_obj_t *btn_graph_back;
    lv_obj_t *btn_label_config_2;
    lv_obj_t *btn_graph_pause;
    lv_obj_t *btn_label_graph_2;
    lv_obj_t *btn_graph_clear;
    lv_obj_t *btn_label_rst_2;
    lv_obj_t *obj3;
    lv_obj_t *lbl_graph_volt;
    lv_obj_t *lbl_graph_current;
    lv_obj_t *lbl_graph_power;
    lv_obj_t *lbl_volt_div;
    lv_obj_t *lbl_curr_div;
} objects_t;

extern objects_t objects;

void create_screen_main_screen();
void tick_screen_main_screen();

void create_screen_menu_screen();
void tick_screen_menu_screen();

void create_screen_limit_screen();
void tick_screen_limit_screen();

void create_screen_reset_modal();
void tick_screen_reset_modal();

void create_screen_ina228_adc_screen();
void tick_screen_ina228_adc_screen();

void create_screen_ina228_cal_screen();
void tick_screen_ina228_cal_screen();

void create_screen_ina228_alert_screen();
void tick_screen_ina228_alert_screen();

void create_screen_ina228_limits_screen();
void tick_screen_ina228_limits_screen();

void create_screen_graph_screen();
void tick_screen_graph_screen();

void tick_screen_by_id(enum ScreensEnum screenId);
void tick_screen(int screen_index);

void create_screens();

#ifdef __cplusplus
}
#endif

#endif /*EEZ_LVGL_UI_SCREENS_H*/