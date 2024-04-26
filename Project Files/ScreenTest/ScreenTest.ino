#include "Arduino_H7_Video.h"
#include "Arduino_GigaDisplayTouch.h"

#include "lvgl.h"
#include "ui.h"

/* Insert resolution WxH according to your SquareLine studio project settings */
Arduino_H7_Video          Display(800, 480, GigaDisplayShield); 
Arduino_GigaDisplayTouch  Touch;

void setup() {
  Display.begin();
  Touch.begin();

  ui_init();


}

void loop() {

  /* Feed LVGL engine */
  lv_timer_handler();
}