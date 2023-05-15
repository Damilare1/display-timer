#include "Api.h"
#include <WiFi.h>
#include <HTTPClient.h>

const char *ssid = getenv("WIFI_SSID");
const char *password = getenv("WIFI_PASSWORD");

Api::Api(void)
{
    setup_wifi();
}

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

void Api::setup_wifi(void)
{
    WiFi.begin(ssid, password);
    while (WiFi.status() != WL_CONNECTED)
    {
        delay(500);
        Serial.print(".");
    }

    Serial.println("");
    Serial.println("WiFi connected");
    Serial.print("IP address: ");
    Serial.print(WiFi.localIP());
}

bool Api::wifi_is_connected(void)
{
    return WiFi.status() == WL_CONNECTED;
}
