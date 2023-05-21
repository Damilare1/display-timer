#include "time.h"
#include <lvgl.h>
#define LGFX_USE_V1
#include <LovyanGFX.hpp>
#include <HTTPClient.h>
#include <WiFi.h>
#include <Arduino_JSON.h>
#include "WifiCredentials.h"
#include "LGFX.h"

const char *ssid = SSID;
const char *password = PASSWORD;

LGFX tft;

/*Change to your screen resolution*/
static const uint32_t screenWidth = 240;
static const uint32_t screenHeight = 320;
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
  now = get_epoch();
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

void get_current_time_epoch(time_t *epoch) {
  JSONVar data = get_current_time_data();
  if (JSON.typeof(data) == "undefined") {
    Serial.println("Parsing input failed!");
    return;
  }
  String str = data["datetime"];
  bool dst = (bool)data["dst"];
  *epoch = convertTimeStringToEpoch(str.c_str(), dst);
}

JSONVar get_current_time_data() {
  String response = http_request("https://worldtimeapi.org/api/timezone/Europe/London", "/", "GET");
  return JSON.parse(response);
}

time_t convertTimeStringToEpoch(const char *timeString, bool isDst) {
  uint16_t YEAR;
  uint8_t MONTH, DAY, HOUR, MINUTE, SECONDS, TZ_H, TZ_M;
  uint32_t MS;
  char tz_sign;
  uint8_t num_fields = sscanf(timeString, "%04hu-%02hhu-%02hhuT%02hhu:%02hhu:%02hhu.%06u%c%02hhu:%02hhu", &YEAR, &MONTH, &DAY, &HOUR, &MINUTE, &SECONDS, &MS, &tz_sign, &TZ_H, &TZ_M);

  struct tm t = { 0 };
  t.tm_hour = HOUR;
  t.tm_min = MINUTE;
  t.tm_sec = SECONDS;
  t.tm_mon = MONTH - 1;
  t.tm_mday = DAY;
  t.tm_year = YEAR - 1900;
  t.tm_isdst = isDst ? 1 : 0;
  char time_string[16];


  // time_t tz_offset = TZ_H * 3600 + TZ_M * 60;
  // if (tz_sign == '+') {
  //   tz_offset = -tz_offset;
  // }
  // time_t time = mktime(&t) + tz_offset;
  return mktime(&t);
}

time_t get_epoch() {
  time_t epoch = 0;
  get_current_time_epoch(&epoch);
  return epoch;
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
  Serial.print(WiFi.localIP());
}

bool wifi_is_connected(void) {
  return WiFi.status() == WL_CONNECTED;
}

String http_request(const char *host, const char *path, const char *method) {
  HTTPClient client;
  if (wifi_is_connected()) {
    Serial.println(method);
    Serial.print(" ");
    Serial.print(path);
    Serial.println(" HTTP/1.1");
    Serial.print("Host: ");
    Serial.println(host);
    client.begin(host);
    Serial.println(client.GET());
    // Read the response.
    if (client.GET() > 0) {
      return client.getString();
    }
  }

  return "";
}
