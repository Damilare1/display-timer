#include "WorldTimeApi.h"
#include "Api.h"

Api api;

time_t WorldTimeApi::get_epoch() {
  time_t epoch = 0;
  get_current_time_epoch(&epoch);
  return epoch;
}

void WorldTimeApi::get_current_time_epoch(time_t *epoch) {
  JSONVar data = get_current_time_data();
  if (JSON.typeof(data) == "undefined") {
    Serial.println("Parsing input failed!");
    return;
  }
  String str = data["datetime"];
  bool dst = (bool)data["dst"];
  *epoch = convertTimeStringToEpoch(str.c_str(), dst);
}

JSONVar WorldTimeApi::get_current_time_data() {
  String response = api.http_request("https://worldtimeapi.org/api/timezone/Europe/London", "/", "GET");
  return JSON.parse(response);
}

time_t WorldTimeApi::convertTimeStringToEpoch(const char *timeString, bool isDst) {
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