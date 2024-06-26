// This file was generated by SquareLine Studio
// SquareLine Studio version: SquareLine Studio 1.4.0
// LVGL version: 8.3.11
// Project name: namishs gui

#include "ui.h"
#include "ui_helpers.h"

///////////////////// VARIABLES ////////////////////
char input[6] = "";
int currentPos = 0;
bool canRFID =  false;
bool passInput = false;

// SCREEN: ui_Home_Screen
void ui_Home_Screen_screen_init(void);
lv_obj_t * ui_Home_Screen;
void ui_event_Password_Button(lv_event_t * e);
lv_obj_t * ui_Password_Button;
lv_obj_t * ui_Go_To_Password;
void ui_event_Scan_RFID(lv_event_t * e);
lv_obj_t * ui_Scan_RFID;
lv_obj_t * ui_Go_To_RFID;
lv_obj_t * ui_Image1;


// SCREEN: ui_Password_Screen
void ui_Password_Screen_screen_init(void);
lv_obj_t * ui_Password_Screen;
void ui_event_Back_Button(lv_event_t * e);
lv_obj_t * ui_Back_Button;
lv_obj_t * ui_Back_Label;
lv_obj_t * ui_Text_Input;
lv_obj_t * ui_Text_input;
void ui_event_Backspace_Button(lv_event_t * e);
lv_obj_t * ui_Backspace_Button;
lv_obj_t * ui_Backspace_Label;
void ui_event_Button1(lv_event_t * e);
lv_obj_t * ui_Button1;
lv_obj_t * ui_Label1;
void ui_event_Button2(lv_event_t * e);
lv_obj_t * ui_Button2;
lv_obj_t * ui_Label2;
void ui_event_Button3(lv_event_t * e);
lv_obj_t * ui_Button3;
lv_obj_t * ui_Label3;
void ui_event_Button4(lv_event_t * e);
lv_obj_t * ui_Button4;
lv_obj_t * ui_Label4;
void ui_event_Button5(lv_event_t * e);
lv_obj_t * ui_Button5;
lv_obj_t * ui_Label5;
void ui_event_Button6(lv_event_t * e);
lv_obj_t * ui_Button6;
lv_obj_t * ui_Label6;
void ui_event_Button7(lv_event_t * e);
lv_obj_t * ui_Button7;
lv_obj_t * ui_Label7;
void ui_event_Button8(lv_event_t * e);
lv_obj_t * ui_Button8;
lv_obj_t * ui_Label8;
void ui_event_Button9(lv_event_t * e);
lv_obj_t * ui_Button9;
lv_obj_t * ui_Label9;
void ui_event_Button0(lv_event_t * e);
lv_obj_t * ui_Button0;
lv_obj_t * ui_Label0;
void ui_event_Enter_Button(lv_event_t * e);
lv_obj_t * ui_Enter_Button;
lv_obj_t * ui_Enter;


// SCREEN: ui_RFID_Screen
void ui_RFID_Screen_screen_init(void);
lv_obj_t * ui_RFID_Screen;
lv_obj_t * ui_Label;
void ui_event_Back_Button2(lv_event_t * e);
lv_obj_t * ui_Back_Button2;
lv_obj_t * ui_Back_Label2;
lv_obj_t * ui____initial_actions0;

///////////////////// TEST LVGL SETTINGS ////////////////////
#if LV_COLOR_DEPTH != 16
    #error "LV_COLOR_DEPTH should be 16bit to match SquareLine Studio's settings"
#endif
#if LV_COLOR_16_SWAP !=0
    #error "LV_COLOR_16_SWAP should be 0 to match SquareLine Studio's settings"
#endif

///////////////////// ANIMATIONS ////////////////////

