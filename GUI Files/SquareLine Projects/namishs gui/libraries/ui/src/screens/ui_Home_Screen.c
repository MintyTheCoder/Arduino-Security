// This file was generated by SquareLine Studio
// SquareLine Studio version: SquareLine Studio 1.4.0
// LVGL version: 8.3.11
// Project name: namishs gui

#include "../ui.h"

void ui_Home_Screen_screen_init(void)
{
ui_Home_Screen = lv_obj_create(NULL);
lv_obj_clear_flag( ui_Home_Screen, LV_OBJ_FLAG_SCROLLABLE );    /// Flags
lv_obj_set_style_bg_color(ui_Home_Screen, lv_color_hex(0x000000), LV_PART_MAIN | LV_STATE_DEFAULT );
lv_obj_set_style_bg_opa(ui_Home_Screen, 255, LV_PART_MAIN| LV_STATE_DEFAULT);

ui_Password_Button = lv_btn_create(ui_Home_Screen);
lv_obj_set_width( ui_Password_Button, 200);
lv_obj_set_height( ui_Password_Button, 100);
lv_obj_set_x( ui_Password_Button, 205 );
lv_obj_set_y( ui_Password_Button, -75 );
lv_obj_set_align( ui_Password_Button, LV_ALIGN_CENTER );
lv_obj_add_flag( ui_Password_Button, LV_OBJ_FLAG_SCROLL_ON_FOCUS );   /// Flags
lv_obj_clear_flag( ui_Password_Button, LV_OBJ_FLAG_SCROLLABLE );    /// Flags
lv_obj_set_style_bg_color(ui_Password_Button, lv_color_hex(0x942624), LV_PART_MAIN | LV_STATE_DEFAULT );
lv_obj_set_style_bg_opa(ui_Password_Button, 255, LV_PART_MAIN| LV_STATE_DEFAULT);

ui_Go_To_Password = lv_label_create(ui_Password_Button);
lv_obj_set_width( ui_Go_To_Password, LV_SIZE_CONTENT);  /// 1
lv_obj_set_height( ui_Go_To_Password, LV_SIZE_CONTENT);   /// 1
lv_obj_set_align( ui_Go_To_Password, LV_ALIGN_CENTER );
lv_label_set_text(ui_Go_To_Password,"PASSWORD");
lv_obj_set_style_text_font(ui_Go_To_Password, &lv_font_montserrat_30, LV_PART_MAIN| LV_STATE_DEFAULT);

ui_Scan_RFID = lv_btn_create(ui_Home_Screen);
lv_obj_set_width( ui_Scan_RFID, 200);
lv_obj_set_height( ui_Scan_RFID, 100);
lv_obj_set_x( ui_Scan_RFID, 205 );
lv_obj_set_y( ui_Scan_RFID, 70 );
lv_obj_set_align( ui_Scan_RFID, LV_ALIGN_CENTER );
lv_obj_add_flag( ui_Scan_RFID, LV_OBJ_FLAG_SCROLL_ON_FOCUS );   /// Flags
lv_obj_clear_flag( ui_Scan_RFID, LV_OBJ_FLAG_SCROLLABLE );    /// Flags
lv_obj_set_style_bg_color(ui_Scan_RFID, lv_color_hex(0x942420), LV_PART_MAIN | LV_STATE_DEFAULT );
lv_obj_set_style_bg_opa(ui_Scan_RFID, 255, LV_PART_MAIN| LV_STATE_DEFAULT);

ui_Go_To_RFID = lv_label_create(ui_Scan_RFID);
lv_obj_set_width( ui_Go_To_RFID, LV_SIZE_CONTENT);  /// 1
lv_obj_set_height( ui_Go_To_RFID, LV_SIZE_CONTENT);   /// 1
lv_obj_set_align( ui_Go_To_RFID, LV_ALIGN_CENTER );
lv_label_set_text(ui_Go_To_RFID,"RFID");
lv_obj_set_style_text_font(ui_Go_To_RFID, &lv_font_montserrat_30, LV_PART_MAIN| LV_STATE_DEFAULT);

ui_Image1 = lv_img_create(ui_Home_Screen);
lv_img_set_src(ui_Image1, &ui_img_logo_transp_png);
lv_obj_set_width( ui_Image1, LV_SIZE_CONTENT);  /// 1
lv_obj_set_height( ui_Image1, LV_SIZE_CONTENT);   /// 1
lv_obj_set_x( ui_Image1, -165 );
lv_obj_set_y( ui_Image1, 0 );
lv_obj_set_align( ui_Image1, LV_ALIGN_CENTER );
lv_obj_add_flag( ui_Image1, LV_OBJ_FLAG_ADV_HITTEST );   /// Flags
lv_obj_clear_flag( ui_Image1, LV_OBJ_FLAG_SCROLLABLE );    /// Flags

lv_obj_add_event_cb(ui_Password_Button, ui_event_Password_Button, LV_EVENT_ALL, NULL);
lv_obj_add_event_cb(ui_Scan_RFID, ui_event_Scan_RFID, LV_EVENT_ALL, NULL);

}
