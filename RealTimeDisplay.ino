#include "time.h"
#include <lvgl.h>
#define LGFX_USE_V1
#include <LovyanGFX.hpp>
#include <WiFi.h>
#include "WifiCredentials.h"
#include "LGFX.h"
#include "WorldTimeApi.h"

const char *ssid = SSID;
const char *password = PASSWORD;

LGFX tft;
WorldTimeApi wApi;

/*Change to your screen resolution*/
static const uint16_t screenWidth = 240;
static const uint16_t screenHeight = 320;
time_t now;
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
  setup_wifi();
  now = wApi.get_epoch();
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

void setup_wifi(void) {
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("");
  Serial.println("WiFi connected");
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());
}
