#ifndef WorldTimeApi_h
#define WorldTimeApi_h

#include <Arduino_JSON.h>

class WorldTimeApi {

public:
  static void get_time_data(void (*callback)(JSONVar response, time_t *epoch), time_t *epoch);
  static time_t get_epoch(void);

private:
  static void handle_response(JSONVar response, time_t *epoch);
  static time_t convertTimeStringToEpoch(const char *timeString, bool isDst);
  static time_t parseData(JSONVar response);
};

#endif