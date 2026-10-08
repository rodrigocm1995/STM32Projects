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
    _SCREEN_ID_LAST = 4
};

typedef struct _objects_t {
    lv_obj_t *main_screen;
    lv_obj_t *menu_screen;
    lv_obj_t *limit_screen;
    lv_obj_t *reset_modal;
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
    lv_obj_t *lbl_title_volt_1;
    lv_obj_t *btn_menu_back;
    lv_obj_t *btn_label_config_1;
    lv_obj_t *btn_menu_select;
    lv_obj_t *btn_label_graph_1;
    lv_obj_t *btn_menu_enter;
    lv_obj_t *btn_label_rst_1;
    lv_obj_t *box_cc;
    lv_obj_t *btn_menu_edit2;
    lv_obj_t *lbl_cc_state;
    lv_obj_t *lbl_title_volt_2;
    lv_obj_t *box_ovp;
    lv_obj_t *btn_menu_edit3;
    lv_obj_t *lbl_btn_edit3;
    lv_obj_t *lbl_val_ovp;
    lv_obj_t *lbl_title_volt_3;
    lv_obj_t *box_dispaly_bl;
    lv_obj_t *btn_menu_edit4;
    lv_obj_t *lbl_btn_edit4;
    lv_obj_t *lbl_menu_bl;
    lv_obj_t *lbl_title_volt_4;
    lv_obj_t *box_ina_config;
    lv_obj_t *lbl_title_volt_5;
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

void tick_screen_by_id(enum ScreensEnum screenId);
void tick_screen(int screen_index);

void create_screens();

#ifdef __cplusplus
}
#endif

#endif /*EEZ_LVGL_UI_SCREENS_H*/