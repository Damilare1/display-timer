#ifndef WorldTimeApi_h
#define WorldTimeApi_h

#include <HTTPClient.h>
#include <Arduino_JSON.h>

class WorldTimeApi {

public:
  static time_t get_epoch(void);

private:
  static void get_current_time_epoch(time_t *epoch);
  static JSONVar get_current_time_data();
  static time_t convertTimeStringToEpoch(const char *timeString, bool isDst);
};

#endif