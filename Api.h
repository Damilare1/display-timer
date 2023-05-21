#ifndef Api_h
#define Api_h
#include <HTTPClient.h>

class Api {

public:
  String http_request(const char *host, const char *path, const char *method);

private:
  bool wifi_is_connected(void);

};


#endif