#ifndef Api_h
#define Api_h
#include <HTTPClient.h>

class Api {

public:
  Api(void);


String http_request(const char *host, const char *path, const char *method);

void setup_wifi(void);

bool wifi_is_connected(void);

};


#endif