///////////////////// FUNCTIONS ////////////////////
void ui_event_Password_Button(lv_event_t * e)
{
    lv_event_code_t event_code = lv_event_get_code(e);
    lv_obj_t * target = lv_event_get_target(e);
    if(event_code == LV_EVENT_CLICKED) {
        _ui_screen_change(&ui_Password_Screen, LV_SCR_LOAD_ANIM_FADE_ON, 500, 0, &ui_Password_Screen_screen_init);
    }
}
void ui_event_Scan_RFID(lv_event_t * e)
{
    lv_event_code_t event_code = lv_event_get_code(e);
    lv_obj_t * target = lv_event_get_target(e);
    if(event_code == LV_EVENT_CLICKED) {
        _ui_screen_change(&ui_RFID_Screen, LV_SCR_LOAD_ANIM_FADE_ON, 500, 0, &ui_RFID_Screen_screen_init);
        canRFID = true;
    }
}
void ui_event_Back_Button(lv_event_t * e)
{
    lv_event_code_t event_code = lv_event_get_code(e);
    lv_obj_t * target = lv_event_get_target(e);
    if(event_code == LV_EVENT_CLICKED) {
        strcpy(input, "");
        currentPos = 0;
        _ui_label_set_property(ui_Text_input, _UI_LABEL_PROPERTY_TEXT, "Password:");
        _ui_screen_change(&ui_Home_Screen, LV_SCR_LOAD_ANIM_FADE_ON, 500, 0, &ui_Home_Screen_screen_init);
    }
}
void ui_event_Backspace_Button(lv_event_t * e)
{
    lv_event_code_t event_code = lv_event_get_code(e);
    lv_obj_t * target = lv_event_get_target(e);
    if(event_code == LV_EVENT_CLICKED) {
        if (currentPos > 0)
        {
            input[currentPos--] = " "; // Append '1' to input and increment currentPos
            input[currentPos] = '\0';  // Null-terminate the string
            _ui_label_set_property(ui_Text_input, _UI_LABEL_PROPERTY_TEXT, input);
        }

        if (currentPos == 0)
        {
            _ui_label_set_property(ui_Text_input, _UI_LABEL_PROPERTY_TEXT, "Password:");
        }
    }
}
void ui_event_Button1(lv_event_t *e)
{
    lv_event_code_t event_code = lv_event_get_code(e);
    lv_obj_t *target = lv_event_get_target(e);
    if (event_code == LV_EVENT_CLICKED)
    {
        if (currentPos <= 5) // Check if there's space in input array
        {
            input[currentPos++] = '1'; // Append '1' to input and increment currentPos
            input[currentPos] = '\0';  // Null-terminate the string
            _ui_label_set_property(ui_Text_input, _UI_LABEL_PROPERTY_TEXT, input);
        }
    }
}
void ui_event_Button2(lv_event_t *e)
{
    lv_event_code_t event_code = lv_event_get_code(e);
    lv_obj_t *target = lv_event_get_target(e);
    if (event_code == LV_EVENT_CLICKED)
    {
        if (currentPos <= 5) // Check if there's space in input array
        {
            input[currentPos++] = '2'; // Append '1' to input and increment currentPos
            input[currentPos] = '\0';  // Null-terminate the string
            _ui_label_set_property(ui_Text_input, _UI_LABEL_PROPERTY_TEXT, input);
        }
    }
}
void ui_event_Button3(lv_event_t *e)
{
    lv_event_code_t event_code = lv_event_get_code(e);
    lv_obj_t *target = lv_event_get_target(e);
    if (event_code == LV_EVENT_CLICKED)
    {
        if (currentPos <= 5) // Check if there's space in input array
        {
            input[currentPos++] = '3'; // Append '1' to input and increment currentPos
            input[currentPos] = '\0';  // Null-terminate the string
            _ui_label_set_property(ui_Text_input, _UI_LABEL_PROPERTY_TEXT, input);
        }
    }
}
void ui_event_Button4(lv_event_t *e)
{
    lv_event_code_t event_code = lv_event_get_code(e);
    lv_obj_t *target = lv_event_get_target(e);
    if (event_code == LV_EVENT_CLICKED)
    {
        if (currentPos <= 5) // Check if there's space in input array
        {
            input[currentPos++] = '4'; // Append '1' to input and increment currentPos
            input[currentPos] = '\0';  // Null-terminate the string
            _ui_label_set_property(ui_Text_input, _UI_LABEL_PROPERTY_TEXT, input);
        }
    }
}
void ui_event_Button5(lv_event_t *e)
{
    lv_event_code_t event_code = lv_event_get_code(e);
    lv_obj_t *target = lv_event_get_target(e);
    if (event_code == LV_EVENT_CLICKED)
    {
        if (currentPos <= 5) // Check if there's space in input array
        {
            input[currentPos++] = '5'; // Append '1' to input and increment currentPos
            input[currentPos] = '\0';  // Null-terminate the string
            _ui_label_set_property(ui_Text_input, _UI_LABEL_PROPERTY_TEXT, input);
        }
    }
}
void ui_event_Button6(lv_event_t *e)
{
    lv_event_code_t event_code = lv_event_get_code(e);
    lv_obj_t *target = lv_event_get_target(e);
    if (event_code == LV_EVENT_CLICKED)
    {
        if (currentPos <= 5) // Check if there's space in input array
        {
            input[currentPos++] = '6'; // Append '1' to input and increment currentPos
            input[currentPos] = '\0';  // Null-terminate the string
            _ui_label_set_property(ui_Text_input, _UI_LABEL_PROPERTY_TEXT, input);
        }
    }
}
void ui_event_Button7(lv_event_t *e)
{
    lv_event_code_t event_code = lv_event_get_code(e);
    lv_obj_t *target = lv_event_get_target(e);
    if (event_code == LV_EVENT_CLICKED)
    {
        if (currentPos <= 5) // Check if there's space in input array
        {
            input[currentPos++] = '7'; // Append '1' to input and increment currentPos
            input[currentPos] = '\0';  // Null-terminate the string
            _ui_label_set_property(ui_Text_input, _UI_LABEL_PROPERTY_TEXT, input);
        }
    }
}
void ui_event_Button8(lv_event_t *e)
{
    lv_event_code_t event_code = lv_event_get_code(e);
    lv_obj_t *target = lv_event_get_target(e);
    if (event_code == LV_EVENT_CLICKED)
    {
        if (currentPos <= 5) // Check if there's space in input array
        {
            input[currentPos++] = '8'; // Append '1' to input and increment currentPos
            input[currentPos] = '\0';  // Null-terminate the string
            _ui_label_set_property(ui_Text_input, _UI_LABEL_PROPERTY_TEXT, input);
        }
    }
}
void ui_event_Button9(lv_event_t *e)
{
    lv_event_code_t event_code = lv_event_get_code(e);
    lv_obj_t *target = lv_event_get_target(e);
    if (event_code == LV_EVENT_CLICKED)
    {
        if (currentPos <= 5) // Check if there's space in input array
        {
            input[currentPos++] = '9'; // Append '1' to input and increment currentPos
            input[currentPos] = '\0';  // Null-terminate the string
            _ui_label_set_property(ui_Text_input, _UI_LABEL_PROPERTY_TEXT, input);
        }
    }
}
void ui_event_Button0(lv_event_t *e)
{
    lv_event_code_t event_code = lv_event_get_code(e);
    lv_obj_t *target = lv_event_get_target(e);
    if (event_code == LV_EVENT_CLICKED)
    {
        if (currentPos <= 5) // Check if there's space in input array
        {
            input[currentPos++] = '0'; // Append '1' to input and increment currentPos
            input[currentPos] = '\0';  // Null-terminate the string
            _ui_label_set_property(ui_Text_input, _UI_LABEL_PROPERTY_TEXT, input);
        }
    }
}
void ui_event_Enter_Button(lv_event_t * e)
{
    lv_event_code_t event_code = lv_event_get_code(e);
    lv_obj_t * target = lv_event_get_target(e);
    if(event_code == LV_EVENT_CLICKED) {
        passInput = true;
    }
}
void ui_event_Back_Button2(lv_event_t * e)
{
    lv_event_code_t event_code = lv_event_get_code(e);
    lv_obj_t * target = lv_event_get_target(e);
    if(event_code == LV_EVENT_CLICKED) {
        _ui_screen_change(&ui_Home_Screen, LV_SCR_LOAD_ANIM_FADE_ON, 500, 0, &ui_Home_Screen_screen_init);
        canRFID = false;
    }
}

///////////////////// SCREENS ////////////////////

void ui_init(void)
{
    lv_disp_t * dispp = lv_disp_get_default();
    lv_theme_t * theme = lv_theme_default_init(dispp, lv_palette_main(LV_PALETTE_BLUE), lv_palette_main(LV_PALETTE_RED),
                                               false, LV_FONT_DEFAULT);
    lv_disp_set_theme(dispp, theme);
    ui_Home_Screen_screen_init();
    ui_Password_Screen_screen_init();
    ui_RFID_Screen_screen_init();
    ui____initial_actions0 = lv_obj_create(NULL);
    lv_disp_load_scr(ui_Home_Screen);
}
