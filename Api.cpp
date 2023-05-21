#include "Api.h"
#include <HTTPClient.h>


String Api::http_request(const char *host, const char *path, const char *method)
{
    HTTPClient client;
    if (wifi_is_connected())
    {
        Serial.println(method);
        Serial.print(" ");
        Serial.print(path);
        Serial.println(" HTTP/1.1");
        Serial.print("Host: ");
        Serial.println(host);
        Serial.println();
        client.begin(host);
        Serial.println(client.GET());
        // Read the response.
        if(client.GET() > 0) {
          return client.getString();
        }
    }
    
    return "";
}


bool Api::wifi_is_connected(void)
{
    return WiFi.status() == WL_CONNECTED;
}
