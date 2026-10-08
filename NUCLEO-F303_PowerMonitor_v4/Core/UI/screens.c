#include <string.h>

#include "screens.h"
#include "images.h"
#include "fonts.h"
#include "actions.h"
#include "vars.h"
#include "styles.h"
#include "ui.h"

#include <string.h>

objects_t objects;

//
// Event handlers
//

lv_obj_t *tick_value_change_obj;

//
// Screens
//

void create_screen_main_screen() {
    lv_obj_t *obj = lv_obj_create(0);
    objects.main_screen = obj;
    lv_obj_set_pos(obj, 0, 0);
    lv_obj_set_size(obj, 320, 240);
    lv_obj_remove_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
    lv_obj_set_style_bg_color(obj, lv_color_hex(0x121212), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(obj, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_color(obj, lv_color_hex(0x121212), LV_PART_MAIN | LV_STATE_CHECKED);
    {
        lv_obj_t *parent_obj = obj;
        {
            lv_obj_t *obj = lv_label_create(parent_obj);
            lv_obj_set_pos(obj, 356, 232);
            lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
            lv_label_set_text_static(obj, "Hello, world!");
        }
        {
            // main_hdr
            lv_obj_t *obj = lv_obj_create(parent_obj);
            objects.main_hdr = obj;
            lv_obj_set_pos(obj, 0, 4);
            lv_obj_set_size(obj, 320, 25);
            lv_obj_set_style_pad_left(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_pad_top(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_pad_right(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_pad_bottom(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_border_width(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_radius(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_bg_color(obj, lv_color_hex(0x272727), LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_bg_opa(obj, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
            {
                lv_obj_t *parent_obj = obj;
                {
                    // lbl_temp
                    lv_obj_t *obj = lv_label_create(parent_obj);
                    objects.lbl_temp = obj;
                    lv_obj_set_pos(obj, 245, 5);
                    lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                    lv_obj_set_style_text_color(obj, lv_color_hex(0xffffff), LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_text_font(obj, &ui_font_abee_zee_14, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_text_opa(obj, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_label_set_text_static(obj, "35.19 °C");
                }
                {
                    // lbl_protocol
                    lv_obj_t *obj = lv_label_create(parent_obj);
                    objects.lbl_protocol = obj;
                    lv_obj_set_pos(obj, 148, 5);
                    lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                    lv_obj_set_style_text_color(obj, lv_color_hex(0xffffff), LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_text_font(obj, &ui_font_abee_zee_14, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_text_opa(obj, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_label_set_text_static(obj, "PD 3.0 20V");
                }
                {
                    // lbl_main_title
                    lv_obj_t *obj = lv_label_create(parent_obj);
                    objects.lbl_main_title = obj;
                    lv_obj_set_pos(obj, 13, 5);
                    lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                    lv_obj_set_style_text_opa(obj, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_text_color(obj, lv_color_hex(0xa2ff67), LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_text_font(obj, &ui_font_amiko_12, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_label_set_text_static(obj, "eFuse ON");
                }
            }
        }
        {
            // box_volt
            lv_obj_t *obj = lv_obj_create(parent_obj);
            objects.box_volt = obj;
            lv_obj_set_pos(obj, 6, 39);
            lv_obj_set_size(obj, 308, 36);
            lv_obj_set_style_pad_left(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_pad_top(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_pad_right(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_pad_bottom(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_border_width(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_radius(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_bg_color(obj, lv_color_hex(0x272727), LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_bg_opa(obj, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
            {
                lv_obj_t *parent_obj = obj;
                {
                    // lbl_unit_volt
                    lv_obj_t *obj = lv_label_create(parent_obj);
                    objects.lbl_unit_volt = obj;
                    lv_obj_set_pos(obj, 278, 12);
                    lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                    lv_obj_set_style_text_color(obj, lv_color_hex(0xbb86fc), LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_text_font(obj, &ui_font_abee_zee_14, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_text_opa(obj, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_label_set_text_static(obj, "mV");
                }
                {
                    // lbl_val_volt
                    lv_obj_t *obj = lv_label_create(parent_obj);
                    objects.lbl_val_volt = obj;
                    lv_obj_set_pos(obj, 156, 4);
                    lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                    lv_obj_set_style_text_color(obj, lv_color_hex(0xbb86fc), LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_text_font(obj, &ui_font_abee_zee_24, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_text_opa(obj, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_label_set_text_static(obj, "020.0482");
                }
                {
                    // lbl_title_volt
                    lv_obj_t *obj = lv_label_create(parent_obj);
                    objects.lbl_title_volt = obj;
                    lv_obj_set_pos(obj, 10, 12);
                    lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                    lv_obj_set_style_text_color(obj, lv_color_hex(0xbb86fc), LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_text_opa(obj, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_text_font(obj, &ui_font_amiko_12, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_label_set_text_static(obj, "VOLTAGE");
                }
            }
        }
        {
            // box_curr
            lv_obj_t *obj = lv_obj_create(parent_obj);
            objects.box_curr = obj;
            lv_obj_set_pos(obj, 6, 78);
            lv_obj_set_size(obj, 308, 36);
            lv_obj_set_style_pad_left(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_pad_top(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_pad_right(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_pad_bottom(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_border_width(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_radius(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_bg_color(obj, lv_color_hex(0x272727), LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_bg_opa(obj, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
            {
                lv_obj_t *parent_obj = obj;
                {
                    // lbl_unit_curr
                    lv_obj_t *obj = lv_label_create(parent_obj);
                    objects.lbl_unit_curr = obj;
                    lv_obj_set_pos(obj, 277, 12);
                    lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                    lv_obj_set_style_text_color(obj, lv_color_hex(0x50c9ff), LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_text_font(obj, &ui_font_abee_zee_14, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_text_opa(obj, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_label_set_text_static(obj, "mA");
                }
                {
                    // lbl_val_curr
                    lv_obj_t *obj = lv_label_create(parent_obj);
                    objects.lbl_val_curr = obj;
                    lv_obj_set_pos(obj, 156, 4);
                    lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                    lv_obj_set_style_text_color(obj, lv_color_hex(0x50c9ff), LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_text_font(obj, &ui_font_abee_zee_24, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_text_opa(obj, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_label_set_text_static(obj, "045.1500");
                }
                {
                    // lbl_title_curr
                    lv_obj_t *obj = lv_label_create(parent_obj);
                    objects.lbl_title_curr = obj;
                    lv_obj_set_pos(obj, 10, 12);
                    lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                    lv_obj_set_style_text_color(obj, lv_color_hex(0x50c9ff), LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_text_opa(obj, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_text_font(obj, &ui_font_amiko_12, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_label_set_text_static(obj, "CURRENT");
                }
            }
        }
        {
            // box_pwr
            lv_obj_t *obj = lv_obj_create(parent_obj);
            objects.box_pwr = obj;
            lv_obj_set_pos(obj, 6, 117);
            lv_obj_set_size(obj, 308, 36);
            lv_obj_set_style_pad_left(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_pad_top(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_pad_right(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_pad_bottom(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_border_width(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_radius(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_remove_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
            lv_obj_set_style_bg_color(obj, lv_color_hex(0x272727), LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_bg_opa(obj, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
            {
                lv_obj_t *parent_obj = obj;
                {
                    // lbl_unit_pwr
                    lv_obj_t *obj = lv_label_create(parent_obj);
                    objects.lbl_unit_pwr = obj;
                    lv_obj_set_pos(obj, 275, 12);
                    lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                    lv_obj_set_style_text_color(obj, lv_color_hex(0x03dac6), LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_text_font(obj, &ui_font_abee_zee_14, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_text_opa(obj, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_label_set_text_static(obj, "mW");
                }
                {
                    // lbl_val_pwr
                    lv_obj_t *obj = lv_label_create(parent_obj);
                    objects.lbl_val_pwr = obj;
                    lv_obj_set_pos(obj, 156, 3);
                    lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                    lv_obj_set_style_text_color(obj, lv_color_hex(0x03dac6), LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_text_font(obj, &ui_font_abee_zee_24, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_text_opa(obj, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_label_set_text_static(obj, "120.1863");
                }
                {
                    // lbl_title_pwr
                    lv_obj_t *obj = lv_label_create(parent_obj);
                    objects.lbl_title_pwr = obj;
                    lv_obj_set_pos(obj, 10, 12);
                    lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                    lv_obj_set_style_text_color(obj, lv_color_hex(0x03dac6), LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_text_opa(obj, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_text_font(obj, &ui_font_amiko_12, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_label_set_text_static(obj, "POWER");
                }
            }
        }
        {
            // box_stats_energy
            lv_obj_t *obj = lv_obj_create(parent_obj);
            objects.box_stats_energy = obj;
            lv_obj_set_pos(obj, 6, 162);
            lv_obj_set_size(obj, 86, 38);
            lv_obj_set_style_pad_left(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_pad_top(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_pad_right(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_pad_bottom(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_border_width(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_radius(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_remove_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
            lv_obj_set_style_bg_color(obj, lv_color_hex(0x272727), LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_bg_opa(obj, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
            {
                lv_obj_t *parent_obj = obj;
                {
                    // lbl_unit_energy
                    lv_obj_t *obj = lv_label_create(parent_obj);
                    objects.lbl_unit_energy = obj;
                    lv_obj_set_pos(obj, 29, 25);
                    lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                    lv_obj_set_style_text_color(obj, lv_color_hex(0xffffff), LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_text_opa(obj, 221, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_text_font(obj, &ui_font_azeebee_12, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_label_set_text_static(obj, "mWh");
                }
                {
                    // lbl_val_energy
                    lv_obj_t *obj = lv_label_create(parent_obj);
                    objects.lbl_val_energy = obj;
                    lv_obj_set_pos(obj, 17, 13);
                    lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                    lv_obj_set_style_text_color(obj, lv_color_hex(0xffffff), LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_text_opa(obj, 221, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_text_font(obj, &ui_font_azeebee_12, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_label_set_text_static(obj, "999.0482");
                }
                {
                    // lbl_title_energy
                    lv_obj_t *obj = lv_label_create(parent_obj);
                    objects.lbl_title_energy = obj;
                    lv_obj_set_pos(obj, 23, 2);
                    lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                    lv_obj_set_style_text_color(obj, lv_color_hex(0xffffff), LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_text_opa(obj, 124, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_text_font(obj, &ui_font_amiko_10, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_label_set_text_static(obj, "ENERGY");
                }
            }
        }
        {
            // box_stats_capacity
            lv_obj_t *obj = lv_obj_create(parent_obj);
            objects.box_stats_capacity = obj;
            lv_obj_set_pos(obj, 117, 162);
            lv_obj_set_size(obj, 86, 38);
            lv_obj_set_style_pad_left(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_pad_top(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_pad_right(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_pad_bottom(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_border_width(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_radius(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_remove_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
            lv_obj_set_style_bg_color(obj, lv_color_hex(0x272727), LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_bg_opa(obj, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
            {
                lv_obj_t *parent_obj = obj;
                {
                    // lbl_unit_capacity
                    lv_obj_t *obj = lv_label_create(parent_obj);
                    objects.lbl_unit_capacity = obj;
                    lv_obj_set_pos(obj, 30, 25);
                    lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                    lv_obj_set_style_text_color(obj, lv_color_hex(0xffffff), LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_text_opa(obj, 221, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_text_font(obj, &ui_font_azeebee_12, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_label_set_text_static(obj, "mAh");
                }
                {
                    // lbl_val_capacity
                    lv_obj_t *obj = lv_label_create(parent_obj);
                    objects.lbl_val_capacity = obj;
                    lv_obj_set_pos(obj, 17, 13);
                    lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                    lv_obj_set_style_text_color(obj, lv_color_hex(0xffffff), LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_text_opa(obj, 221, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_text_font(obj, &ui_font_azeebee_12, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_label_set_text_static(obj, "999.0482");
                }
                {
                    // lbl_title_capacity
                    lv_obj_t *obj = lv_label_create(parent_obj);
                    objects.lbl_title_capacity = obj;
                    lv_obj_set_pos(obj, 18, 2);
                    lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                    lv_obj_set_style_text_color(obj, lv_color_hex(0xffffff), LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_text_opa(obj, 124, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_text_font(obj, &ui_font_amiko_10, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_label_set_text_static(obj, "CAPACITY");
                }
            }
        }
        {
            // box_stats_runtime
            lv_obj_t *obj = lv_obj_create(parent_obj);
            objects.box_stats_runtime = obj;
            lv_obj_set_pos(obj, 229, 163);
            lv_obj_set_size(obj, 86, 38);
            lv_obj_set_style_pad_left(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_pad_top(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_pad_right(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_pad_bottom(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_border_width(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_radius(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_remove_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
            lv_obj_set_style_bg_color(obj, lv_color_hex(0x272727), LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_bg_opa(obj, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
            {
                lv_obj_t *parent_obj = obj;
                {
                    // lbl_val_runtime
                    lv_obj_t *obj = lv_label_create(parent_obj);
                    objects.lbl_val_runtime = obj;
                    lv_obj_set_pos(obj, 18, 18);
                    lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                    lv_obj_set_style_text_color(obj, lv_color_hex(0xffffff), LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_text_opa(obj, 221, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_text_font(obj, &ui_font_azeebee_12, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_label_set_text_static(obj, "00:14:32");
                }
                {
                    // lbl_title_runtime
                    lv_obj_t *obj = lv_label_create(parent_obj);
                    objects.lbl_title_runtime = obj;
                    lv_obj_set_pos(obj, 18, 2);
                    lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                    lv_obj_set_style_text_color(obj, lv_color_hex(0xffffff), LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_text_opa(obj, 124, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_text_font(obj, &ui_font_amiko_10, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_label_set_text_static(obj, "RUN TIME");
                }
            }
        }
        {
            // btn_main_config
            lv_obj_t *obj = lv_button_create(parent_obj);
            objects.btn_main_config = obj;
            lv_obj_set_pos(obj, 6, 206);
            lv_obj_set_size(obj, 86, 28);
            lv_obj_set_style_bg_color(obj, lv_color_hex(0xffffff), LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_text_color(obj, lv_color_hex(0x000000), LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_text_opa(obj, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_text_font(obj, &ui_font_amiko_12, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_radius(obj, 4, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_shadow_ofs_y(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_text_align(obj, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_PRESSED);
            {
                lv_obj_t *parent_obj = obj;
                {
                    // btn_label_config
                    lv_obj_t *obj = lv_label_create(parent_obj);
                    objects.btn_label_config = obj;
                    lv_obj_set_pos(obj, 0, 0);
                    lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                    lv_obj_set_style_align(obj, LV_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_text_font(obj, &ui_font_amiko_12, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_label_set_text_static(obj, "CONFIG");
                }
            }
        }
        {
            // btn_main_graph
            lv_obj_t *obj = lv_button_create(parent_obj);
            objects.btn_main_graph = obj;
            lv_obj_set_pos(obj, 118, 206);
            lv_obj_set_size(obj, 86, 28);
            lv_obj_set_style_bg_color(obj, lv_color_hex(0xffffff), LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_text_color(obj, lv_color_hex(0x000000), LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_text_opa(obj, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_text_font(obj, &ui_font_amiko_12, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_radius(obj, 4, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_shadow_ofs_y(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_text_align(obj, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_PRESSED);
            {
                lv_obj_t *parent_obj = obj;
                {
                    // btn_label_graph
                    lv_obj_t *obj = lv_label_create(parent_obj);
                    objects.btn_label_graph = obj;
                    lv_obj_set_pos(obj, 0, 0);
                    lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                    lv_obj_set_style_align(obj, LV_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_text_font(obj, &ui_font_amiko_12, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_label_set_text_static(obj, "GRAPH");
                }
            }
        }
        {
            // btn_main_rst
            lv_obj_t *obj = lv_button_create(parent_obj);
            objects.btn_main_rst = obj;
            lv_obj_set_pos(obj, 229, 206);
            lv_obj_set_size(obj, 86, 28);
            lv_obj_set_style_bg_color(obj, lv_color_hex(0xef5350), LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_text_color(obj, lv_color_hex(0xffffff), LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_text_opa(obj, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_text_font(obj, &ui_font_amiko_12, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_radius(obj, 4, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_bg_opa(obj, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_shadow_ofs_y(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_text_align(obj, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_PRESSED);
            {
                lv_obj_t *parent_obj = obj;
                {
                    // btn_label_rst
                    lv_obj_t *obj = lv_label_create(parent_obj);
                    objects.btn_label_rst = obj;
                    lv_obj_set_pos(obj, 0, 0);
                    lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                    lv_obj_set_style_align(obj, LV_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_text_color(obj, lv_color_hex(0xffffff), LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_label_set_text_static(obj, "RST ACCUM");
                }
            }
        }
        {
            lv_obj_t *obj = lv_obj_create(parent_obj);
            objects.obj0 = obj;
            lv_obj_set_pos(obj, 6, 162);
            lv_obj_set_size(obj, 4, 38);
            lv_obj_set_style_pad_left(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_pad_top(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_pad_right(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_pad_bottom(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_border_width(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_radius(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_bg_color(obj, lv_color_hex(0x46cd24), LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_bg_opa(obj, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
        }
        {
            lv_obj_t *obj = lv_obj_create(parent_obj);
            objects.obj1 = obj;
            lv_obj_set_pos(obj, 117, 162);
            lv_obj_set_size(obj, 4, 38);
            lv_obj_set_style_pad_left(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_pad_top(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_pad_right(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_pad_bottom(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_border_width(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_radius(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_bg_color(obj, lv_color_hex(0xcd4c24), LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_bg_opa(obj, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
        }
        {
            lv_obj_t *obj = lv_obj_create(parent_obj);
            objects.obj2 = obj;
            lv_obj_set_pos(obj, 229, 163);
            lv_obj_set_size(obj, 4, 38);
            lv_obj_set_style_pad_left(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_pad_top(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_pad_right(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_pad_bottom(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_border_width(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_radius(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_bg_color(obj, lv_color_hex(0x24b3cd), LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_bg_opa(obj, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
        }
    }
    
    tick_screen_main_screen();
}

void tick_screen_main_screen() {
}

void create_screen_menu_screen() {
    lv_obj_t *obj = lv_obj_create(0);
    objects.menu_screen = obj;
    lv_obj_set_pos(obj, 0, 0);
    lv_obj_set_size(obj, 320, 240);
    lv_obj_remove_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
    lv_obj_set_style_bg_color(obj, lv_color_hex(0x121212), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(obj, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
    {
        lv_obj_t *parent_obj = obj;
        {
            // menu_hdr
            lv_obj_t *obj = lv_obj_create(parent_obj);
            objects.menu_hdr = obj;
            lv_obj_set_pos(obj, 0, 4);
            lv_obj_set_size(obj, 320, 25);
            lv_obj_set_style_pad_left(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_pad_top(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_pad_right(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_pad_bottom(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_border_width(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_radius(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_bg_color(obj, lv_color_hex(0x272727), LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_bg_opa(obj, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
            {
                lv_obj_t *parent_obj = obj;
                {
                    // lbl_menu_title
                    lv_obj_t *obj = lv_label_create(parent_obj);
                    objects.lbl_menu_title = obj;
                    lv_obj_set_pos(obj, 13, 6);
                    lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                    lv_obj_set_style_text_opa(obj, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_text_color(obj, lv_color_hex(0xffffff), LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_text_font(obj, &ui_font_amiko_12, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_label_set_text_static(obj, "HARDWARE SETTINGS");
                }
            }
        }
        {
            // box_efuse
            lv_obj_t *obj = lv_obj_create(parent_obj);
            objects.box_efuse = obj;
            lv_obj_set_pos(obj, 6, 38);
            lv_obj_set_size(obj, 308, 30);
            lv_obj_set_style_pad_left(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_pad_top(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_pad_right(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_pad_bottom(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_border_width(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_radius(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_remove_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
            lv_obj_set_style_bg_color(obj, lv_color_hex(0x272727), LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_bg_opa(obj, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_bg_color(obj, lv_color_hex(0x383838), LV_PART_MAIN | LV_STATE_FOCUSED);
            lv_obj_set_style_bg_opa(obj, 255, LV_PART_MAIN | LV_STATE_FOCUSED);
            lv_obj_set_style_bg_color(obj, lv_color_hex(0x383838), LV_PART_MAIN | LV_STATE_HOVERED);
            lv_obj_set_style_bg_opa(obj, 255, LV_PART_MAIN | LV_STATE_HOVERED);
            {
                lv_obj_t *parent_obj = obj;
                {
                    // btn_menu_edit1
                    lv_obj_t *obj = lv_button_create(parent_obj);
                    objects.btn_menu_edit1 = obj;
                    lv_obj_set_pos(obj, 251, 3);
                    lv_obj_set_size(obj, 52, 24);
                    lv_obj_set_style_bg_color(obj, lv_color_hex(0xffffff), LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_text_color(obj, lv_color_hex(0x000000), LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_text_opa(obj, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_text_font(obj, &ui_font_amiko_12, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_radius(obj, 4, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_shadow_ofs_y(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_text_align(obj, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_PRESSED);
                    {
                        lv_obj_t *parent_obj = obj;
                        {
                            // lbl_btn_edit1
                            lv_obj_t *obj = lv_label_create(parent_obj);
                            objects.lbl_btn_edit1 = obj;
                            lv_obj_set_pos(obj, 0, 0);
                            lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                            lv_obj_set_style_align(obj, LV_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_obj_set_style_text_font(obj, &ui_font_amiko_12, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_label_set_text_static(obj, "SET");
                        }
                    }
                }
                {
                    // lbl_val_efuse_limit
                    lv_obj_t *obj = lv_label_create(parent_obj);
                    objects.lbl_val_efuse_limit = obj;
                    lv_obj_set_pos(obj, 166, 8);
                    lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                    lv_obj_set_style_text_color(obj, lv_color_hex(0xffffff), LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_text_font(obj, &ui_font_azeebee_12, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_text_opa(obj, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_label_set_text_static(obj, "6.00 A");
                }
                {
                    // lbl_menu_text1
                    lv_obj_t *obj = lv_label_create(parent_obj);
                    objects.lbl_menu_text1 = obj;
                    lv_obj_set_pos(obj, 7, 8);
                    lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                    lv_obj_set_style_text_color(obj, lv_color_hex(0xffffff), LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_text_opa(obj, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_text_font(obj, &ui_font_amiko_12, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_label_set_text_static(obj, "1. eFuse Limit");
                }
            }
        }
        {
            // btn_menu_back
            lv_obj_t *obj = lv_button_create(parent_obj);
            objects.btn_menu_back = obj;
            lv_obj_set_pos(obj, 6, 206);
            lv_obj_set_size(obj, 86, 28);
            lv_obj_set_style_bg_color(obj, lv_color_hex(0xef5350), LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_text_color(obj, lv_color_hex(0x000000), LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_text_opa(obj, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_text_font(obj, &ui_font_amiko_12, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_radius(obj, 4, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_shadow_ofs_y(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_bg_opa(obj, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_text_align(obj, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_PRESSED);
            lv_obj_set_style_bg_color(obj, lv_color_hex(0xef5350), LV_PART_MAIN | LV_STATE_CHECKED);
            {
                lv_obj_t *parent_obj = obj;
                {
                    // btn_label_config_1
                    lv_obj_t *obj = lv_label_create(parent_obj);
                    objects.btn_label_config_1 = obj;
                    lv_obj_set_pos(obj, 0, 0);
                    lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                    lv_obj_set_style_align(obj, LV_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_text_color(obj, lv_color_hex(0xffffff), LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_label_set_text_static(obj, "BACK");
                }
            }
        }
        {
            // btn_menu_select
            lv_obj_t *obj = lv_button_create(parent_obj);
            objects.btn_menu_select = obj;
            lv_obj_set_pos(obj, 118, 206);
            lv_obj_set_size(obj, 86, 28);
            lv_obj_set_style_bg_color(obj, lv_color_hex(0xffffff), LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_text_color(obj, lv_color_hex(0x000000), LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_text_opa(obj, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_text_font(obj, &ui_font_amiko_12, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_radius(obj, 4, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_shadow_ofs_y(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_text_align(obj, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_PRESSED);
            {
                lv_obj_t *parent_obj = obj;
                {
                    // btn_label_graph_1
                    lv_obj_t *obj = lv_label_create(parent_obj);
                    objects.btn_label_graph_1 = obj;
                    lv_obj_set_pos(obj, 0, 0);
                    lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                    lv_obj_set_style_align(obj, LV_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_label_set_text_static(obj, "SELECT");
                }
            }
        }
        {
            // btn_menu_enter
            lv_obj_t *obj = lv_button_create(parent_obj);
            objects.btn_menu_enter = obj;
            lv_obj_set_pos(obj, 229, 206);
            lv_obj_set_size(obj, 86, 28);
            lv_obj_set_style_bg_color(obj, lv_color_hex(0xa2ff67), LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_text_color(obj, lv_color_hex(0xffffff), LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_text_opa(obj, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_text_font(obj, &ui_font_amiko_12, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_radius(obj, 4, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_bg_opa(obj, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_shadow_ofs_y(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_text_align(obj, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_PRESSED);
            lv_obj_set_style_bg_color(obj, lv_color_hex(0xa2ff67), LV_PART_MAIN | LV_STATE_CHECKED);
            {
                lv_obj_t *parent_obj = obj;
                {
                    // btn_label_rst_1
                    lv_obj_t *obj = lv_label_create(parent_obj);
                    objects.btn_label_rst_1 = obj;
                    lv_obj_set_pos(obj, 0, 0);
                    lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                    lv_obj_set_style_align(obj, LV_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_text_color(obj, lv_color_hex(0x000000), LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_text_opa(obj, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_text_font(obj, &ui_font_amiko_12, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_label_set_text_static(obj, "ENTER");
                }
            }
        }
        {
            // box_cc
            lv_obj_t *obj = lv_obj_create(parent_obj);
            objects.box_cc = obj;
            lv_obj_set_pos(obj, 6, 70);
            lv_obj_set_size(obj, 308, 30);
            lv_obj_set_style_pad_left(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_pad_top(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_pad_right(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_pad_bottom(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_border_width(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_radius(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_remove_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
            lv_obj_set_style_bg_color(obj, lv_color_hex(0x272727), LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_bg_opa(obj, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_bg_color(obj, lv_color_hex(0x383838), LV_PART_MAIN | LV_STATE_CHECKED | LV_STATE_PRESSED);
            lv_obj_set_style_bg_opa(obj, 255, LV_PART_MAIN | LV_STATE_CHECKED | LV_STATE_PRESSED);
            lv_obj_set_style_bg_color(obj, lv_color_hex(0x383838), LV_PART_MAIN | LV_STATE_HOVERED);
            lv_obj_set_style_bg_opa(obj, 255, LV_PART_MAIN | LV_STATE_HOVERED);
            lv_obj_set_style_bg_color(obj, lv_color_hex(0x272727), LV_PART_MAIN | LV_STATE_FOCUSED);
            {
                lv_obj_t *parent_obj = obj;
                {
                    // btn_menu_edit2
                    lv_obj_t *obj = lv_switch_create(parent_obj);
                    objects.btn_menu_edit2 = obj;
                    lv_obj_set_pos(obj, 252, 3);
                    lv_obj_set_size(obj, 50, 25);
                    lv_obj_set_style_bg_color(obj, lv_color_hex(0xbb86fc), LV_PART_INDICATOR | LV_STATE_CHECKED);
                }
                {
                    // lbl_cc_state
                    lv_obj_t *obj = lv_label_create(parent_obj);
                    objects.lbl_cc_state = obj;
                    lv_obj_set_pos(obj, 166, 8);
                    lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                    lv_obj_set_style_text_color(obj, lv_color_hex(0xffffff), LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_text_font(obj, &ui_font_azeebee_12, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_text_opa(obj, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_label_set_text_static(obj, "OFF");
                }
                {
                    // lbl_menu_text2
                    lv_obj_t *obj = lv_label_create(parent_obj);
                    objects.lbl_menu_text2 = obj;
                    lv_obj_set_pos(obj, 7, 8);
                    lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                    lv_obj_set_style_text_color(obj, lv_color_hex(0xffffff), LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_text_opa(obj, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_text_font(obj, &ui_font_amiko_12, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_label_set_text_static(obj, "2. CC Output");
                }
            }
        }
        {
            // box_ovp
            lv_obj_t *obj = lv_obj_create(parent_obj);
            objects.box_ovp = obj;
            lv_obj_set_pos(obj, 6, 102);
            lv_obj_set_size(obj, 308, 30);
            lv_obj_set_style_pad_left(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_pad_top(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_pad_right(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_pad_bottom(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_border_width(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_radius(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_remove_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
            lv_obj_set_style_bg_color(obj, lv_color_hex(0x272727), LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_bg_opa(obj, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_bg_color(obj, lv_color_hex(0x383838), LV_PART_MAIN | LV_STATE_CHECKED | LV_STATE_PRESSED);
            lv_obj_set_style_bg_opa(obj, 255, LV_PART_MAIN | LV_STATE_CHECKED | LV_STATE_PRESSED);
            lv_obj_set_style_bg_color(obj, lv_color_hex(0x383838), LV_PART_MAIN | LV_STATE_HOVERED);
            lv_obj_set_style_bg_opa(obj, 255, LV_PART_MAIN | LV_STATE_HOVERED);
            {
                lv_obj_t *parent_obj = obj;
                {
                    // btn_menu_edit3
                    lv_obj_t *obj = lv_button_create(parent_obj);
                    objects.btn_menu_edit3 = obj;
                    lv_obj_set_pos(obj, 250, 3);
                    lv_obj_set_size(obj, 52, 24);
                    lv_obj_set_style_bg_color(obj, lv_color_hex(0xffffff), LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_text_color(obj, lv_color_hex(0x000000), LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_text_opa(obj, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_text_font(obj, &ui_font_amiko_12, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_radius(obj, 4, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_shadow_ofs_y(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_text_align(obj, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_PRESSED);
                    {
                        lv_obj_t *parent_obj = obj;
                        {
                            // lbl_btn_edit3
                            lv_obj_t *obj = lv_label_create(parent_obj);
                            objects.lbl_btn_edit3 = obj;
                            lv_obj_set_pos(obj, 0, 0);
                            lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                            lv_obj_set_style_align(obj, LV_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_obj_set_style_text_font(obj, &ui_font_amiko_12, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_label_set_text_static(obj, "MODE");
                        }
                    }
                }
                {
                    // lbl_val_ovp
                    lv_obj_t *obj = lv_label_create(parent_obj);
                    objects.lbl_val_ovp = obj;
                    lv_obj_set_pos(obj, 166, 8);
                    lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                    lv_obj_set_style_text_color(obj, lv_color_hex(0xffffff), LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_text_font(obj, &ui_font_amiko_12, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_text_opa(obj, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_label_set_text_static(obj, "5.5 V USB");
                }
                {
                    // lbl_menu_text3
                    lv_obj_t *obj = lv_label_create(parent_obj);
                    objects.lbl_menu_text3 = obj;
                    lv_obj_set_pos(obj, 7, 8);
                    lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                    lv_obj_set_style_text_color(obj, lv_color_hex(0xffffff), LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_text_opa(obj, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_text_font(obj, &ui_font_amiko_12, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_label_set_text_static(obj, "3. Set OVP Preset");
                }
            }
        }
        {
            // box_dispaly_bl
            lv_obj_t *obj = lv_obj_create(parent_obj);
            objects.box_dispaly_bl = obj;
            lv_obj_set_pos(obj, 6, 134);
            lv_obj_set_size(obj, 308, 30);
            lv_obj_set_style_pad_left(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_pad_top(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_pad_right(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_pad_bottom(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_border_width(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_radius(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_remove_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
            lv_obj_set_style_bg_color(obj, lv_color_hex(0x272727), LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_bg_opa(obj, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_bg_color(obj, lv_color_hex(0x383838), LV_PART_MAIN | LV_STATE_CHECKED | LV_STATE_PRESSED);
            lv_obj_set_style_bg_opa(obj, 255, LV_PART_MAIN | LV_STATE_CHECKED | LV_STATE_PRESSED);
            lv_obj_set_style_bg_color(obj, lv_color_hex(0x383838), LV_PART_MAIN | LV_STATE_HOVERED);
            lv_obj_set_style_bg_opa(obj, 255, LV_PART_MAIN | LV_STATE_HOVERED);
            {
                lv_obj_t *parent_obj = obj;
                {
                    // btn_menu_edit4
                    lv_obj_t *obj = lv_button_create(parent_obj);
                    objects.btn_menu_edit4 = obj;
                    lv_obj_set_pos(obj, 250, 3);
                    lv_obj_set_size(obj, 52, 24);
                    lv_obj_set_style_bg_color(obj, lv_color_hex(0xffffff), LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_text_color(obj, lv_color_hex(0x000000), LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_text_opa(obj, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_text_font(obj, &ui_font_amiko_12, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_radius(obj, 4, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_shadow_ofs_y(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_text_align(obj, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_PRESSED);
                    {
                        lv_obj_t *parent_obj = obj;
                        {
                            // lbl_btn_edit4
                            lv_obj_t *obj = lv_label_create(parent_obj);
                            objects.lbl_btn_edit4 = obj;
                            lv_obj_set_pos(obj, 0, 0);
                            lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                            lv_obj_set_style_align(obj, LV_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_obj_set_style_text_font(obj, &ui_font_amiko_12, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_label_set_text_static(obj, "ADJ");
                        }
                    }
                }
                {
                    // lbl_menu_bl
                    lv_obj_t *obj = lv_label_create(parent_obj);
                    objects.lbl_menu_bl = obj;
                    lv_obj_set_pos(obj, 166, 8);
                    lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                    lv_obj_set_style_text_color(obj, lv_color_hex(0xffffff), LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_text_font(obj, &ui_font_amiko_12, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_text_opa(obj, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_label_set_text_static(obj, "80 %");
                }
                {
                    // lbl_menu_text4
                    lv_obj_t *obj = lv_label_create(parent_obj);
                    objects.lbl_menu_text4 = obj;
                    lv_obj_set_pos(obj, 7, 8);
                    lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                    lv_obj_set_style_text_color(obj, lv_color_hex(0xffffff), LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_text_opa(obj, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_text_font(obj, &ui_font_amiko_12, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_label_set_text_static(obj, "4. Display Brightness");
                }
            }
        }
        {
            // box_ina_config
            lv_obj_t *obj = lv_obj_create(parent_obj);
            objects.box_ina_config = obj;
            lv_obj_set_pos(obj, 6, 166);
            lv_obj_set_size(obj, 308, 30);
            lv_obj_set_style_pad_left(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_pad_top(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_pad_right(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_pad_bottom(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_border_width(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_radius(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_remove_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
            lv_obj_set_style_bg_color(obj, lv_color_hex(0x272727), LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_bg_opa(obj, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_bg_color(obj, lv_color_hex(0x383838), LV_PART_MAIN | LV_STATE_CHECKED | LV_STATE_PRESSED);
            lv_obj_set_style_bg_opa(obj, 255, LV_PART_MAIN | LV_STATE_CHECKED | LV_STATE_PRESSED);
            lv_obj_set_style_bg_color(obj, lv_color_hex(0x383838), LV_PART_MAIN | LV_STATE_HOVERED);
            lv_obj_set_style_bg_opa(obj, 255, LV_PART_MAIN | LV_STATE_HOVERED);
            {
                lv_obj_t *parent_obj = obj;
                {
                    // lbl_menu_text5
                    lv_obj_t *obj = lv_label_create(parent_obj);
                    objects.lbl_menu_text5 = obj;
                    lv_obj_set_pos(obj, 7, 8);
                    lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                    lv_obj_set_style_text_color(obj, lv_color_hex(0xffffff), LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_text_opa(obj, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_text_font(obj, &ui_font_amiko_12, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_label_set_text_static(obj, "5. Internal INA228 Config");
                }
                {
                    // btn_menu_edit5
                    lv_obj_t *obj = lv_button_create(parent_obj);
                    objects.btn_menu_edit5 = obj;
                    lv_obj_set_pos(obj, 250, 3);
                    lv_obj_set_size(obj, 52, 24);
                    lv_obj_set_style_bg_color(obj, lv_color_hex(0xffffff), LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_text_color(obj, lv_color_hex(0x000000), LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_text_opa(obj, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_text_font(obj, &ui_font_amiko_12, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_radius(obj, 4, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_shadow_ofs_y(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_text_align(obj, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_PRESSED);
                    {
                        lv_obj_t *parent_obj = obj;
                        {
                            // lbl_btn_edit5
                            lv_obj_t *obj = lv_label_create(parent_obj);
                            objects.lbl_btn_edit5 = obj;
                            lv_obj_set_pos(obj, 0, 0);
                            lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                            lv_obj_set_style_align(obj, LV_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_obj_set_style_text_font(obj, &ui_font_amiko_12, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_label_set_text_static(obj, "ENTER");
                        }
                    }
                }
            }
        }
    }
    
    tick_screen_menu_screen();
}

void tick_screen_menu_screen() {
}

void create_screen_limit_screen() {
    lv_obj_t *obj = lv_obj_create(0);
    objects.limit_screen = obj;
    lv_obj_set_pos(obj, 0, 0);
    lv_obj_set_size(obj, 320, 240);
    lv_obj_remove_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
    lv_obj_set_style_bg_color(obj, lv_color_hex(0x121212), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(obj, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
    {
        lv_obj_t *parent_obj = obj;
        {
            lv_obj_t *obj = lv_label_create(parent_obj);
            lv_obj_set_pos(obj, 356, 232);
            lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
            lv_label_set_text_static(obj, "Hello, world!");
        }
        {
            // main_hdr_2
            lv_obj_t *obj = lv_obj_create(parent_obj);
            objects.main_hdr_2 = obj;
            lv_obj_set_pos(obj, 0, 4);
            lv_obj_set_size(obj, 320, 25);
            lv_obj_set_style_pad_left(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_pad_top(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_pad_right(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_pad_bottom(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_border_width(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_radius(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_bg_color(obj, lv_color_hex(0x272727), LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_bg_opa(obj, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
            {
                lv_obj_t *parent_obj = obj;
                {
                    // lbl_limit_text
                    lv_obj_t *obj = lv_label_create(parent_obj);
                    objects.lbl_limit_text = obj;
                    lv_obj_set_pos(obj, 13, 5);
                    lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                    lv_obj_set_style_text_opa(obj, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_text_color(obj, lv_color_hex(0xffffff), LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_text_font(obj, &ui_font_amiko_12, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_label_set_text_static(obj, "CURRENT LIMIT ADJUSTMENT (eFuse)");
                }
            }
        }
        {
            // box_volt_1
            lv_obj_t *obj = lv_obj_create(parent_obj);
            objects.box_volt_1 = obj;
            lv_obj_set_pos(obj, 6, 39);
            lv_obj_set_size(obj, 308, 110);
            lv_obj_set_style_pad_left(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_pad_top(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_pad_right(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_pad_bottom(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_border_width(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_radius(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_remove_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
            lv_obj_set_style_bg_color(obj, lv_color_hex(0x272727), LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_bg_opa(obj, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
            {
                lv_obj_t *parent_obj = obj;
                {
                    // lbl_limit_unit
                    lv_obj_t *obj = lv_label_create(parent_obj);
                    objects.lbl_limit_unit = obj;
                    lv_obj_set_pos(obj, 193, 24);
                    lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                    lv_obj_set_style_text_color(obj, lv_color_hex(0xffffff), LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_text_opa(obj, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_text_font(obj, &ui_font_abee_zee_14, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_label_set_text_static(obj, "A");
                }
                {
                    // lbl_limit_val
                    lv_obj_t *obj = lv_label_create(parent_obj);
                    objects.lbl_limit_val = obj;
                    lv_obj_set_pos(obj, 109, 17);
                    lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                    lv_obj_set_style_text_color(obj, lv_color_hex(0xffffff), LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_text_opa(obj, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_text_font(obj, &ui_font_abee_zee_24, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_label_set_text_static(obj, "3.5000");
                }
                {
                    // lbl_limit_max
                    lv_obj_t *obj = lv_label_create(parent_obj);
                    objects.lbl_limit_max = obj;
                    lv_obj_set_pos(obj, 243, 88);
                    lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                    lv_obj_set_style_text_color(obj, lv_color_hex(0xffffff), LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_text_font(obj, &ui_font_azeebee_12, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_text_opa(obj, 150, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_label_set_text_static(obj, "6.0000 A");
                }
                {
                    // lbl_limit_min
                    lv_obj_t *obj = lv_label_create(parent_obj);
                    objects.lbl_limit_min = obj;
                    lv_obj_set_pos(obj, 14, 88);
                    lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                    lv_obj_set_style_text_color(obj, lv_color_hex(0xffffff), LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_text_font(obj, &ui_font_azeebee_12, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_text_opa(obj, 150, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_label_set_text_static(obj, "0.6000 A");
                }
                {
                    // limit_slider
                    lv_obj_t *obj = lv_slider_create(parent_obj);
                    objects.limit_slider = obj;
                    lv_obj_set_pos(obj, 14, 62);
                    lv_obj_set_size(obj, 280, 14);
                    lv_slider_set_range(obj, 60, 600);
                    lv_slider_set_value(obj, 250, LV_ANIM_OFF);
                    lv_obj_set_style_bg_color(obj, lv_color_hex(0x0d1117), LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_bg_opa(obj, 51, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_bg_color(obj, lv_color_hex(0xffffff), LV_PART_INDICATOR | LV_STATE_DEFAULT);
                    lv_obj_set_style_bg_opa(obj, 255, LV_PART_INDICATOR | LV_STATE_DEFAULT);
                    lv_obj_set_style_bg_color(obj, lv_color_hex(0x8b8b8b), LV_PART_KNOB | LV_STATE_DEFAULT);
                    lv_obj_set_style_bg_opa(obj, 255, LV_PART_KNOB | LV_STATE_DEFAULT);
                }
            }
        }
        {
            // btn_limit_save
            lv_obj_t *obj = lv_button_create(parent_obj);
            objects.btn_limit_save = obj;
            lv_obj_set_pos(obj, 162, 204);
            lv_obj_set_size(obj, 152, 28);
            lv_obj_set_style_bg_color(obj, lv_color_hex(0xa2ff67), LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_text_color(obj, lv_color_hex(0x000000), LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_text_opa(obj, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_text_font(obj, &ui_font_amiko_12, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_radius(obj, 4, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_shadow_ofs_y(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_text_align(obj, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_PRESSED);
            {
                lv_obj_t *parent_obj = obj;
                {
                    // lbl_limit_save
                    lv_obj_t *obj = lv_label_create(parent_obj);
                    objects.lbl_limit_save = obj;
                    lv_obj_set_pos(obj, 0, 0);
                    lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                    lv_obj_set_style_align(obj, LV_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_label_set_text_static(obj, "SAVE");
                }
            }
        }
        {
            // btn_limit_cancel
            lv_obj_t *obj = lv_button_create(parent_obj);
            objects.btn_limit_cancel = obj;
            lv_obj_set_pos(obj, 6, 204);
            lv_obj_set_size(obj, 152, 28);
            lv_obj_set_style_bg_color(obj, lv_color_hex(0xef5350), LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_text_color(obj, lv_color_hex(0xffffff), LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_text_opa(obj, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_text_font(obj, &ui_font_amiko_12, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_radius(obj, 4, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_bg_opa(obj, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_shadow_ofs_y(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_text_align(obj, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_PRESSED);
            {
                lv_obj_t *parent_obj = obj;
                {
                    // lbl_limit_cancel
                    lv_obj_t *obj = lv_label_create(parent_obj);
                    objects.lbl_limit_cancel = obj;
                    lv_obj_set_pos(obj, 0, 0);
                    lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                    lv_obj_set_style_align(obj, LV_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_text_color(obj, lv_color_hex(0xffffff), LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_label_set_text_static(obj, "CANCEL");
                }
            }
        }
        {
            // btn_limit_dec
            lv_obj_t *obj = lv_button_create(parent_obj);
            objects.btn_limit_dec = obj;
            lv_obj_set_pos(obj, 7, 161);
            lv_obj_set_size(obj, 56, 32);
            lv_obj_set_style_bg_color(obj, lv_color_hex(0xef5350), LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_text_color(obj, lv_color_hex(0xffffff), LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_text_opa(obj, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_text_font(obj, &ui_font_amiko_12, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_radius(obj, 4, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_shadow_ofs_y(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_text_align(obj, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_PRESSED);
            {
                lv_obj_t *parent_obj = obj;
                {
                    // label_limit_dec
                    lv_obj_t *obj = lv_label_create(parent_obj);
                    objects.label_limit_dec = obj;
                    lv_obj_set_pos(obj, 0, 0);
                    lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                    lv_obj_set_style_align(obj, LV_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_label_set_text_static(obj, "-0.1 A");
                }
            }
        }
        {
            // btn_limit_inc
            lv_obj_t *obj = lv_button_create(parent_obj);
            objects.btn_limit_inc = obj;
            lv_obj_set_pos(obj, 68, 161);
            lv_obj_set_size(obj, 56, 32);
            lv_obj_set_style_bg_color(obj, lv_color_hex(0x03dac5), LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_text_color(obj, lv_color_hex(0xffffff), LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_text_opa(obj, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_text_font(obj, &ui_font_amiko_12, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_radius(obj, 4, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_shadow_ofs_y(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_text_align(obj, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_PRESSED);
            {
                lv_obj_t *parent_obj = obj;
                {
                    // label_limit_inc
                    lv_obj_t *obj = lv_label_create(parent_obj);
                    objects.label_limit_inc = obj;
                    lv_obj_set_pos(obj, 0, 0);
                    lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                    lv_obj_set_style_align(obj, LV_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_text_color(obj, lv_color_hex(0x000000), LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_label_set_text_static(obj, "+0.1 A");
                }
            }
        }
        {
            // btn_limit_preset1
            lv_obj_t *obj = lv_button_create(parent_obj);
            objects.btn_limit_preset1 = obj;
            lv_obj_set_pos(obj, 136, 161);
            lv_obj_set_size(obj, 56, 32);
            lv_obj_set_style_bg_color(obj, lv_color_hex(0x1e293b), LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_text_color(obj, lv_color_hex(0xffffff), LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_text_opa(obj, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_text_font(obj, &ui_font_amiko_12, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_radius(obj, 4, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_shadow_ofs_y(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_text_align(obj, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_PRESSED);
            {
                lv_obj_t *parent_obj = obj;
                {
                    // label_limit_preset1
                    lv_obj_t *obj = lv_label_create(parent_obj);
                    objects.label_limit_preset1 = obj;
                    lv_obj_set_pos(obj, 0, 0);
                    lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                    lv_obj_set_style_align(obj, LV_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_text_color(obj, lv_color_hex(0xffffff), LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_label_set_text_static(obj, "1.0 A");
                }
            }
        }
        {
            // btn_limit_preset2
            lv_obj_t *obj = lv_button_create(parent_obj);
            objects.btn_limit_preset2 = obj;
            lv_obj_set_pos(obj, 197, 161);
            lv_obj_set_size(obj, 56, 32);
            lv_obj_set_style_bg_color(obj, lv_color_hex(0x1e293b), LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_text_color(obj, lv_color_hex(0xffffff), LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_text_opa(obj, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_text_font(obj, &ui_font_amiko_12, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_radius(obj, 4, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_shadow_ofs_y(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_text_align(obj, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_PRESSED);
            {
                lv_obj_t *parent_obj = obj;
                {
                    // label_limit_preset2
                    lv_obj_t *obj = lv_label_create(parent_obj);
                    objects.label_limit_preset2 = obj;
                    lv_obj_set_pos(obj, 0, 0);
                    lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                    lv_obj_set_style_align(obj, LV_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_text_color(obj, lv_color_hex(0xffffff), LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_label_set_text_static(obj, "3.0 A");
                }
            }
        }
        {
            // btn_limit_preset3
            lv_obj_t *obj = lv_button_create(parent_obj);
            objects.btn_limit_preset3 = obj;
            lv_obj_set_pos(obj, 258, 161);
            lv_obj_set_size(obj, 56, 32);
            lv_obj_set_style_bg_color(obj, lv_color_hex(0x1e293b), LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_text_color(obj, lv_color_hex(0xffffff), LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_text_opa(obj, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_text_font(obj, &ui_font_amiko_12, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_radius(obj, 4, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_shadow_ofs_y(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_text_align(obj, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_PRESSED);
            {
                lv_obj_t *parent_obj = obj;
                {
                    // label_limit_preset3
                    lv_obj_t *obj = lv_label_create(parent_obj);
                    objects.label_limit_preset3 = obj;
                    lv_obj_set_pos(obj, 0, 0);
                    lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                    lv_obj_set_style_align(obj, LV_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_text_color(obj, lv_color_hex(0xffffff), LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_label_set_text_static(obj, "6.0 A");
                }
            }
        }
    }
    
    tick_screen_limit_screen();
}

void tick_screen_limit_screen() {
}

void create_screen_reset_modal() {
    lv_obj_t *obj = lv_obj_create(0);
    objects.reset_modal = obj;
    lv_obj_set_pos(obj, 0, 0);
    lv_obj_set_size(obj, 320, 240);
    lv_obj_remove_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
    lv_obj_set_style_bg_color(obj, lv_color_hex(0x121212), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(obj, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_color(obj, lv_color_hex(0x121212), LV_PART_MAIN | LV_STATE_CHECKED);
    {
        lv_obj_t *parent_obj = obj;
        {
            // modal_rst_box
            lv_obj_t *obj = lv_obj_create(parent_obj);
            objects.modal_rst_box = obj;
            lv_obj_set_pos(obj, 6, 42);
            lv_obj_set_size(obj, 308, 145);
            lv_obj_set_style_pad_left(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_pad_top(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_pad_right(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_pad_bottom(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_border_width(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_remove_flag(obj, LV_OBJ_FLAG_SCROLLABLE|LV_OBJ_FLAG_SCROLL_ELASTIC);
            lv_obj_set_style_bg_color(obj, lv_color_hex(0x383838), LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_bg_opa(obj, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_radius(obj, 5, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_shadow_width(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_shadow_ofs_y(obj, 2, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_bg_color(obj, lv_color_hex(0x383838), LV_PART_MAIN | LV_STATE_HOVERED);
            lv_obj_set_style_bg_opa(obj, 255, LV_PART_MAIN | LV_STATE_HOVERED);
            lv_obj_set_style_border_color(obj, lv_color_hex(0xef5350), LV_PART_MAIN | LV_STATE_HOVERED);
            lv_obj_set_style_border_opa(obj, 255, LV_PART_MAIN | LV_STATE_HOVERED);
            lv_obj_set_style_border_width(obj, 2, LV_PART_MAIN | LV_STATE_HOVERED);
            {
                lv_obj_t *parent_obj = obj;
                {
                    // btn_modal_abort
                    lv_obj_t *obj = lv_button_create(parent_obj);
                    objects.btn_modal_abort = obj;
                    lv_obj_set_pos(obj, 9, 107);
                    lv_obj_set_size(obj, 152, 28);
                    lv_obj_set_style_bg_color(obj, lv_color_hex(0xef5350), LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_text_color(obj, lv_color_hex(0xffffff), LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_text_opa(obj, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_text_font(obj, &ui_font_amiko_12, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_radius(obj, 4, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_bg_opa(obj, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_shadow_ofs_y(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_text_align(obj, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_PRESSED);
                    {
                        lv_obj_t *parent_obj = obj;
                        {
                            // btn_label_rst_3
                            lv_obj_t *obj = lv_label_create(parent_obj);
                            objects.btn_label_rst_3 = obj;
                            lv_obj_set_pos(obj, 0, 0);
                            lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                            lv_obj_set_style_align(obj, LV_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_obj_set_style_text_color(obj, lv_color_hex(0xffffff), LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_label_set_text_static(obj, "No, abort");
                        }
                    }
                }
                {
                    // btn_modal_reset
                    lv_obj_t *obj = lv_button_create(parent_obj);
                    objects.btn_modal_reset = obj;
                    lv_obj_set_pos(obj, 171, 107);
                    lv_obj_set_size(obj, 129, 28);
                    lv_obj_set_style_bg_color(obj, lv_color_hex(0xa2ff67), LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_text_color(obj, lv_color_hex(0x000000), LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_text_opa(obj, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_text_font(obj, &ui_font_amiko_12, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_radius(obj, 4, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_shadow_ofs_y(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_text_align(obj, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_PRESSED);
                    {
                        lv_obj_t *parent_obj = obj;
                        {
                            // btn_label_config_12
                            lv_obj_t *obj = lv_label_create(parent_obj);
                            objects.btn_label_config_12 = obj;
                            lv_obj_set_pos(obj, 0, 0);
                            lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                            lv_obj_set_style_align(obj, LV_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_obj_set_style_text_color(obj, lv_color_hex(0x000000), LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_label_set_text_static(obj, "Yes, reset data");
                        }
                    }
                }
            }
        }
        {
            // lbl_modal_msg
            lv_obj_t *obj = lv_label_create(parent_obj);
            objects.lbl_modal_msg = obj;
            lv_obj_set_pos(obj, 52, 78);
            lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
            lv_obj_set_style_text_color(obj, lv_color_hex(0xffffff), LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_text_font(obj, &ui_font_amiko_12, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_align(obj, LV_ALIGN_DEFAULT, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_width(obj, 230, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_label_set_text_static(obj, "Do you want to reset the accumulated energy (Wh), capacity (mAh), and session time to zero?");
        }
        {
            // modal_rst_hdr
            lv_obj_t *obj = lv_obj_create(parent_obj);
            objects.modal_rst_hdr = obj;
            lv_obj_set_pos(obj, 6, 42);
            lv_obj_set_size(obj, 308, 23);
            lv_obj_set_style_pad_left(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_pad_top(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_pad_right(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_pad_bottom(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_border_width(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_bg_color(obj, lv_color_hex(0x272727), LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_bg_opa(obj, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_radius(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_clip_corner(obj, false, LV_PART_MAIN | LV_STATE_DEFAULT);
        }
    }
    
    tick_screen_reset_modal();
}

void tick_screen_reset_modal() {
}

void create_screen_ina228_adc_screen() {
    lv_obj_t *obj = lv_obj_create(0);
    objects.ina228_adc_screen = obj;
    lv_obj_set_pos(obj, 0, 0);
    lv_obj_set_size(obj, 320, 240);
    lv_obj_remove_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
    lv_obj_set_style_bg_color(obj, lv_color_hex(0x121212), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(obj, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
    {
        lv_obj_t *parent_obj = obj;
        {
            // ina228_adc_hdr
            lv_obj_t *obj = lv_obj_create(parent_obj);
            objects.ina228_adc_hdr = obj;
            lv_obj_set_pos(obj, 0, 4);
            lv_obj_set_size(obj, 320, 25);
            lv_obj_set_style_pad_left(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_pad_top(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_pad_right(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_pad_bottom(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_border_width(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_radius(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_remove_flag(obj, LV_OBJ_FLAG_SCROLLABLE|LV_OBJ_FLAG_SCROLL_CHAIN_HOR|LV_OBJ_FLAG_SCROLL_CHAIN_VER|LV_OBJ_FLAG_SCROLL_ELASTIC|LV_OBJ_FLAG_SCROLL_MOMENTUM|LV_OBJ_FLAG_SCROLL_WITH_ARROW);
            lv_obj_set_style_bg_color(obj, lv_color_hex(0x272727), LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_bg_opa(obj, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
            {
                lv_obj_t *parent_obj = obj;
                {
                    // lbl_ina228_adc_title
                    lv_obj_t *obj = lv_label_create(parent_obj);
                    objects.lbl_ina228_adc_title = obj;
                    lv_obj_set_pos(obj, 13, 6);
                    lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                    lv_obj_set_style_text_opa(obj, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_text_color(obj, lv_color_hex(0xffffff), LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_text_font(obj, &ui_font_amiko_12, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_label_set_text_static(obj, "INA228 - ADC SAMPLES");
                }
                {
                    // lbl_ina228_adc_page
                    lv_obj_t *obj = lv_label_create(parent_obj);
                    objects.lbl_ina228_adc_page = obj;
                    lv_obj_set_pos(obj, 274, 6);
                    lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                    lv_obj_set_style_text_color(obj, lv_color_hex(0xffffff), LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_text_font(obj, &ui_font_abee_zee_14, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_text_opa(obj, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_label_set_text_static(obj, "1/4");
                }
            }
        }
        {
            // box_adc_range
            lv_obj_t *obj = lv_obj_create(parent_obj);
            objects.box_adc_range = obj;
            lv_obj_set_pos(obj, 6, 38);
            lv_obj_set_size(obj, 308, 22);
            lv_obj_set_style_pad_left(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_pad_top(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_pad_right(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_pad_bottom(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_border_width(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_radius(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_remove_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
            lv_obj_set_style_bg_color(obj, lv_color_hex(0x272727), LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_bg_opa(obj, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_bg_color(obj, lv_color_hex(0x383838), LV_PART_MAIN | LV_STATE_FOCUSED);
            lv_obj_set_style_bg_opa(obj, 255, LV_PART_MAIN | LV_STATE_FOCUSED);
            lv_obj_set_style_bg_color(obj, lv_color_hex(0x383838), LV_PART_MAIN | LV_STATE_HOVERED);
            lv_obj_set_style_bg_opa(obj, 255, LV_PART_MAIN | LV_STATE_HOVERED);
            {
                lv_obj_t *parent_obj = obj;
                {
                    // btn_ina228_adc_edit1
                    lv_obj_t *obj = lv_button_create(parent_obj);
                    objects.btn_ina228_adc_edit1 = obj;
                    lv_obj_set_pos(obj, 251, 2);
                    lv_obj_set_size(obj, 52, 18);
                    lv_obj_set_style_bg_color(obj, lv_color_hex(0xffffff), LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_text_color(obj, lv_color_hex(0x000000), LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_text_opa(obj, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_text_font(obj, &ui_font_amiko_12, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_radius(obj, 4, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_shadow_ofs_y(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_text_align(obj, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_PRESSED);
                    {
                        lv_obj_t *parent_obj = obj;
                        {
                            // lbl_btn_adc_edit1
                            lv_obj_t *obj = lv_label_create(parent_obj);
                            objects.lbl_btn_adc_edit1 = obj;
                            lv_obj_set_pos(obj, 0, 0);
                            lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                            lv_obj_set_style_align(obj, LV_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_obj_set_style_text_font(obj, &ui_font_amiko_10, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_label_set_text_static(obj, "SET");
                        }
                    }
                }
                {
                    // lbl_val_adc_range
                    lv_obj_t *obj = lv_label_create(parent_obj);
                    objects.lbl_val_adc_range = obj;
                    lv_obj_set_pos(obj, 147, 5);
                    lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                    lv_obj_set_style_text_color(obj, lv_color_hex(0x03dac6), LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_text_font(obj, &ui_font_azeebee_12, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_text_opa(obj, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_label_set_text_static(obj, "+- 163.84 mV");
                }
                {
                    // lbl_ina228_adc_text1
                    lv_obj_t *obj = lv_label_create(parent_obj);
                    objects.lbl_ina228_adc_text1 = obj;
                    lv_obj_set_pos(obj, 7, 4);
                    lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                    lv_obj_set_style_text_color(obj, lv_color_hex(0xffffff), LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_text_opa(obj, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_text_font(obj, &ui_font_amiko_12, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_label_set_text_static(obj, "1. ADC Range");
                }
            }
        }
        {
            // btn_adc_cancel
            lv_obj_t *obj = lv_button_create(parent_obj);
            objects.btn_adc_cancel = obj;
            lv_obj_set_pos(obj, 6, 211);
            lv_obj_set_size(obj, 86, 24);
            lv_obj_set_style_bg_color(obj, lv_color_hex(0xef5350), LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_text_color(obj, lv_color_hex(0x000000), LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_text_opa(obj, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_text_font(obj, &ui_font_amiko_12, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_radius(obj, 4, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_shadow_ofs_y(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_bg_opa(obj, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_text_align(obj, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_PRESSED);
            lv_obj_set_style_bg_color(obj, lv_color_hex(0xef5350), LV_PART_MAIN | LV_STATE_CHECKED);
            {
                lv_obj_t *parent_obj = obj;
                {
                    // btn_label_adc_back
                    lv_obj_t *obj = lv_label_create(parent_obj);
                    objects.btn_label_adc_back = obj;
                    lv_obj_set_pos(obj, 0, 0);
                    lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                    lv_obj_set_style_align(obj, LV_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_text_color(obj, lv_color_hex(0xffffff), LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_label_set_text_static(obj, "CANCEL");
                }
            }
        }
        {
            // btn_adc_next
            lv_obj_t *obj = lv_button_create(parent_obj);
            objects.btn_adc_next = obj;
            lv_obj_set_pos(obj, 228, 211);
            lv_obj_set_size(obj, 86, 24);
            lv_obj_set_style_bg_color(obj, lv_color_hex(0xffffff), LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_text_color(obj, lv_color_hex(0x000000), LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_text_opa(obj, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_text_font(obj, &ui_font_amiko_12, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_radius(obj, 4, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_shadow_ofs_y(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_text_align(obj, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_PRESSED);
            {
                lv_obj_t *parent_obj = obj;
                {
                    // btn_label_adc_select
                    lv_obj_t *obj = lv_label_create(parent_obj);
                    objects.btn_label_adc_select = obj;
                    lv_obj_set_pos(obj, 0, 0);
                    lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                    lv_obj_set_style_align(obj, LV_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_label_set_text_static(obj, "NEXT");
                }
            }
        }
        {
            // btn_adc_save
            lv_obj_t *obj = lv_button_create(parent_obj);
            objects.btn_adc_save = obj;
            lv_obj_set_pos(obj, 115, 211);
            lv_obj_set_size(obj, 86, 24);
            lv_obj_set_style_bg_color(obj, lv_color_hex(0xa2ff67), LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_text_color(obj, lv_color_hex(0xffffff), LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_text_opa(obj, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_text_font(obj, &ui_font_amiko_12, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_radius(obj, 4, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_bg_opa(obj, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_shadow_ofs_y(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_text_align(obj, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_PRESSED);
            lv_obj_set_style_bg_color(obj, lv_color_hex(0xa2ff67), LV_PART_MAIN | LV_STATE_CHECKED);
            {
                lv_obj_t *parent_obj = obj;
                {
                    // btn_label_adc_enter
                    lv_obj_t *obj = lv_label_create(parent_obj);
                    objects.btn_label_adc_enter = obj;
                    lv_obj_set_pos(obj, 0, 0);
                    lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                    lv_obj_set_style_align(obj, LV_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_text_color(obj, lv_color_hex(0x000000), LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_text_opa(obj, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_text_font(obj, &ui_font_amiko_12, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_label_set_text_static(obj, "SAVE EXIT");
                }
            }
        }
        {
            // box_adc_mode
            lv_obj_t *obj = lv_obj_create(parent_obj);
            objects.box_adc_mode = obj;
            lv_obj_set_pos(obj, 6, 62);
            lv_obj_set_size(obj, 308, 22);
            lv_obj_set_style_pad_left(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_pad_top(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_pad_right(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_pad_bottom(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_border_width(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_radius(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_remove_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
            lv_obj_set_style_bg_color(obj, lv_color_hex(0x272727), LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_bg_opa(obj, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_bg_color(obj, lv_color_hex(0x383838), LV_PART_MAIN | LV_STATE_FOCUSED);
            lv_obj_set_style_bg_opa(obj, 255, LV_PART_MAIN | LV_STATE_FOCUSED);
            lv_obj_set_style_bg_color(obj, lv_color_hex(0x383838), LV_PART_MAIN | LV_STATE_HOVERED);
            lv_obj_set_style_bg_opa(obj, 255, LV_PART_MAIN | LV_STATE_HOVERED);
            {
                lv_obj_t *parent_obj = obj;
                {
                    // btn_ina228_adc_edit2
                    lv_obj_t *obj = lv_button_create(parent_obj);
                    objects.btn_ina228_adc_edit2 = obj;
                    lv_obj_set_pos(obj, 251, 2);
                    lv_obj_set_size(obj, 52, 18);
                    lv_obj_set_style_bg_color(obj, lv_color_hex(0xffffff), LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_text_color(obj, lv_color_hex(0x000000), LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_text_opa(obj, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_text_font(obj, &ui_font_amiko_12, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_radius(obj, 4, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_shadow_ofs_y(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_text_align(obj, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_PRESSED);
                    {
                        lv_obj_t *parent_obj = obj;
                        {
                            // lbl_btn_adc_edit2
                            lv_obj_t *obj = lv_label_create(parent_obj);
                            objects.lbl_btn_adc_edit2 = obj;
                            lv_obj_set_pos(obj, 0, 0);
                            lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                            lv_obj_set_style_align(obj, LV_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_obj_set_style_text_font(obj, &ui_font_amiko_10, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_label_set_text_static(obj, "SET");
                        }
                    }
                }
                {
                    // lbl_val_adc_mode
                    lv_obj_t *obj = lv_label_create(parent_obj);
                    objects.lbl_val_adc_mode = obj;
                    lv_obj_set_pos(obj, 147, 5);
                    lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                    lv_obj_set_style_text_color(obj, lv_color_hex(0x03dac6), LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_text_font(obj, &ui_font_amiko_12, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_text_opa(obj, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_label_set_text_static(obj, "CONT: V + I + T");
                }
                {
                    // lbl_ina228_adc_text2
                    lv_obj_t *obj = lv_label_create(parent_obj);
                    objects.lbl_ina228_adc_text2 = obj;
                    lv_obj_set_pos(obj, 7, 4);
                    lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                    lv_obj_set_style_text_color(obj, lv_color_hex(0xffffff), LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_text_opa(obj, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_text_font(obj, &ui_font_amiko_12, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_label_set_text_static(obj, "2. Mode");
                }
            }
        }
        {
            // box_adc_avg
            lv_obj_t *obj = lv_obj_create(parent_obj);
            objects.box_adc_avg = obj;
            lv_obj_set_pos(obj, 6, 86);
            lv_obj_set_size(obj, 308, 22);
            lv_obj_set_style_pad_left(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_pad_top(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_pad_right(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_pad_bottom(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_border_width(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_radius(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_remove_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
            lv_obj_set_style_bg_color(obj, lv_color_hex(0x272727), LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_bg_opa(obj, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_bg_color(obj, lv_color_hex(0x383838), LV_PART_MAIN | LV_STATE_FOCUSED);
            lv_obj_set_style_bg_opa(obj, 255, LV_PART_MAIN | LV_STATE_FOCUSED);
            lv_obj_set_style_bg_color(obj, lv_color_hex(0x383838), LV_PART_MAIN | LV_STATE_HOVERED);
            lv_obj_set_style_bg_opa(obj, 255, LV_PART_MAIN | LV_STATE_HOVERED);
            {
                lv_obj_t *parent_obj = obj;
                {
                    // btn_ina228_adc_edit3
                    lv_obj_t *obj = lv_button_create(parent_obj);
                    objects.btn_ina228_adc_edit3 = obj;
                    lv_obj_set_pos(obj, 251, 2);
                    lv_obj_set_size(obj, 52, 18);
                    lv_obj_set_style_bg_color(obj, lv_color_hex(0xffffff), LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_text_color(obj, lv_color_hex(0x000000), LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_text_opa(obj, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_text_font(obj, &ui_font_amiko_12, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_radius(obj, 4, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_shadow_ofs_y(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_text_align(obj, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_PRESSED);
                    {
                        lv_obj_t *parent_obj = obj;
                        {
                            // lbl_btn_adc_edit3
                            lv_obj_t *obj = lv_label_create(parent_obj);
                            objects.lbl_btn_adc_edit3 = obj;
                            lv_obj_set_pos(obj, 0, 0);
                            lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                            lv_obj_set_style_align(obj, LV_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_obj_set_style_text_font(obj, &ui_font_amiko_10, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_label_set_text_static(obj, "SET");
                        }
                    }
                }
                {
                    // lbl_val_adc_samples
                    lv_obj_t *obj = lv_label_create(parent_obj);
                    objects.lbl_val_adc_samples = obj;
                    lv_obj_set_pos(obj, 147, 5);
                    lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                    lv_obj_set_style_text_color(obj, lv_color_hex(0x03dac6), LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_text_font(obj, &ui_font_amiko_12, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_text_opa(obj, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_label_set_text_static(obj, "64 samples");
                }
                {
                    // lbl_ina228_adc_text3
                    lv_obj_t *obj = lv_label_create(parent_obj);
                    objects.lbl_ina228_adc_text3 = obj;
                    lv_obj_set_pos(obj, 7, 4);
                    lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                    lv_obj_set_style_text_color(obj, lv_color_hex(0xffffff), LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_text_opa(obj, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_text_font(obj, &ui_font_amiko_12, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_label_set_text_static(obj, "3. Average (AVG)");
                }
            }
        }
        {
            // box_adc_convdelay
            lv_obj_t *obj = lv_obj_create(parent_obj);
            objects.box_adc_convdelay = obj;
            lv_obj_set_pos(obj, 6, 110);
            lv_obj_set_size(obj, 308, 22);
            lv_obj_set_style_pad_left(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_pad_top(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_pad_right(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_pad_bottom(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_border_width(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_radius(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_remove_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
            lv_obj_set_style_bg_color(obj, lv_color_hex(0x272727), LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_bg_opa(obj, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_bg_color(obj, lv_color_hex(0x383838), LV_PART_MAIN | LV_STATE_FOCUSED);
            lv_obj_set_style_bg_opa(obj, 255, LV_PART_MAIN | LV_STATE_FOCUSED);
            lv_obj_set_style_bg_color(obj, lv_color_hex(0x383838), LV_PART_MAIN | LV_STATE_HOVERED);
            lv_obj_set_style_bg_opa(obj, 255, LV_PART_MAIN | LV_STATE_HOVERED);
            {
                lv_obj_t *parent_obj = obj;
                {
                    // btn_ina228_adc_edit4
                    lv_obj_t *obj = lv_button_create(parent_obj);
                    objects.btn_ina228_adc_edit4 = obj;
                    lv_obj_set_pos(obj, 251, 2);
                    lv_obj_set_size(obj, 52, 18);
                    lv_obj_set_style_bg_color(obj, lv_color_hex(0xffffff), LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_text_color(obj, lv_color_hex(0x000000), LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_text_opa(obj, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_text_font(obj, &ui_font_amiko_12, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_radius(obj, 4, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_shadow_ofs_y(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_text_align(obj, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_PRESSED);
                    {
                        lv_obj_t *parent_obj = obj;
                        {
                            // lbl_btn_adc_edit4
                            lv_obj_t *obj = lv_label_create(parent_obj);
                            objects.lbl_btn_adc_edit4 = obj;
                            lv_obj_set_pos(obj, 0, 0);
                            lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                            lv_obj_set_style_align(obj, LV_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_obj_set_style_text_font(obj, &ui_font_amiko_10, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_label_set_text_static(obj, "SET");
                        }
                    }
                }
                {
                    // lbl_val_adc_convdelay
                    lv_obj_t *obj = lv_label_create(parent_obj);
                    objects.lbl_val_adc_convdelay = obj;
                    lv_obj_set_pos(obj, 147, 5);
                    lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                    lv_obj_set_style_text_color(obj, lv_color_hex(0x03dac6), LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_text_font(obj, &ui_font_amiko_12, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_text_opa(obj, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_label_set_text_static(obj, "0 ms");
                }
                {
                    // lbl_ina228_adc_text4
                    lv_obj_t *obj = lv_label_create(parent_obj);
                    objects.lbl_ina228_adc_text4 = obj;
                    lv_obj_set_pos(obj, 7, 4);
                    lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                    lv_obj_set_style_text_color(obj, lv_color_hex(0xffffff), LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_text_opa(obj, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_text_font(obj, &ui_font_amiko_12, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_label_set_text_static(obj, "4. Conv Delay");
                }
            }
        }
        {
            // box_adc_vbus_time
            lv_obj_t *obj = lv_obj_create(parent_obj);
            objects.box_adc_vbus_time = obj;
            lv_obj_set_pos(obj, 6, 134);
            lv_obj_set_size(obj, 308, 22);
            lv_obj_set_style_pad_left(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_pad_top(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_pad_right(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_pad_bottom(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_border_width(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_radius(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_remove_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
            lv_obj_set_style_bg_color(obj, lv_color_hex(0x272727), LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_bg_opa(obj, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_bg_color(obj, lv_color_hex(0x383838), LV_PART_MAIN | LV_STATE_FOCUSED);
            lv_obj_set_style_bg_opa(obj, 255, LV_PART_MAIN | LV_STATE_FOCUSED);
            lv_obj_set_style_bg_color(obj, lv_color_hex(0x383838), LV_PART_MAIN | LV_STATE_HOVERED);
            lv_obj_set_style_bg_opa(obj, 255, LV_PART_MAIN | LV_STATE_HOVERED);
            {
                lv_obj_t *parent_obj = obj;
                {
                    // btn_ina228_adc_edit5
                    lv_obj_t *obj = lv_button_create(parent_obj);
                    objects.btn_ina228_adc_edit5 = obj;
                    lv_obj_set_pos(obj, 251, 2);
                    lv_obj_set_size(obj, 52, 18);
                    lv_obj_set_style_bg_color(obj, lv_color_hex(0xffffff), LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_text_color(obj, lv_color_hex(0x000000), LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_text_opa(obj, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_text_font(obj, &ui_font_amiko_12, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_radius(obj, 4, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_shadow_ofs_y(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_text_align(obj, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_PRESSED);
                    {
                        lv_obj_t *parent_obj = obj;
                        {
                            // lbl_btn_adc_edit5
                            lv_obj_t *obj = lv_label_create(parent_obj);
                            objects.lbl_btn_adc_edit5 = obj;
                            lv_obj_set_pos(obj, 0, 0);
                            lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                            lv_obj_set_style_align(obj, LV_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_obj_set_style_text_font(obj, &ui_font_amiko_10, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_label_set_text_static(obj, "SET");
                        }
                    }
                }
                {
                    // lbl_val_adc_vbus_time
                    lv_obj_t *obj = lv_label_create(parent_obj);
                    objects.lbl_val_adc_vbus_time = obj;
                    lv_obj_set_pos(obj, 147, 5);
                    lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                    lv_obj_set_style_text_color(obj, lv_color_hex(0x03dac6), LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_text_font(obj, &ui_font_amiko_12, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_text_opa(obj, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_label_set_text_static(obj, "540 us");
                }
                {
                    // lbl_ina228_adc_text5
                    lv_obj_t *obj = lv_label_create(parent_obj);
                    objects.lbl_ina228_adc_text5 = obj;
                    lv_obj_set_pos(obj, 7, 4);
                    lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                    lv_obj_set_style_text_color(obj, lv_color_hex(0xffffff), LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_text_opa(obj, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_text_font(obj, &ui_font_amiko_12, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_label_set_text_static(obj, "5. VBUS Time");
                }
            }
        }
        {
            // box_adc_vshunt_time
            lv_obj_t *obj = lv_obj_create(parent_obj);
            objects.box_adc_vshunt_time = obj;
            lv_obj_set_pos(obj, 6, 158);
            lv_obj_set_size(obj, 308, 22);
            lv_obj_set_style_pad_left(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_pad_top(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_pad_right(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_pad_bottom(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_border_width(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_radius(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_remove_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
            lv_obj_set_style_bg_color(obj, lv_color_hex(0x272727), LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_bg_opa(obj, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_bg_color(obj, lv_color_hex(0x383838), LV_PART_MAIN | LV_STATE_FOCUSED);
            lv_obj_set_style_bg_opa(obj, 255, LV_PART_MAIN | LV_STATE_FOCUSED);
            lv_obj_set_style_bg_color(obj, lv_color_hex(0x383838), LV_PART_MAIN | LV_STATE_HOVERED);
            lv_obj_set_style_bg_opa(obj, 255, LV_PART_MAIN | LV_STATE_HOVERED);
            {
                lv_obj_t *parent_obj = obj;
                {
                    // btn_ina228_adc_edit6
                    lv_obj_t *obj = lv_button_create(parent_obj);
                    objects.btn_ina228_adc_edit6 = obj;
                    lv_obj_set_pos(obj, 251, 2);
                    lv_obj_set_size(obj, 52, 18);
                    lv_obj_set_style_bg_color(obj, lv_color_hex(0xffffff), LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_text_color(obj, lv_color_hex(0x000000), LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_text_opa(obj, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_text_font(obj, &ui_font_amiko_12, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_radius(obj, 4, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_shadow_ofs_y(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_text_align(obj, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_PRESSED);
                    {
                        lv_obj_t *parent_obj = obj;
                        {
                            // lbl_btn_adc_edit6
                            lv_obj_t *obj = lv_label_create(parent_obj);
                            objects.lbl_btn_adc_edit6 = obj;
                            lv_obj_set_pos(obj, 0, 0);
                            lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                            lv_obj_set_style_align(obj, LV_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_obj_set_style_text_font(obj, &ui_font_amiko_10, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_label_set_text_static(obj, "SET");
                        }
                    }
                }
                {
                    // lbl_val_adc_vshunt_time
                    lv_obj_t *obj = lv_label_create(parent_obj);
                    objects.lbl_val_adc_vshunt_time = obj;
                    lv_obj_set_pos(obj, 147, 5);
                    lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                    lv_obj_set_style_text_color(obj, lv_color_hex(0x03dac6), LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_text_font(obj, &ui_font_amiko_12, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_text_opa(obj, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_label_set_text_static(obj, "540 us");
                }
                {
                    // lbl_ina228_adc_text6
                    lv_obj_t *obj = lv_label_create(parent_obj);
                    objects.lbl_ina228_adc_text6 = obj;
                    lv_obj_set_pos(obj, 7, 4);
                    lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                    lv_obj_set_style_text_color(obj, lv_color_hex(0xffffff), LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_text_opa(obj, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_text_font(obj, &ui_font_amiko_12, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_label_set_text_static(obj, "6. VSHUNT Time");
                }
            }
        }
        {
            // box_adc_temp_time
            lv_obj_t *obj = lv_obj_create(parent_obj);
            objects.box_adc_temp_time = obj;
            lv_obj_set_pos(obj, 6, 182);
            lv_obj_set_size(obj, 308, 22);
            lv_obj_set_style_pad_left(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_pad_top(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_pad_right(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_pad_bottom(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_border_width(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_radius(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_remove_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
            lv_obj_set_style_bg_color(obj, lv_color_hex(0x272727), LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_bg_opa(obj, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_bg_color(obj, lv_color_hex(0x383838), LV_PART_MAIN | LV_STATE_FOCUSED);
            lv_obj_set_style_bg_opa(obj, 255, LV_PART_MAIN | LV_STATE_FOCUSED);
            lv_obj_set_style_bg_color(obj, lv_color_hex(0x383838), LV_PART_MAIN | LV_STATE_HOVERED);
            lv_obj_set_style_bg_opa(obj, 255, LV_PART_MAIN | LV_STATE_HOVERED);
            {
                lv_obj_t *parent_obj = obj;
                {
                    // btn_ina228_adc_edit7
                    lv_obj_t *obj = lv_button_create(parent_obj);
                    objects.btn_ina228_adc_edit7 = obj;
                    lv_obj_set_pos(obj, 251, 2);
                    lv_obj_set_size(obj, 52, 18);
                    lv_obj_set_style_bg_color(obj, lv_color_hex(0xffffff), LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_text_color(obj, lv_color_hex(0x000000), LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_text_opa(obj, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_text_font(obj, &ui_font_amiko_12, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_radius(obj, 4, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_shadow_ofs_y(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_text_align(obj, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_PRESSED);
                    {
                        lv_obj_t *parent_obj = obj;
                        {
                            // lbl_btn_adc_edit7
                            lv_obj_t *obj = lv_label_create(parent_obj);
                            objects.lbl_btn_adc_edit7 = obj;
                            lv_obj_set_pos(obj, 0, 0);
                            lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                            lv_obj_set_style_align(obj, LV_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_obj_set_style_text_font(obj, &ui_font_amiko_10, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_label_set_text_static(obj, "SET");
                        }
                    }
                }
                {
                    // lbl_val_adc_temp_time
                    lv_obj_t *obj = lv_label_create(parent_obj);
                    objects.lbl_val_adc_temp_time = obj;
                    lv_obj_set_pos(obj, 147, 5);
                    lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                    lv_obj_set_style_text_color(obj, lv_color_hex(0x03dac6), LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_text_font(obj, &ui_font_amiko_12, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_text_opa(obj, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_label_set_text_static(obj, "540 us");
                }
                {
                    // lbl_ina228_adc_text7
                    lv_obj_t *obj = lv_label_create(parent_obj);
                    objects.lbl_ina228_adc_text7 = obj;
                    lv_obj_set_pos(obj, 7, 4);
                    lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                    lv_obj_set_style_text_color(obj, lv_color_hex(0xffffff), LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_text_opa(obj, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_text_font(obj, &ui_font_amiko_12, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_label_set_text_static(obj, "7. TEMP Time");
                }
            }
        }
    }
    
    tick_screen_ina228_adc_screen();
}

void tick_screen_ina228_adc_screen() {
}

void create_screen_ina228_cal_screen() {
    lv_obj_t *obj = lv_obj_create(0);
    objects.ina228_cal_screen = obj;
    lv_obj_set_pos(obj, 0, 0);
    lv_obj_set_size(obj, 320, 240);
    lv_obj_remove_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
    lv_obj_set_style_bg_color(obj, lv_color_hex(0x121212), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(obj, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
    {
        lv_obj_t *parent_obj = obj;
        {
            // ina228_cal_hdr
            lv_obj_t *obj = lv_obj_create(parent_obj);
            objects.ina228_cal_hdr = obj;
            lv_obj_set_pos(obj, 0, 4);
            lv_obj_set_size(obj, 320, 25);
            lv_obj_set_style_pad_left(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_pad_top(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_pad_right(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_pad_bottom(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_border_width(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_radius(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_remove_flag(obj, LV_OBJ_FLAG_SCROLLABLE|LV_OBJ_FLAG_SCROLL_CHAIN_HOR|LV_OBJ_FLAG_SCROLL_CHAIN_VER|LV_OBJ_FLAG_SCROLL_ELASTIC|LV_OBJ_FLAG_SCROLL_MOMENTUM|LV_OBJ_FLAG_SCROLL_WITH_ARROW);
            lv_obj_set_style_bg_color(obj, lv_color_hex(0x272727), LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_bg_opa(obj, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
            {
                lv_obj_t *parent_obj = obj;
                {
                    // lbl_ina228_cal_title
                    lv_obj_t *obj = lv_label_create(parent_obj);
                    objects.lbl_ina228_cal_title = obj;
                    lv_obj_set_pos(obj, 13, 6);
                    lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                    lv_obj_set_style_text_opa(obj, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_text_color(obj, lv_color_hex(0xffffff), LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_text_font(obj, &ui_font_amiko_12, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_label_set_text_static(obj, "INA228 - CALIBRATION SHUNT");
                }
                {
                    // lbl_ina228_cal_page
                    lv_obj_t *obj = lv_label_create(parent_obj);
                    objects.lbl_ina228_cal_page = obj;
                    lv_obj_set_pos(obj, 274, 6);
                    lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                    lv_obj_set_style_text_color(obj, lv_color_hex(0xffffff), LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_text_font(obj, &ui_font_abee_zee_14, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_text_opa(obj, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_label_set_text_static(obj, "2/4");
                }
            }
        }
        {
            // box_cal_shunt_res
            lv_obj_t *obj = lv_obj_create(parent_obj);
            objects.box_cal_shunt_res = obj;
            lv_obj_set_pos(obj, 6, 38);
            lv_obj_set_size(obj, 308, 25);
            lv_obj_set_style_pad_left(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_pad_top(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_pad_right(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_pad_bottom(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_border_width(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_radius(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_remove_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
            lv_obj_set_style_bg_color(obj, lv_color_hex(0x272727), LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_bg_opa(obj, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_bg_color(obj, lv_color_hex(0x383838), LV_PART_MAIN | LV_STATE_FOCUSED);
            lv_obj_set_style_bg_opa(obj, 255, LV_PART_MAIN | LV_STATE_FOCUSED);
            lv_obj_set_style_bg_color(obj, lv_color_hex(0x383838), LV_PART_MAIN | LV_STATE_HOVERED);
            lv_obj_set_style_bg_opa(obj, 255, LV_PART_MAIN | LV_STATE_HOVERED);
            {
                lv_obj_t *parent_obj = obj;
                {
                    // lbl_unit_shunt_res
                    lv_obj_t *obj = lv_label_create(parent_obj);
                    objects.lbl_unit_shunt_res = obj;
                    lv_obj_set_pos(obj, 187, 6);
                    lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                    lv_obj_set_style_text_color(obj, lv_color_hex(0x03dac6), LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_text_font(obj, &ui_font_azeebee_12, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_text_opa(obj, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_label_set_text_static(obj, "mOhm");
                }
                {
                    // btn_ina228_cal_edit1
                    lv_obj_t *obj = lv_button_create(parent_obj);
                    objects.btn_ina228_cal_edit1 = obj;
                    lv_obj_set_pos(obj, 251, 2);
                    lv_obj_set_size(obj, 52, 20);
                    lv_obj_set_style_bg_color(obj, lv_color_hex(0xffffff), LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_text_color(obj, lv_color_hex(0x000000), LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_text_opa(obj, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_text_font(obj, &ui_font_amiko_12, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_radius(obj, 4, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_shadow_ofs_y(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_text_align(obj, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_PRESSED);
                    {
                        lv_obj_t *parent_obj = obj;
                        {
                            // lbl_btn_adc_edit1_1
                            lv_obj_t *obj = lv_label_create(parent_obj);
                            objects.lbl_btn_adc_edit1_1 = obj;
                            lv_obj_set_pos(obj, 0, 0);
                            lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                            lv_obj_set_style_align(obj, LV_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_obj_set_style_text_font(obj, &ui_font_amiko_10, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_label_set_text_static(obj, "SET");
                        }
                    }
                }
                {
                    // lbl_val_shunt_res
                    lv_obj_t *obj = lv_label_create(parent_obj);
                    objects.lbl_val_shunt_res = obj;
                    lv_obj_set_pos(obj, 141, 6);
                    lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                    lv_obj_set_style_text_color(obj, lv_color_hex(0x03dac6), LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_text_font(obj, &ui_font_azeebee_12, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_text_opa(obj, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_label_set_text_static(obj, "5.0000");
                }
                {
                    // lbl_ina228_cal_text1
                    lv_obj_t *obj = lv_label_create(parent_obj);
                    objects.lbl_ina228_cal_text1 = obj;
                    lv_obj_set_pos(obj, 7, 6);
                    lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                    lv_obj_set_style_text_color(obj, lv_color_hex(0xffffff), LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_text_opa(obj, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_text_font(obj, &ui_font_amiko_12, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_label_set_text_static(obj, "1. Shunt Resistance");
                }
            }
        }
        {
            // btn_cal_prev
            lv_obj_t *obj = lv_button_create(parent_obj);
            objects.btn_cal_prev = obj;
            lv_obj_set_pos(obj, 6, 207);
            lv_obj_set_size(obj, 86, 28);
            lv_obj_set_style_bg_color(obj, lv_color_hex(0xef5350), LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_text_color(obj, lv_color_hex(0x000000), LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_text_opa(obj, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_text_font(obj, &ui_font_amiko_12, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_radius(obj, 4, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_shadow_ofs_y(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_bg_opa(obj, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_text_align(obj, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_PRESSED);
            lv_obj_set_style_bg_color(obj, lv_color_hex(0xef5350), LV_PART_MAIN | LV_STATE_CHECKED);
            {
                lv_obj_t *parent_obj = obj;
                {
                    // btn_label_adc_back_1
                    lv_obj_t *obj = lv_label_create(parent_obj);
                    objects.btn_label_adc_back_1 = obj;
                    lv_obj_set_pos(obj, 0, 0);
                    lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                    lv_obj_set_style_align(obj, LV_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_text_color(obj, lv_color_hex(0xffffff), LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_label_set_text_static(obj, "< PREV");
                }
            }
        }
        {
            // btn_cal_next
            lv_obj_t *obj = lv_button_create(parent_obj);
            objects.btn_cal_next = obj;
            lv_obj_set_pos(obj, 228, 207);
            lv_obj_set_size(obj, 86, 28);
            lv_obj_set_style_bg_color(obj, lv_color_hex(0xffffff), LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_text_color(obj, lv_color_hex(0x000000), LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_text_opa(obj, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_text_font(obj, &ui_font_amiko_12, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_radius(obj, 4, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_shadow_ofs_y(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_text_align(obj, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_PRESSED);
            {
                lv_obj_t *parent_obj = obj;
                {
                    // btn_label_adc_select_1
                    lv_obj_t *obj = lv_label_create(parent_obj);
                    objects.btn_label_adc_select_1 = obj;
                    lv_obj_set_pos(obj, 0, 0);
                    lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                    lv_obj_set_style_align(obj, LV_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_label_set_text_static(obj, "NEXT >");
                }
            }
        }
        {
            // btn_cal_save
            lv_obj_t *obj = lv_button_create(parent_obj);
            objects.btn_cal_save = obj;
            lv_obj_set_pos(obj, 117, 207);
            lv_obj_set_size(obj, 86, 28);
            lv_obj_set_style_bg_color(obj, lv_color_hex(0xa2ff67), LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_text_color(obj, lv_color_hex(0xffffff), LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_text_opa(obj, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_text_font(obj, &ui_font_amiko_12, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_radius(obj, 4, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_bg_opa(obj, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_shadow_ofs_y(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_text_align(obj, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_PRESSED);
            lv_obj_set_style_bg_color(obj, lv_color_hex(0xa2ff67), LV_PART_MAIN | LV_STATE_CHECKED);
            {
                lv_obj_t *parent_obj = obj;
                {
                    // btn_label_adc_enter_1
                    lv_obj_t *obj = lv_label_create(parent_obj);
                    objects.btn_label_adc_enter_1 = obj;
                    lv_obj_set_pos(obj, 0, 0);
                    lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                    lv_obj_set_style_align(obj, LV_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_text_color(obj, lv_color_hex(0x000000), LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_text_opa(obj, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_text_font(obj, &ui_font_amiko_12, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_label_set_text_static(obj, "SAVE & EXIT");
                }
            }
        }
        {
            // box_cal_max_current
            lv_obj_t *obj = lv_obj_create(parent_obj);
            objects.box_cal_max_current = obj;
            lv_obj_set_pos(obj, 6, 65);
            lv_obj_set_size(obj, 308, 25);
            lv_obj_set_style_pad_left(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_pad_top(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_pad_right(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_pad_bottom(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_border_width(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_radius(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_remove_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
            lv_obj_set_style_bg_color(obj, lv_color_hex(0x272727), LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_bg_opa(obj, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_bg_color(obj, lv_color_hex(0x383838), LV_PART_MAIN | LV_STATE_FOCUSED);
            lv_obj_set_style_bg_opa(obj, 255, LV_PART_MAIN | LV_STATE_FOCUSED);
            lv_obj_set_style_bg_color(obj, lv_color_hex(0x383838), LV_PART_MAIN | LV_STATE_HOVERED);
            lv_obj_set_style_bg_opa(obj, 255, LV_PART_MAIN | LV_STATE_HOVERED);
            {
                lv_obj_t *parent_obj = obj;
                {
                    // lbl_unit_max_curr
                    lv_obj_t *obj = lv_label_create(parent_obj);
                    objects.lbl_unit_max_curr = obj;
                    lv_obj_set_pos(obj, 187, 6);
                    lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                    lv_obj_set_style_text_color(obj, lv_color_hex(0x03dac6), LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_text_font(obj, &ui_font_azeebee_12, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_text_opa(obj, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_label_set_text_static(obj, "A");
                }
                {
                    // btn_ina228_cal_edit2
                    lv_obj_t *obj = lv_button_create(parent_obj);
                    objects.btn_ina228_cal_edit2 = obj;
                    lv_obj_set_pos(obj, 251, 2);
                    lv_obj_set_size(obj, 52, 20);
                    lv_obj_set_style_bg_color(obj, lv_color_hex(0xffffff), LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_text_color(obj, lv_color_hex(0x000000), LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_text_opa(obj, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_text_font(obj, &ui_font_amiko_12, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_radius(obj, 4, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_shadow_ofs_y(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_text_align(obj, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_PRESSED);
                    {
                        lv_obj_t *parent_obj = obj;
                        {
                            // lbl_btn_adc_edit1_2
                            lv_obj_t *obj = lv_label_create(parent_obj);
                            objects.lbl_btn_adc_edit1_2 = obj;
                            lv_obj_set_pos(obj, 0, 0);
                            lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                            lv_obj_set_style_align(obj, LV_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_obj_set_style_text_font(obj, &ui_font_amiko_10, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_label_set_text_static(obj, "SET");
                        }
                    }
                }
                {
                    // lbl_val_max_curr
                    lv_obj_t *obj = lv_label_create(parent_obj);
                    objects.lbl_val_max_curr = obj;
                    lv_obj_set_pos(obj, 141, 6);
                    lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                    lv_obj_set_style_text_color(obj, lv_color_hex(0x03dac6), LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_text_font(obj, &ui_font_azeebee_12, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_text_opa(obj, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_label_set_text_static(obj, "6.0000");
                }
                {
                    // lbl_ina228_cal_text2
                    lv_obj_t *obj = lv_label_create(parent_obj);
                    objects.lbl_ina228_cal_text2 = obj;
                    lv_obj_set_pos(obj, 7, 6);
                    lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                    lv_obj_set_style_text_color(obj, lv_color_hex(0xffffff), LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_text_opa(obj, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_text_font(obj, &ui_font_amiko_12, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_label_set_text_static(obj, "2. Max Current");
                }
            }
        }
        {
            // box_cal_temp_comp
            lv_obj_t *obj = lv_obj_create(parent_obj);
            objects.box_cal_temp_comp = obj;
            lv_obj_set_pos(obj, 6, 92);
            lv_obj_set_size(obj, 308, 25);
            lv_obj_set_style_pad_left(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_pad_top(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_pad_right(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_pad_bottom(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_border_width(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_radius(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_remove_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
            lv_obj_set_style_bg_color(obj, lv_color_hex(0x272727), LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_bg_opa(obj, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_bg_color(obj, lv_color_hex(0x383838), LV_PART_MAIN | LV_STATE_FOCUSED);
            lv_obj_set_style_bg_opa(obj, 255, LV_PART_MAIN | LV_STATE_FOCUSED);
            lv_obj_set_style_bg_color(obj, lv_color_hex(0x383838), LV_PART_MAIN | LV_STATE_HOVERED);
            lv_obj_set_style_bg_opa(obj, 255, LV_PART_MAIN | LV_STATE_HOVERED);
            {
                lv_obj_t *parent_obj = obj;
                {
                    // lbl_val_temp_comp
                    lv_obj_t *obj = lv_label_create(parent_obj);
                    objects.lbl_val_temp_comp = obj;
                    lv_obj_set_pos(obj, 141, 6);
                    lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                    lv_obj_set_style_text_color(obj, lv_color_hex(0x03dac6), LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_text_font(obj, &ui_font_azeebee_12, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_text_opa(obj, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_label_set_text_static(obj, "OFF");
                }
                {
                    // lbl_ina228_cal_text3
                    lv_obj_t *obj = lv_label_create(parent_obj);
                    objects.lbl_ina228_cal_text3 = obj;
                    lv_obj_set_pos(obj, 7, 6);
                    lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                    lv_obj_set_style_text_color(obj, lv_color_hex(0xffffff), LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_text_opa(obj, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_text_font(obj, &ui_font_amiko_12, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_label_set_text_static(obj, "3. Temp. Comp.");
                }
                {
                    // switch_cal
                    lv_obj_t *obj = lv_switch_create(parent_obj);
                    objects.switch_cal = obj;
                    lv_obj_set_pos(obj, 252, 2);
                    lv_obj_set_size(obj, 50, 20);
                    lv_obj_set_style_bg_color(obj, lv_color_hex(0xbb86fc), LV_PART_INDICATOR | LV_STATE_CHECKED);
                }
            }
        }
        {
            // box_cal_shunt_cal
            lv_obj_t *obj = lv_obj_create(parent_obj);
            objects.box_cal_shunt_cal = obj;
            lv_obj_set_pos(obj, 6, 119);
            lv_obj_set_size(obj, 308, 25);
            lv_obj_set_style_pad_left(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_pad_top(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_pad_right(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_pad_bottom(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_border_width(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_radius(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_remove_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
            lv_obj_set_style_bg_color(obj, lv_color_hex(0x272727), LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_bg_opa(obj, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_bg_color(obj, lv_color_hex(0x383838), LV_PART_MAIN | LV_STATE_FOCUSED);
            lv_obj_set_style_bg_opa(obj, 255, LV_PART_MAIN | LV_STATE_FOCUSED);
            lv_obj_set_style_bg_color(obj, lv_color_hex(0x383838), LV_PART_MAIN | LV_STATE_HOVERED);
            lv_obj_set_style_bg_opa(obj, 255, LV_PART_MAIN | LV_STATE_HOVERED);
            {
                lv_obj_t *parent_obj = obj;
                {
                    // lbl_val_shunt_cal
                    lv_obj_t *obj = lv_label_create(parent_obj);
                    objects.lbl_val_shunt_cal = obj;
                    lv_obj_set_pos(obj, 141, 6);
                    lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                    lv_obj_set_style_text_color(obj, lv_color_hex(0x03dac6), LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_text_font(obj, &ui_font_azeebee_12, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_text_opa(obj, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_label_set_text_static(obj, "0x0640");
                }
                {
                    // lbl_ina228_cal_text4
                    lv_obj_t *obj = lv_label_create(parent_obj);
                    objects.lbl_ina228_cal_text4 = obj;
                    lv_obj_set_pos(obj, 7, 6);
                    lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                    lv_obj_set_style_text_color(obj, lv_color_hex(0xffffff), LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_text_opa(obj, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_text_font(obj, &ui_font_amiko_12, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_label_set_text_static(obj, "4.  SHUNT CAL");
                }
            }
        }
        {
            // box_cal_manuf_id
            lv_obj_t *obj = lv_obj_create(parent_obj);
            objects.box_cal_manuf_id = obj;
            lv_obj_set_pos(obj, 6, 146);
            lv_obj_set_size(obj, 308, 25);
            lv_obj_set_style_pad_left(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_pad_top(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_pad_right(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_pad_bottom(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_border_width(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_radius(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_remove_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
            lv_obj_set_style_bg_color(obj, lv_color_hex(0x272727), LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_bg_opa(obj, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_bg_color(obj, lv_color_hex(0x383838), LV_PART_MAIN | LV_STATE_FOCUSED);
            lv_obj_set_style_bg_opa(obj, 255, LV_PART_MAIN | LV_STATE_FOCUSED);
            lv_obj_set_style_bg_color(obj, lv_color_hex(0x383838), LV_PART_MAIN | LV_STATE_HOVERED);
            lv_obj_set_style_bg_opa(obj, 255, LV_PART_MAIN | LV_STATE_HOVERED);
            {
                lv_obj_t *parent_obj = obj;
                {
                    // lbl_val_manuf_id
                    lv_obj_t *obj = lv_label_create(parent_obj);
                    objects.lbl_val_manuf_id = obj;
                    lv_obj_set_pos(obj, 141, 6);
                    lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                    lv_obj_set_style_text_color(obj, lv_color_hex(0x03dac6), LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_text_font(obj, &ui_font_azeebee_12, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_text_opa(obj, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_label_set_text_static(obj, "0x0000");
                }
                {
                    // lbl_ina228_cal_text5
                    lv_obj_t *obj = lv_label_create(parent_obj);
                    objects.lbl_ina228_cal_text5 = obj;
                    lv_obj_set_pos(obj, 7, 6);
                    lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                    lv_obj_set_style_text_color(obj, lv_color_hex(0xffffff), LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_text_opa(obj, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_text_font(obj, &ui_font_amiko_12, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_label_set_text_static(obj, "5. MANUFACT ID");
                }
            }
        }
        {
            // box_cal_device_id
            lv_obj_t *obj = lv_obj_create(parent_obj);
            objects.box_cal_device_id = obj;
            lv_obj_set_pos(obj, 6, 173);
            lv_obj_set_size(obj, 308, 25);
            lv_obj_set_style_pad_left(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_pad_top(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_pad_right(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_pad_bottom(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_border_width(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_radius(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_remove_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
            lv_obj_set_style_bg_color(obj, lv_color_hex(0x272727), LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_bg_opa(obj, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_bg_color(obj, lv_color_hex(0x383838), LV_PART_MAIN | LV_STATE_FOCUSED);
            lv_obj_set_style_bg_opa(obj, 255, LV_PART_MAIN | LV_STATE_FOCUSED);
            lv_obj_set_style_bg_color(obj, lv_color_hex(0x383838), LV_PART_MAIN | LV_STATE_HOVERED);
            lv_obj_set_style_bg_opa(obj, 255, LV_PART_MAIN | LV_STATE_HOVERED);
            {
                lv_obj_t *parent_obj = obj;
                {
                    // lbl_val_device_id
                    lv_obj_t *obj = lv_label_create(parent_obj);
                    objects.lbl_val_device_id = obj;
                    lv_obj_set_pos(obj, 141, 6);
                    lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                    lv_obj_set_style_text_color(obj, lv_color_hex(0x03dac6), LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_text_font(obj, &ui_font_azeebee_12, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_text_opa(obj, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_label_set_text_static(obj, "0x0000");
                }
                {
                    // lbl_ina228_cal_text6
                    lv_obj_t *obj = lv_label_create(parent_obj);
                    objects.lbl_ina228_cal_text6 = obj;
                    lv_obj_set_pos(obj, 7, 6);
                    lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                    lv_obj_set_style_text_color(obj, lv_color_hex(0xffffff), LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_text_opa(obj, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_text_font(obj, &ui_font_amiko_12, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_label_set_text_static(obj, "6. DEVICE ID");
                }
            }
        }
    }
    
    tick_screen_ina228_cal_screen();
}

void tick_screen_ina228_cal_screen() {
}

void create_screen_ina228_alert_screen() {
    lv_obj_t *obj = lv_obj_create(0);
    objects.ina228_alert_screen = obj;
    lv_obj_set_pos(obj, 0, 0);
    lv_obj_set_size(obj, 320, 240);
    lv_obj_remove_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
    lv_obj_set_style_bg_color(obj, lv_color_hex(0x121212), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(obj, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
    {
        lv_obj_t *parent_obj = obj;
        {
            // ina228_alert_hdr
            lv_obj_t *obj = lv_obj_create(parent_obj);
            objects.ina228_alert_hdr = obj;
            lv_obj_set_pos(obj, 0, 4);
            lv_obj_set_size(obj, 320, 25);
            lv_obj_set_style_pad_left(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_pad_top(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_pad_right(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_pad_bottom(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_border_width(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_radius(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_remove_flag(obj, LV_OBJ_FLAG_SCROLLABLE|LV_OBJ_FLAG_SCROLL_CHAIN_HOR|LV_OBJ_FLAG_SCROLL_CHAIN_VER|LV_OBJ_FLAG_SCROLL_ELASTIC|LV_OBJ_FLAG_SCROLL_MOMENTUM|LV_OBJ_FLAG_SCROLL_WITH_ARROW);
            lv_obj_set_style_bg_color(obj, lv_color_hex(0x272727), LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_bg_opa(obj, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
            {
                lv_obj_t *parent_obj = obj;
                {
                    // lbl_ina228_alert_title
                    lv_obj_t *obj = lv_label_create(parent_obj);
                    objects.lbl_ina228_alert_title = obj;
                    lv_obj_set_pos(obj, 13, 6);
                    lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                    lv_obj_set_style_text_opa(obj, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_text_color(obj, lv_color_hex(0xffffff), LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_text_font(obj, &ui_font_amiko_12, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_label_set_text_static(obj, "INA228 - ALERT CONFIG");
                }
                {
                    // lbl_ina228_alert_page
                    lv_obj_t *obj = lv_label_create(parent_obj);
                    objects.lbl_ina228_alert_page = obj;
                    lv_obj_set_pos(obj, 274, 6);
                    lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                    lv_obj_set_style_text_color(obj, lv_color_hex(0xffffff), LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_text_font(obj, &ui_font_abee_zee_14, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_text_opa(obj, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_label_set_text_static(obj, "3/4");
                }
            }
        }
        {
            // box_alert_latch
            lv_obj_t *obj = lv_obj_create(parent_obj);
            objects.box_alert_latch = obj;
            lv_obj_set_pos(obj, 6, 38);
            lv_obj_set_size(obj, 308, 38);
            lv_obj_set_style_pad_left(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_pad_top(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_pad_right(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_pad_bottom(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_border_width(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_radius(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_remove_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
            lv_obj_set_style_bg_color(obj, lv_color_hex(0x272727), LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_bg_opa(obj, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_bg_color(obj, lv_color_hex(0x383838), LV_PART_MAIN | LV_STATE_FOCUSED);
            lv_obj_set_style_bg_opa(obj, 255, LV_PART_MAIN | LV_STATE_FOCUSED);
            lv_obj_set_style_bg_color(obj, lv_color_hex(0x383838), LV_PART_MAIN | LV_STATE_HOVERED);
            lv_obj_set_style_bg_opa(obj, 255, LV_PART_MAIN | LV_STATE_HOVERED);
            {
                lv_obj_t *parent_obj = obj;
                {
                    // lbl_alert_val1
                    lv_obj_t *obj = lv_label_create(parent_obj);
                    objects.lbl_alert_val1 = obj;
                    lv_obj_set_pos(obj, 176, 12);
                    lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                    lv_obj_set_style_text_color(obj, lv_color_hex(0x03dac6), LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_text_font(obj, &ui_font_azeebee_12, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_text_opa(obj, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_label_set_text_static(obj, "LATCHED");
                }
                {
                    // lbl_ina228_alert_text1
                    lv_obj_t *obj = lv_label_create(parent_obj);
                    objects.lbl_ina228_alert_text1 = obj;
                    lv_obj_set_pos(obj, 7, 12);
                    lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                    lv_obj_set_style_text_color(obj, lv_color_hex(0xffffff), LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_text_opa(obj, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_text_font(obj, &ui_font_amiko_12, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_label_set_text_static(obj, "1. ALATCH Mode");
                }
            }
        }
        {
            // btn_alert_prev
            lv_obj_t *obj = lv_button_create(parent_obj);
            objects.btn_alert_prev = obj;
            lv_obj_set_pos(obj, 6, 207);
            lv_obj_set_size(obj, 86, 28);
            lv_obj_set_style_bg_color(obj, lv_color_hex(0xef5350), LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_text_color(obj, lv_color_hex(0x000000), LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_text_opa(obj, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_text_font(obj, &ui_font_amiko_12, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_radius(obj, 4, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_shadow_ofs_y(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_bg_opa(obj, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_text_align(obj, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_PRESSED);
            lv_obj_set_style_bg_color(obj, lv_color_hex(0xef5350), LV_PART_MAIN | LV_STATE_CHECKED);
            {
                lv_obj_t *parent_obj = obj;
                {
                    // btn_label_adc_back_2
                    lv_obj_t *obj = lv_label_create(parent_obj);
                    objects.btn_label_adc_back_2 = obj;
                    lv_obj_set_pos(obj, 0, 0);
                    lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                    lv_obj_set_style_align(obj, LV_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_text_color(obj, lv_color_hex(0xffffff), LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_label_set_text_static(obj, "< PREV");
                }
            }
        }
        {
            // btn_alert_next
            lv_obj_t *obj = lv_button_create(parent_obj);
            objects.btn_alert_next = obj;
            lv_obj_set_pos(obj, 228, 207);
            lv_obj_set_size(obj, 86, 28);
            lv_obj_set_style_bg_color(obj, lv_color_hex(0xffffff), LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_text_color(obj, lv_color_hex(0x000000), LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_text_opa(obj, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_text_font(obj, &ui_font_amiko_12, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_radius(obj, 4, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_shadow_ofs_y(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_text_align(obj, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_PRESSED);
            {
                lv_obj_t *parent_obj = obj;
                {
                    // btn_label_adc_select_2
                    lv_obj_t *obj = lv_label_create(parent_obj);
                    objects.btn_label_adc_select_2 = obj;
                    lv_obj_set_pos(obj, 0, 0);
                    lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                    lv_obj_set_style_align(obj, LV_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_label_set_text_static(obj, "NEXT >");
                }
            }
        }
        {
            // btn_alert_save
            lv_obj_t *obj = lv_button_create(parent_obj);
            objects.btn_alert_save = obj;
            lv_obj_set_pos(obj, 117, 207);
            lv_obj_set_size(obj, 86, 28);
            lv_obj_set_style_bg_color(obj, lv_color_hex(0xa2ff67), LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_text_color(obj, lv_color_hex(0xffffff), LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_text_opa(obj, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_text_font(obj, &ui_font_amiko_12, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_radius(obj, 4, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_bg_opa(obj, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_shadow_ofs_y(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_text_align(obj, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_PRESSED);
            lv_obj_set_style_bg_color(obj, lv_color_hex(0xa2ff67), LV_PART_MAIN | LV_STATE_CHECKED);
            {
                lv_obj_t *parent_obj = obj;
                {
                    // btn_label_adc_enter_2
                    lv_obj_t *obj = lv_label_create(parent_obj);
                    objects.btn_label_adc_enter_2 = obj;
                    lv_obj_set_pos(obj, 0, 0);
                    lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                    lv_obj_set_style_align(obj, LV_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_text_color(obj, lv_color_hex(0x000000), LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_text_opa(obj, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_text_font(obj, &ui_font_amiko_12, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_label_set_text_static(obj, "SAVE & EXIT");
                }
            }
        }
        {
            // box_alert_cnvr
            lv_obj_t *obj = lv_obj_create(parent_obj);
            objects.box_alert_cnvr = obj;
            lv_obj_set_pos(obj, 6, 78);
            lv_obj_set_size(obj, 308, 38);
            lv_obj_set_style_pad_left(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_pad_top(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_pad_right(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_pad_bottom(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_border_width(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_radius(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_remove_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
            lv_obj_set_style_bg_color(obj, lv_color_hex(0x272727), LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_bg_opa(obj, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_bg_color(obj, lv_color_hex(0x383838), LV_PART_MAIN | LV_STATE_FOCUSED);
            lv_obj_set_style_bg_opa(obj, 255, LV_PART_MAIN | LV_STATE_FOCUSED);
            lv_obj_set_style_bg_color(obj, lv_color_hex(0x383838), LV_PART_MAIN | LV_STATE_HOVERED);
            lv_obj_set_style_bg_opa(obj, 255, LV_PART_MAIN | LV_STATE_HOVERED);
            {
                lv_obj_t *parent_obj = obj;
                {
                    // lbl_alert_val2
                    lv_obj_t *obj = lv_label_create(parent_obj);
                    objects.lbl_alert_val2 = obj;
                    lv_obj_set_pos(obj, 176, 12);
                    lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                    lv_obj_set_style_text_color(obj, lv_color_hex(0x03dac6), LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_text_font(obj, &ui_font_azeebee_12, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_text_opa(obj, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_label_set_text_static(obj, "ON");
                }
                {
                    // lbl_ina228_alert_text2
                    lv_obj_t *obj = lv_label_create(parent_obj);
                    objects.lbl_ina228_alert_text2 = obj;
                    lv_obj_set_pos(obj, 7, 12);
                    lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                    lv_obj_set_style_text_color(obj, lv_color_hex(0xffffff), LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_text_opa(obj, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_text_font(obj, &ui_font_amiko_12, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_label_set_text_static(obj, "2. Conv Ready Pin (CNVR)");
                }
            }
        }
        {
            // box_alert_apol
            lv_obj_t *obj = lv_obj_create(parent_obj);
            objects.box_alert_apol = obj;
            lv_obj_set_pos(obj, 6, 118);
            lv_obj_set_size(obj, 308, 38);
            lv_obj_set_style_pad_left(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_pad_top(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_pad_right(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_pad_bottom(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_border_width(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_radius(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_remove_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
            lv_obj_set_style_bg_color(obj, lv_color_hex(0x272727), LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_bg_opa(obj, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_bg_color(obj, lv_color_hex(0x383838), LV_PART_MAIN | LV_STATE_FOCUSED);
            lv_obj_set_style_bg_opa(obj, 255, LV_PART_MAIN | LV_STATE_FOCUSED);
            lv_obj_set_style_bg_color(obj, lv_color_hex(0x383838), LV_PART_MAIN | LV_STATE_HOVERED);
            lv_obj_set_style_bg_opa(obj, 255, LV_PART_MAIN | LV_STATE_HOVERED);
            {
                lv_obj_t *parent_obj = obj;
                {
                    // lbl_alert_val3
                    lv_obj_t *obj = lv_label_create(parent_obj);
                    objects.lbl_alert_val3 = obj;
                    lv_obj_set_pos(obj, 176, 12);
                    lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                    lv_obj_set_style_text_color(obj, lv_color_hex(0x03dac6), LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_text_font(obj, &ui_font_azeebee_12, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_text_opa(obj, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_label_set_text_static(obj, "ACTIVE LOW");
                }
                {
                    // lbl_ina228_alert_text3
                    lv_obj_t *obj = lv_label_create(parent_obj);
                    objects.lbl_ina228_alert_text3 = obj;
                    lv_obj_set_pos(obj, 7, 12);
                    lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                    lv_obj_set_style_text_color(obj, lv_color_hex(0xffffff), LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_text_opa(obj, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_text_font(obj, &ui_font_amiko_12, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_label_set_text_static(obj, "3. Pin Polarity (APOL)");
                }
                {
                    // switch_alert3
                    lv_obj_t *obj = lv_switch_create(parent_obj);
                    objects.switch_alert3 = obj;
                    lv_obj_set_pos(obj, 252, 7);
                    lv_obj_set_size(obj, 50, 25);
                    lv_obj_set_style_bg_color(obj, lv_color_hex(0xbb86fc), LV_PART_INDICATOR | LV_STATE_CHECKED);
                }
            }
        }
        {
            // box_alert_filter
            lv_obj_t *obj = lv_obj_create(parent_obj);
            objects.box_alert_filter = obj;
            lv_obj_set_pos(obj, 6, 158);
            lv_obj_set_size(obj, 308, 38);
            lv_obj_set_style_pad_left(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_pad_top(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_pad_right(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_pad_bottom(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_border_width(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_radius(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_remove_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
            lv_obj_set_style_bg_color(obj, lv_color_hex(0x272727), LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_bg_opa(obj, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_bg_color(obj, lv_color_hex(0x383838), LV_PART_MAIN | LV_STATE_FOCUSED);
            lv_obj_set_style_bg_opa(obj, 255, LV_PART_MAIN | LV_STATE_FOCUSED);
            lv_obj_set_style_bg_color(obj, lv_color_hex(0x383838), LV_PART_MAIN | LV_STATE_HOVERED);
            lv_obj_set_style_bg_opa(obj, 255, LV_PART_MAIN | LV_STATE_HOVERED);
            {
                lv_obj_t *parent_obj = obj;
                {
                    // lbl_alert_val4
                    lv_obj_t *obj = lv_label_create(parent_obj);
                    objects.lbl_alert_val4 = obj;
                    lv_obj_set_pos(obj, 176, 12);
                    lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                    lv_obj_set_style_text_color(obj, lv_color_hex(0x03dac6), LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_text_font(obj, &ui_font_azeebee_12, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_text_opa(obj, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_label_set_text_static(obj, "AVERAGED");
                }
                {
                    // lbl_ina228_alert_text4
                    lv_obj_t *obj = lv_label_create(parent_obj);
                    objects.lbl_ina228_alert_text4 = obj;
                    lv_obj_set_pos(obj, 7, 12);
                    lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                    lv_obj_set_style_text_color(obj, lv_color_hex(0xffffff), LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_text_opa(obj, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_text_font(obj, &ui_font_amiko_12, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_label_set_text_static(obj, "4. Alert Filter (SLOW)");
                }
                {
                    // switch_alert4
                    lv_obj_t *obj = lv_switch_create(parent_obj);
                    objects.switch_alert4 = obj;
                    lv_obj_set_pos(obj, 252, 7);
                    lv_obj_set_size(obj, 50, 25);
                    lv_obj_set_style_bg_color(obj, lv_color_hex(0xbb86fc), LV_PART_INDICATOR | LV_STATE_CHECKED);
                }
            }
        }
        {
            // switch_alert1
            lv_obj_t *obj = lv_switch_create(parent_obj);
            objects.switch_alert1 = obj;
            lv_obj_set_pos(obj, 258, 45);
            lv_obj_set_size(obj, 50, 25);
            lv_obj_set_style_bg_color(obj, lv_color_hex(0xbb86fc), LV_PART_INDICATOR | LV_STATE_CHECKED);
        }
        {
            // switch_alert2
            lv_obj_t *obj = lv_switch_create(parent_obj);
            objects.switch_alert2 = obj;
            lv_obj_set_pos(obj, 258, 85);
            lv_obj_set_size(obj, 50, 25);
            lv_obj_set_style_bg_color(obj, lv_color_hex(0xbb86fc), LV_PART_INDICATOR | LV_STATE_CHECKED);
        }
    }
    
    tick_screen_ina228_alert_screen();
}

void tick_screen_ina228_alert_screen() {
}

void create_screen_ina228_limits_screen() {
    lv_obj_t *obj = lv_obj_create(0);
    objects.ina228_limits_screen = obj;
    lv_obj_set_pos(obj, 0, 0);
    lv_obj_set_size(obj, 320, 240);
    lv_obj_remove_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
    lv_obj_set_style_bg_color(obj, lv_color_hex(0x121212), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(obj, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
    {
        lv_obj_t *parent_obj = obj;
        {
            // ina228_thr_hdr
            lv_obj_t *obj = lv_obj_create(parent_obj);
            objects.ina228_thr_hdr = obj;
            lv_obj_set_pos(obj, 0, 4);
            lv_obj_set_size(obj, 320, 25);
            lv_obj_set_style_pad_left(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_pad_top(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_pad_right(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_pad_bottom(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_border_width(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_radius(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_remove_flag(obj, LV_OBJ_FLAG_SCROLLABLE|LV_OBJ_FLAG_SCROLL_CHAIN_HOR|LV_OBJ_FLAG_SCROLL_CHAIN_VER|LV_OBJ_FLAG_SCROLL_ELASTIC|LV_OBJ_FLAG_SCROLL_MOMENTUM|LV_OBJ_FLAG_SCROLL_WITH_ARROW);
            lv_obj_set_style_bg_color(obj, lv_color_hex(0x272727), LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_bg_opa(obj, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
            {
                lv_obj_t *parent_obj = obj;
                {
                    // lbl_ina228_thr_title
                    lv_obj_t *obj = lv_label_create(parent_obj);
                    objects.lbl_ina228_thr_title = obj;
                    lv_obj_set_pos(obj, 13, 6);
                    lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                    lv_obj_set_style_text_opa(obj, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_text_color(obj, lv_color_hex(0xffffff), LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_text_font(obj, &ui_font_amiko_12, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_label_set_text_static(obj, "INA228 - THRESHOLD PROTECT");
                }
                {
                    // lbl_ina228_thr_page
                    lv_obj_t *obj = lv_label_create(parent_obj);
                    objects.lbl_ina228_thr_page = obj;
                    lv_obj_set_pos(obj, 274, 6);
                    lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                    lv_obj_set_style_text_color(obj, lv_color_hex(0xffffff), LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_text_font(obj, &ui_font_abee_zee_14, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_text_opa(obj, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_label_set_text_static(obj, "4/4");
                }
            }
        }
        {
            // box_adc_range_1
            lv_obj_t *obj = lv_obj_create(parent_obj);
            objects.box_adc_range_1 = obj;
            lv_obj_set_pos(obj, 6, 38);
            lv_obj_set_size(obj, 308, 25);
            lv_obj_set_style_pad_left(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_pad_top(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_pad_right(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_pad_bottom(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_border_width(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_radius(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_remove_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
            lv_obj_set_style_bg_color(obj, lv_color_hex(0x272727), LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_bg_opa(obj, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_bg_color(obj, lv_color_hex(0x383838), LV_PART_MAIN | LV_STATE_FOCUSED);
            lv_obj_set_style_bg_opa(obj, 255, LV_PART_MAIN | LV_STATE_FOCUSED);
            lv_obj_set_style_bg_color(obj, lv_color_hex(0x383838), LV_PART_MAIN | LV_STATE_HOVERED);
            lv_obj_set_style_bg_opa(obj, 255, LV_PART_MAIN | LV_STATE_HOVERED);
            {
                lv_obj_t *parent_obj = obj;
                {
                    // btn_ina228_thr_edit1
                    lv_obj_t *obj = lv_button_create(parent_obj);
                    objects.btn_ina228_thr_edit1 = obj;
                    lv_obj_set_pos(obj, 251, 4);
                    lv_obj_set_size(obj, 52, 18);
                    lv_obj_set_style_bg_color(obj, lv_color_hex(0xffffff), LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_text_color(obj, lv_color_hex(0x000000), LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_text_opa(obj, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_text_font(obj, &ui_font_amiko_12, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_radius(obj, 4, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_shadow_ofs_y(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_text_align(obj, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_PRESSED);
                    {
                        lv_obj_t *parent_obj = obj;
                        {
                            // lbl_btn_adc_edit1_3
                            lv_obj_t *obj = lv_label_create(parent_obj);
                            objects.lbl_btn_adc_edit1_3 = obj;
                            lv_obj_set_pos(obj, 0, 0);
                            lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                            lv_obj_set_style_align(obj, LV_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_obj_set_style_text_font(obj, &ui_font_amiko_10, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_label_set_text_static(obj, "SET");
                        }
                    }
                }
                {
                    // lbl_thr_text1
                    lv_obj_t *obj = lv_label_create(parent_obj);
                    objects.lbl_thr_text1 = obj;
                    lv_obj_set_pos(obj, 7, 6);
                    lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                    lv_obj_set_style_text_color(obj, lv_color_hex(0xffffff), LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_text_opa(obj, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_text_font(obj, &ui_font_amiko_12, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_label_set_text_static(obj, "1. Shunt Over V (SOVL)");
                }
                {
                    // lbl_val_thr1
                    lv_obj_t *obj = lv_label_create(parent_obj);
                    objects.lbl_val_thr1 = obj;
                    lv_obj_set_pos(obj, 183, 6);
                    lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                    lv_obj_set_style_text_color(obj, lv_color_hex(0xbb86fc), LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_text_font(obj, &ui_font_amiko_12, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_text_opa(obj, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_label_set_text_static(obj, "50.00 mV");
                }
            }
        }
        {
            // btn_thr_prev
            lv_obj_t *obj = lv_button_create(parent_obj);
            objects.btn_thr_prev = obj;
            lv_obj_set_pos(obj, 6, 207);
            lv_obj_set_size(obj, 86, 28);
            lv_obj_set_style_bg_color(obj, lv_color_hex(0xef5350), LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_text_color(obj, lv_color_hex(0x000000), LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_text_opa(obj, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_text_font(obj, &ui_font_amiko_12, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_radius(obj, 4, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_shadow_ofs_y(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_bg_opa(obj, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_text_align(obj, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_PRESSED);
            lv_obj_set_style_bg_color(obj, lv_color_hex(0xef5350), LV_PART_MAIN | LV_STATE_CHECKED);
            {
                lv_obj_t *parent_obj = obj;
                {
                    // btn_label_adc_back_3
                    lv_obj_t *obj = lv_label_create(parent_obj);
                    objects.btn_label_adc_back_3 = obj;
                    lv_obj_set_pos(obj, 0, 0);
                    lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                    lv_obj_set_style_align(obj, LV_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_text_color(obj, lv_color_hex(0xffffff), LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_label_set_text_static(obj, "< PREV");
                }
            }
        }
        {
            // btn_thr_save
            lv_obj_t *obj = lv_button_create(parent_obj);
            objects.btn_thr_save = obj;
            lv_obj_set_pos(obj, 228, 207);
            lv_obj_set_size(obj, 86, 28);
            lv_obj_set_style_bg_color(obj, lv_color_hex(0xa2ff67), LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_text_color(obj, lv_color_hex(0xffffff), LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_text_opa(obj, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_text_font(obj, &ui_font_amiko_12, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_radius(obj, 4, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_bg_opa(obj, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_shadow_ofs_y(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_text_align(obj, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_PRESSED);
            lv_obj_set_style_bg_color(obj, lv_color_hex(0xa2ff67), LV_PART_MAIN | LV_STATE_CHECKED);
            {
                lv_obj_t *parent_obj = obj;
                {
                    // btn_label_adc_enter_3
                    lv_obj_t *obj = lv_label_create(parent_obj);
                    objects.btn_label_adc_enter_3 = obj;
                    lv_obj_set_pos(obj, 0, 0);
                    lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                    lv_obj_set_style_align(obj, LV_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_text_color(obj, lv_color_hex(0x000000), LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_text_opa(obj, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_text_font(obj, &ui_font_amiko_12, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_label_set_text_static(obj, "SAVE & EXIT");
                }
            }
        }
        {
            // box_adc_mode_1
            lv_obj_t *obj = lv_obj_create(parent_obj);
            objects.box_adc_mode_1 = obj;
            lv_obj_set_pos(obj, 6, 65);
            lv_obj_set_size(obj, 308, 25);
            lv_obj_set_style_pad_left(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_pad_top(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_pad_right(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_pad_bottom(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_border_width(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_radius(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_remove_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
            lv_obj_set_style_bg_color(obj, lv_color_hex(0x272727), LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_bg_opa(obj, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_bg_color(obj, lv_color_hex(0x383838), LV_PART_MAIN | LV_STATE_FOCUSED);
            lv_obj_set_style_bg_opa(obj, 255, LV_PART_MAIN | LV_STATE_FOCUSED);
            lv_obj_set_style_bg_color(obj, lv_color_hex(0x383838), LV_PART_MAIN | LV_STATE_HOVERED);
            lv_obj_set_style_bg_opa(obj, 255, LV_PART_MAIN | LV_STATE_HOVERED);
            {
                lv_obj_t *parent_obj = obj;
                {
                    // btn_ina228_thr_edit2
                    lv_obj_t *obj = lv_button_create(parent_obj);
                    objects.btn_ina228_thr_edit2 = obj;
                    lv_obj_set_pos(obj, 251, 3);
                    lv_obj_set_size(obj, 52, 18);
                    lv_obj_set_style_bg_color(obj, lv_color_hex(0xffffff), LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_text_color(obj, lv_color_hex(0x000000), LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_text_opa(obj, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_text_font(obj, &ui_font_amiko_12, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_radius(obj, 4, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_shadow_ofs_y(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_text_align(obj, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_PRESSED);
                    {
                        lv_obj_t *parent_obj = obj;
                        {
                            // lbl_btn_adc_edit2_1
                            lv_obj_t *obj = lv_label_create(parent_obj);
                            objects.lbl_btn_adc_edit2_1 = obj;
                            lv_obj_set_pos(obj, 0, 0);
                            lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                            lv_obj_set_style_align(obj, LV_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_obj_set_style_text_font(obj, &ui_font_amiko_10, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_label_set_text_static(obj, "SET");
                        }
                    }
                }
                {
                    // lbl_thr_text2
                    lv_obj_t *obj = lv_label_create(parent_obj);
                    objects.lbl_thr_text2 = obj;
                    lv_obj_set_pos(obj, 7, 4);
                    lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                    lv_obj_set_style_text_color(obj, lv_color_hex(0xffffff), LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_text_opa(obj, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_text_font(obj, &ui_font_amiko_12, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_label_set_text_static(obj, "2. Shunt Under V (SUVL)");
                }
                {
                    // lbl_val_thr2
                    lv_obj_t *obj = lv_label_create(parent_obj);
                    objects.lbl_val_thr2 = obj;
                    lv_obj_set_pos(obj, 178, 6);
                    lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                    lv_obj_set_style_text_color(obj, lv_color_hex(0xbb86fc), LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_text_font(obj, &ui_font_amiko_12, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_text_opa(obj, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_label_set_text_static(obj, "-10.00 mV");
                }
            }
        }
        {
            // box_adc_avg_1
            lv_obj_t *obj = lv_obj_create(parent_obj);
            objects.box_adc_avg_1 = obj;
            lv_obj_set_pos(obj, 6, 92);
            lv_obj_set_size(obj, 308, 25);
            lv_obj_set_style_pad_left(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_pad_top(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_pad_right(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_pad_bottom(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_border_width(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_radius(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_remove_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
            lv_obj_set_style_bg_color(obj, lv_color_hex(0x272727), LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_bg_opa(obj, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_bg_color(obj, lv_color_hex(0x383838), LV_PART_MAIN | LV_STATE_FOCUSED);
            lv_obj_set_style_bg_opa(obj, 255, LV_PART_MAIN | LV_STATE_FOCUSED);
            lv_obj_set_style_bg_color(obj, lv_color_hex(0x383838), LV_PART_MAIN | LV_STATE_HOVERED);
            lv_obj_set_style_bg_opa(obj, 255, LV_PART_MAIN | LV_STATE_HOVERED);
            {
                lv_obj_t *parent_obj = obj;
                {
                    // lbl_val_thr3
                    lv_obj_t *obj = lv_label_create(parent_obj);
                    objects.lbl_val_thr3 = obj;
                    lv_obj_set_pos(obj, 183, 6);
                    lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                    lv_obj_set_style_text_color(obj, lv_color_hex(0xbb86fc), LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_text_font(obj, &ui_font_amiko_12, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_text_opa(obj, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_label_set_text_static(obj, "52.00 V");
                }
                {
                    // btn_ina228_thr_edit3
                    lv_obj_t *obj = lv_button_create(parent_obj);
                    objects.btn_ina228_thr_edit3 = obj;
                    lv_obj_set_pos(obj, 251, 3);
                    lv_obj_set_size(obj, 52, 18);
                    lv_obj_set_style_bg_color(obj, lv_color_hex(0xffffff), LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_text_color(obj, lv_color_hex(0x000000), LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_text_opa(obj, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_text_font(obj, &ui_font_amiko_12, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_radius(obj, 4, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_shadow_ofs_y(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_text_align(obj, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_PRESSED);
                    {
                        lv_obj_t *parent_obj = obj;
                        {
                            // lbl_btn_adc_edit3_1
                            lv_obj_t *obj = lv_label_create(parent_obj);
                            objects.lbl_btn_adc_edit3_1 = obj;
                            lv_obj_set_pos(obj, 0, 0);
                            lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                            lv_obj_set_style_align(obj, LV_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_obj_set_style_text_font(obj, &ui_font_amiko_10, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_label_set_text_static(obj, "SET");
                        }
                    }
                }
                {
                    // lbl_thr_text3
                    lv_obj_t *obj = lv_label_create(parent_obj);
                    objects.lbl_thr_text3 = obj;
                    lv_obj_set_pos(obj, 7, 6);
                    lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                    lv_obj_set_style_text_color(obj, lv_color_hex(0xffffff), LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_text_opa(obj, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_text_font(obj, &ui_font_amiko_12, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_label_set_text_static(obj, "3. Bus Under V (BOVL)");
                }
            }
        }
        {
            // box_adc_convdelay_1
            lv_obj_t *obj = lv_obj_create(parent_obj);
            objects.box_adc_convdelay_1 = obj;
            lv_obj_set_pos(obj, 6, 119);
            lv_obj_set_size(obj, 308, 25);
            lv_obj_set_style_pad_left(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_pad_top(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_pad_right(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_pad_bottom(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_border_width(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_radius(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_remove_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
            lv_obj_set_style_bg_color(obj, lv_color_hex(0x272727), LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_bg_opa(obj, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_bg_color(obj, lv_color_hex(0x383838), LV_PART_MAIN | LV_STATE_FOCUSED);
            lv_obj_set_style_bg_opa(obj, 255, LV_PART_MAIN | LV_STATE_FOCUSED);
            lv_obj_set_style_bg_color(obj, lv_color_hex(0x383838), LV_PART_MAIN | LV_STATE_HOVERED);
            lv_obj_set_style_bg_opa(obj, 255, LV_PART_MAIN | LV_STATE_HOVERED);
            {
                lv_obj_t *parent_obj = obj;
                {
                    // btn_ina228_thr_edit4
                    lv_obj_t *obj = lv_button_create(parent_obj);
                    objects.btn_ina228_thr_edit4 = obj;
                    lv_obj_set_pos(obj, 251, 2);
                    lv_obj_set_size(obj, 52, 18);
                    lv_obj_set_style_bg_color(obj, lv_color_hex(0xffffff), LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_text_color(obj, lv_color_hex(0x000000), LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_text_opa(obj, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_text_font(obj, &ui_font_amiko_12, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_radius(obj, 4, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_shadow_ofs_y(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_text_align(obj, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_PRESSED);
                    {
                        lv_obj_t *parent_obj = obj;
                        {
                            // lbl_btn_adc_edit4_1
                            lv_obj_t *obj = lv_label_create(parent_obj);
                            objects.lbl_btn_adc_edit4_1 = obj;
                            lv_obj_set_pos(obj, 0, 0);
                            lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                            lv_obj_set_style_align(obj, LV_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_obj_set_style_text_font(obj, &ui_font_amiko_10, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_label_set_text_static(obj, "SET");
                        }
                    }
                }
                {
                    // lbl_thr_text4
                    lv_obj_t *obj = lv_label_create(parent_obj);
                    objects.lbl_thr_text4 = obj;
                    lv_obj_set_pos(obj, 7, 6);
                    lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                    lv_obj_set_style_text_color(obj, lv_color_hex(0xffffff), LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_text_opa(obj, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_text_font(obj, &ui_font_amiko_12, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_label_set_text_static(obj, "4. Bus under V (BUVL)");
                }
                {
                    // lbl_val_thr4
                    lv_obj_t *obj = lv_label_create(parent_obj);
                    objects.lbl_val_thr4 = obj;
                    lv_obj_set_pos(obj, 183, 6);
                    lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                    lv_obj_set_style_text_color(obj, lv_color_hex(0xbb86fc), LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_text_font(obj, &ui_font_amiko_12, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_text_opa(obj, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_label_set_text_static(obj, "4.40 V");
                }
            }
        }
        {
            // box_adc_vbus_time_1
            lv_obj_t *obj = lv_obj_create(parent_obj);
            objects.box_adc_vbus_time_1 = obj;
            lv_obj_set_pos(obj, 6, 146);
            lv_obj_set_size(obj, 308, 25);
            lv_obj_set_style_pad_left(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_pad_top(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_pad_right(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_pad_bottom(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_border_width(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_radius(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_remove_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
            lv_obj_set_style_bg_color(obj, lv_color_hex(0x272727), LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_bg_opa(obj, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_bg_color(obj, lv_color_hex(0x383838), LV_PART_MAIN | LV_STATE_FOCUSED);
            lv_obj_set_style_bg_opa(obj, 255, LV_PART_MAIN | LV_STATE_FOCUSED);
            lv_obj_set_style_bg_color(obj, lv_color_hex(0x383838), LV_PART_MAIN | LV_STATE_HOVERED);
            lv_obj_set_style_bg_opa(obj, 255, LV_PART_MAIN | LV_STATE_HOVERED);
            {
                lv_obj_t *parent_obj = obj;
                {
                    // btn_ina228_thr_edit5
                    lv_obj_t *obj = lv_button_create(parent_obj);
                    objects.btn_ina228_thr_edit5 = obj;
                    lv_obj_set_pos(obj, 251, 2);
                    lv_obj_set_size(obj, 52, 18);
                    lv_obj_set_style_bg_color(obj, lv_color_hex(0xffffff), LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_text_color(obj, lv_color_hex(0x000000), LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_text_opa(obj, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_text_font(obj, &ui_font_amiko_12, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_radius(obj, 4, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_shadow_ofs_y(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_text_align(obj, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_PRESSED);
                    {
                        lv_obj_t *parent_obj = obj;
                        {
                            // lbl_btn_adc_edit5_1
                            lv_obj_t *obj = lv_label_create(parent_obj);
                            objects.lbl_btn_adc_edit5_1 = obj;
                            lv_obj_set_pos(obj, 0, 0);
                            lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                            lv_obj_set_style_align(obj, LV_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_obj_set_style_text_font(obj, &ui_font_amiko_10, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_label_set_text_static(obj, "SET");
                        }
                    }
                }
                {
                    // lbl_val_thr5
                    lv_obj_t *obj = lv_label_create(parent_obj);
                    objects.lbl_val_thr5 = obj;
                    lv_obj_set_pos(obj, 183, 5);
                    lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                    lv_obj_set_style_text_color(obj, lv_color_hex(0xbb86fc), LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_text_font(obj, &ui_font_amiko_12, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_text_opa(obj, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_label_set_text_static(obj, "85.0 °C");
                }
                {
                    // lbl_thr_text5
                    lv_obj_t *obj = lv_label_create(parent_obj);
                    objects.lbl_thr_text5 = obj;
                    lv_obj_set_pos(obj, 7, 6);
                    lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                    lv_obj_set_style_text_color(obj, lv_color_hex(0xffffff), LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_text_opa(obj, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_text_font(obj, &ui_font_amiko_12, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_label_set_text_static(obj, "5. Over Temp (TEMP)");
                }
            }
        }
        {
            // box_adc_vshunt_time_1
            lv_obj_t *obj = lv_obj_create(parent_obj);
            objects.box_adc_vshunt_time_1 = obj;
            lv_obj_set_pos(obj, 6, 173);
            lv_obj_set_size(obj, 308, 25);
            lv_obj_set_style_pad_left(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_pad_top(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_pad_right(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_pad_bottom(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_border_width(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_radius(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_remove_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
            lv_obj_set_style_bg_color(obj, lv_color_hex(0x272727), LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_bg_opa(obj, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_bg_color(obj, lv_color_hex(0x383838), LV_PART_MAIN | LV_STATE_FOCUSED);
            lv_obj_set_style_bg_opa(obj, 255, LV_PART_MAIN | LV_STATE_FOCUSED);
            lv_obj_set_style_bg_color(obj, lv_color_hex(0x383838), LV_PART_MAIN | LV_STATE_HOVERED);
            lv_obj_set_style_bg_opa(obj, 255, LV_PART_MAIN | LV_STATE_HOVERED);
            {
                lv_obj_t *parent_obj = obj;
                {
                    // btn_ina228_thr_edit6
                    lv_obj_t *obj = lv_button_create(parent_obj);
                    objects.btn_ina228_thr_edit6 = obj;
                    lv_obj_set_pos(obj, 251, 2);
                    lv_obj_set_size(obj, 52, 18);
                    lv_obj_set_style_bg_color(obj, lv_color_hex(0xffffff), LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_text_color(obj, lv_color_hex(0x000000), LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_text_opa(obj, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_text_font(obj, &ui_font_amiko_12, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_radius(obj, 4, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_shadow_ofs_y(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_text_align(obj, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_PRESSED);
                    {
                        lv_obj_t *parent_obj = obj;
                        {
                            // lbl_btn_adc_edit6_1
                            lv_obj_t *obj = lv_label_create(parent_obj);
                            objects.lbl_btn_adc_edit6_1 = obj;
                            lv_obj_set_pos(obj, 0, 0);
                            lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                            lv_obj_set_style_align(obj, LV_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_obj_set_style_text_font(obj, &ui_font_amiko_10, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_label_set_text_static(obj, "SET");
                        }
                    }
                }
                {
                    // lbl_val_thr6
                    lv_obj_t *obj = lv_label_create(parent_obj);
                    objects.lbl_val_thr6 = obj;
                    lv_obj_set_pos(obj, 183, 5);
                    lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                    lv_obj_set_style_text_color(obj, lv_color_hex(0xbb86fc), LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_text_font(obj, &ui_font_amiko_12, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_text_opa(obj, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_label_set_text_static(obj, "540 us");
                }
                {
                    // lbl_thr_text6
                    lv_obj_t *obj = lv_label_create(parent_obj);
                    objects.lbl_thr_text6 = obj;
                    lv_obj_set_pos(obj, 7, 6);
                    lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                    lv_obj_set_style_text_color(obj, lv_color_hex(0xffffff), LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_text_opa(obj, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_text_font(obj, &ui_font_amiko_12, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_label_set_text_static(obj, "6. Over Power (PWR)");
                }
            }
        }
    }
    
    tick_screen_ina228_limits_screen();
}

void tick_screen_ina228_limits_screen() {
}

typedef void (*tick_screen_func_t)();
tick_screen_func_t tick_screen_funcs[] = {
    tick_screen_main_screen,
    tick_screen_menu_screen,
    tick_screen_limit_screen,
    tick_screen_reset_modal,
    tick_screen_ina228_adc_screen,
    tick_screen_ina228_cal_screen,
    tick_screen_ina228_alert_screen,
    tick_screen_ina228_limits_screen,
};
void tick_screen(int screen_index) {
    if (screen_index >= 0 && screen_index < 8) {
        tick_screen_funcs[screen_index]();
    }
}
void tick_screen_by_id(enum ScreensEnum screenId) {
    tick_screen(screenId - 1);
}

//
// Fonts
//

ext_font_desc_t fonts[] = {
    { "Amiko_12", &ui_font_amiko_12 },
    { "ABeeZee_14", &ui_font_abee_zee_14 },
    { "ABeeZee_24", &ui_font_abee_zee_24 },
    { "AZeebee_12", &ui_font_azeebee_12 },
    { "Amiko_10", &ui_font_amiko_10 },
#if LV_FONT_MONTSERRAT_8
    { "MONTSERRAT_8", &lv_font_montserrat_8 },
#endif
#if LV_FONT_MONTSERRAT_10
    { "MONTSERRAT_10", &lv_font_montserrat_10 },
#endif
#if LV_FONT_MONTSERRAT_12
    { "MONTSERRAT_12", &lv_font_montserrat_12 },
#endif
#if LV_FONT_MONTSERRAT_14
    { "MONTSERRAT_14", &lv_font_montserrat_14 },
#endif
#if LV_FONT_MONTSERRAT_16
    { "MONTSERRAT_16", &lv_font_montserrat_16 },
#endif
#if LV_FONT_MONTSERRAT_18
    { "MONTSERRAT_18", &lv_font_montserrat_18 },
#endif
#if LV_FONT_MONTSERRAT_20
    { "MONTSERRAT_20", &lv_font_montserrat_20 },
#endif
#if LV_FONT_MONTSERRAT_22
    { "MONTSERRAT_22", &lv_font_montserrat_22 },
#endif
#if LV_FONT_MONTSERRAT_24
    { "MONTSERRAT_24", &lv_font_montserrat_24 },
#endif
#if LV_FONT_MONTSERRAT_26
    { "MONTSERRAT_26", &lv_font_montserrat_26 },
#endif
#if LV_FONT_MONTSERRAT_28
    { "MONTSERRAT_28", &lv_font_montserrat_28 },
#endif
#if LV_FONT_MONTSERRAT_30
    { "MONTSERRAT_30", &lv_font_montserrat_30 },
#endif
#if LV_FONT_MONTSERRAT_32
    { "MONTSERRAT_32", &lv_font_montserrat_32 },
#endif
#if LV_FONT_MONTSERRAT_34
    { "MONTSERRAT_34", &lv_font_montserrat_34 },
#endif
#if LV_FONT_MONTSERRAT_36
    { "MONTSERRAT_36", &lv_font_montserrat_36 },
#endif
#if LV_FONT_MONTSERRAT_38
    { "MONTSERRAT_38", &lv_font_montserrat_38 },
#endif
#if LV_FONT_MONTSERRAT_40
    { "MONTSERRAT_40", &lv_font_montserrat_40 },
#endif
#if LV_FONT_MONTSERRAT_42
    { "MONTSERRAT_42", &lv_font_montserrat_42 },
#endif
#if LV_FONT_MONTSERRAT_44
    { "MONTSERRAT_44", &lv_font_montserrat_44 },
#endif
#if LV_FONT_MONTSERRAT_46
    { "MONTSERRAT_46", &lv_font_montserrat_46 },
#endif
#if LV_FONT_MONTSERRAT_48
    { "MONTSERRAT_48", &lv_font_montserrat_48 },
#endif
};

//
// Color themes
//

uint32_t active_theme_index = 0;

//
//
//

void create_screens() {

// Set default LVGL theme
    lv_display_t *dispp = lv_display_get_default();
    lv_theme_t *theme = lv_theme_default_init(dispp, lv_palette_main(LV_PALETTE_BLUE), lv_palette_main(LV_PALETTE_RED), false, LV_FONT_DEFAULT);
    lv_display_set_theme(dispp, theme);
    
    // Initialize screens
    // Create screens
    create_screen_main_screen();
    //create_screen_menu_screen();
    //create_screen_limit_screen();
    //create_screen_reset_modal();
    //create_screen_ina228_adc_screen();
    //create_screen_ina228_cal_screen();
    //create_screen_ina228_alert_screen();
    //create_screen_ina228_limits_screen();
}