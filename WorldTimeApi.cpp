#include "WorldTimeApi.h"
#include "Api.h"
#include <HTTPClient.h>
#include <Arduino_JSON.h>

Api api;

void WorldTimeApi::get_time_data(void (*callback)(JSONVar response, time_t *epoch), time_t *epoch)
{
    String response = api.http_request("https://worldtimeapi.org/api/timezone/Europe/London", "/", "GET");
    if (!response.equals(""))
    {
        callback(JSON.parse(response), epoch);
    }
}

void WorldTimeApi::handle_response(JSONVar response, time_t* epoch)
{
    *epoch = parseData(response);
}

time_t WorldTimeApi::parseData(JSONVar response)
{
    if (JSON.typeof(response) == "undefined")
    {
        Serial.println("Parsing input failed!");
        return 0;
    }

    // Expected format: "2023-05-13T13:14:17.961294+01:00"
    String str = response["datetime"];
    bool dst = (bool)response["dst"];

    return convertTimeStringToEpoch(str.c_str(), dst);
}

time_t WorldTimeApi::convertTimeStringToEpoch(const char *timeString, bool isDst)
{
    unsigned int YEAR, MONTH, DAY, HOUR, MINUTE, SECONDS, MS, TZ_H, TZ_M;
    char tz_sign;
    int num_fields = sscanf(timeString, "%04u-%02u-%02uT%02u:%02u:%02u.%06u%c%02u:%02u", &YEAR, &MONTH, &DAY, &HOUR, &MINUTE, &SECONDS, &MS, &tz_sign, &TZ_H, &TZ_M);
    struct tm t = {0};
    t.tm_hour = HOUR;
    t.tm_min = MINUTE;
    t.tm_sec = SECONDS;
    t.tm_mon = MONTH - 1;
    t.tm_mday = DAY;
    t.tm_year = YEAR - 1900;
    t.tm_isdst = isDst ? 1 : 0;
    // time_t tz_offset = TZ_H * 3600 + TZ_M * 60;
    // if (tz_sign == '+') {
    //   tz_offset = -tz_offset;
    // }
    // time_t time = mktime(&t) + tz_offset;
    return mktime(&t);
}

time_t WorldTimeApi::get_epoch()
{
    time_t epoch = 0;
    get_time_data(handle_response, &epoch);
    return epoch;
}