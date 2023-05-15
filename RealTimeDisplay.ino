/////////////////////////////////////////////////////////////////
/*
  ESP32 | LVGL8 | Ep 0. GFX Setup (ft. LovyanGFX)
  Video Tutorial: https://youtu.be/IPCvQ4o_WP8
  Created by Eric N. (ThatProject) 
*/
/////////////////////////////////////////////////////////////////
//To download the LovyanGFX library before use//
#include "time.h"
#include <lvgl.h>
#include "LGFX.h"
#include "WorldTimeApi.h"

LGFX tft;
WorldTimeApi api;

/*Change to your screen resolution*/
static const uint32_t screenWidth = 240;
static const uint32_t screenHeight = 320;
long temp = 28066275 * 60;
time_t now = api.get_epoch();
static lv_disp_draw_buf_t draw_buf;
static lv_color_t buf[screenWidth * 10];


/* Display flushing */
void my_disp_flush(lv_disp_drv_t *disp, const lv_area_t *area, lv_color_t *color_p) {
  uint32_t w = (area->x2 - area->x1 + 1);
  uint32_t h = (area->y2 - area->y1 + 1);

  tft.startWrite();
  tft.setAddrWindow(area->x1, area->y1, w, h);
  //tft.pushColors( ( uint16_t * )&color_p->full, w * h, true );
  tft.writePixels((lgfx::rgb565_t *)&color_p->full, w * h);
  tft.endWrite();

  lv_disp_flush_ready(disp);
}

/*Read the touchpad*/
void my_touchpad_read(lv_indev_drv_t *indev_driver, lv_indev_data_t *data) {
  uint16_t touchX, touchY;
  bool touched = tft.getTouch(&touchX, &touchY);
  if (!touched) {
    data->state = LV_INDEV_STATE_REL;
  } else {
    data->state = LV_INDEV_STATE_PR;

    /*Set the coordinates*/
    data->point.x = touchX;
    data->point.y = touchY;

    Serial.print("Data x ");
    Serial.println(touchX);

    Serial.print("Data y ");
    Serial.println(touchY);
  }
}

void setup() {
  Serial.begin(115200);
  
  tft.begin();
  tft.setRotation(2);
  tft.setBrightness(255);
  uint16_t calData[] = { 239, 3926, 233, 265, 3856, 3896, 3714, 308 };
  tft.setTouchCalibrate(calData);
  //touch_calibrate();//屏幕校准
  lv_init();
  lv_disp_draw_buf_init(&draw_buf, buf, NULL, screenWidth * 10);

  /*Initialize the display*/
  static lv_disp_drv_t disp_drv;
  lv_disp_drv_init(&disp_drv);

  /*Change the following line to your display resolution*/
  disp_drv.hor_res = screenWidth;
  disp_drv.ver_res = screenHeight;
  disp_drv.flush_cb = my_disp_flush;
  disp_drv.draw_buf = &draw_buf;
  lv_disp_drv_register(&disp_drv);

  /*Initialize the (dummy) input device driver*/
  static lv_indev_drv_t indev_drv;
  lv_indev_drv_init(&indev_drv);
  indev_drv.type = LV_INDEV_TYPE_POINTER;
  indev_drv.read_cb = my_touchpad_read;
  lv_indev_drv_register(&indev_drv);

  display_timer();
}

void loop() {
  lv_timer_handler(); /* let the GUI do its work */
  lv_task_handler();
  delay(5);
}

static void btn_event_cb(lv_event_t *e) {
  lv_event_code_t code = lv_event_get_code(e);
  lv_obj_t *btn = lv_event_get_target(e);
  if (code == LV_EVENT_CLICKED) {
    static uint8_t cnt = 0;
    cnt++;

    /*Get the first child of the button which is the label and change its text*/
    lv_obj_t *label = lv_obj_get_child(btn, 0);
    lv_label_set_text_fmt(label, "Button: %d", cnt);
  }
}

void display_timer() {
  // Create a label for the time.
  lv_obj_t *label = lv_label_create(lv_scr_act());
  lv_label_set_text(label, "00:00:00");
  lv_obj_align(label, LV_ALIGN_CENTER, 0, 0);
  // Set the update timer.
  lv_timer_create(update_timer, 1000, label);
}

void update_timer(lv_timer_t *t) {
  // Get the current time.
  lv_obj_t *label = (lv_obj_t *)t->user_data;

  struct tm *tm = localtime(&now);
  now++;
  char time_string[16];
  sprintf(time_string, "%02d:%02d:%02d %s", tm->tm_hour, tm->tm_min, tm->tm_sec, tm->tm_isdst == 1 ? "DST" : "");
  // Set the time label text.
  lv_label_set_text(label, time_string);
}
/**
 * Create a button with a label and react on click event.
 */
void lv_example_get_started_1(void) {
  lv_obj_t *btn = lv_btn_create(lv_scr_act()); /*Add a button the current screen*/
  lv_obj_set_size(btn, 120, 50);               /*Set its size*/
  lv_obj_align(btn, LV_ALIGN_CENTER, 0, 0);
  lv_obj_add_event_cb(btn, btn_event_cb, LV_EVENT_ALL, NULL); /*Assign a callback to the button*/

  lv_obj_t *label = lv_label_create(btn); /*Add a label to the button*/
  lv_label_set_text(label, "Button");     /*Set the labels text*/
  lv_obj_center(label);
}
