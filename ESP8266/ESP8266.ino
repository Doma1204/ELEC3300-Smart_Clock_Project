#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <WiFiClient.h>

extern "C" {
    #include "user_interface.h"
    #include "wpa2_enterprise.h"
}

#include "NTPClock.h"

static const char* ssid = "ssid"; // enter your ssid
static const char* username = "username"; // enter your username
static const char* password = "password"; // enter you password

NTPClock ntpClock;
bool isWiFiReady = false;

uint8_t cur_icon = 0;
uint8_t temp[27] = {0};

#define FORECAST_DAY 3
uint8_t fore_icon[FORECAST_DAY], fore_max[FORECAST_DAY], fore_min[FORECAST_DAY];

void setup() {
    Serial.begin(115200);
    pinMode(LED_BUILTIN, OUTPUT);
    delay(100);
    
    wifi_set_opmode(STATION_MODE);

    struct station_config wifi_config;
    memset(&wifi_config, 0, sizeof(wifi_config));
    strcpy((char*)wifi_config.ssid, ssid);
    
    wifi_station_set_config(&wifi_config);
    
    wifi_station_clear_cert_key();
    wifi_station_clear_enterprise_ca_cert();
    wifi_station_set_wpa2_enterprise_auth(1);
    wifi_station_set_enterprise_identity((uint8*)username, strlen(username));
    wifi_station_set_enterprise_username((uint8*)username, strlen(username));
    wifi_station_set_enterprise_password((uint8*)password, strlen(password));
    wifi_station_connect();

    while(WiFi.status() != WL_CONNECTED) {
        delay(100);
        ledToggle();
        respondSTM();
    }
    
    ntpClock.init();
    hko_get_current_temperature();
    hko_get_forecast();

    isWiFiReady = true;
    digitalWrite(LED_BUILTIN, HIGH);
}

void loop() {
    ntpClock.update();

    static uint32_t last_weather_ticks = 0;
    if (millis() - last_weather_ticks > 1800000) { // half an hour
        hko_get_current_temperature();
        hko_get_forecast();
        last_weather_ticks = millis();
    }
    
    respondSTM();
}

void ledToggle(void) {
    static bool ledOn;
    ledOn = !ledOn;
    digitalWrite(LED_BUILTIN, ledOn);
}

void hko_get_current_temperature(void) {
    HTTPClient http;
    http.begin("http://data.weather.gov.hk/weatherAPI/opendata/weather.php?dataType=rhrread&lang=en");
    int code = http.GET();

    if (code > 0) {
        String cur_temp = http.getString();
        int icon_index = cur_temp.indexOf("icon");
        String icon_txt = cur_temp.substring(icon_index + 7, icon_index + 9);
        icon_txt.replace("]", "");
        cur_icon = icon_txt.toInt();

        cur_temp = cur_temp.substring (cur_temp.indexOf("temperature"), cur_temp.indexOf("warningMessage"));
        int index = cur_temp.indexOf("value");
        uint8_t i = 0;
        while (index != -1) {
            String local_temp = cur_temp.substring(index + 7, index + 9);
            local_temp.replace(",", "");
            temp[i++] = local_temp.toInt();
            cur_temp = cur_temp.substring(index + 10);
            index = cur_temp.indexOf("value");
        }
    }
}

void hko_get_forecast(void) {
    HTTPClient http;
    http.begin("http://data.weather.gov.hk/weatherAPI/opendata/weather.php?dataType=fnd&lang=en");
    int code = http.GET();

    if (code > 0) {
        String forecast = http.getString();
        Serial.println();
        for (uint8_t i = 0; i < FORECAST_DAY; ++i) {
            int index = forecast.indexOf("forecastMaxtemp");
            String max = forecast.substring(index + 26, index + 28);
            max.replace(",", "");
            fore_max[i] = max.toInt();
            // Serial.print(max); Serial.print(" ");
            
            index = forecast.indexOf("forecastMintemp");
            String min = forecast.substring(index + 26, index + 28);
            min.replace(",", "");
            fore_min[i] = min.toInt();
            // Serial.print(min); Serial.print(" ");

            index = forecast.indexOf("ForecastIcon");
            String icon = forecast.substring(index + 14, index + 16);
            icon.replace("}", "");
            fore_icon[i] = icon.toInt();
            // Serial.println(icon);

            forecast = forecast.substring(index + 16);
        }
    }
}

typedef union _BUFFER32 {
    uint8_t buffer[4];
    uint32_t value;
} BUFFER32;

#define stmBuff_LENGTH 4
void respondSTM(void) {
    static uint8_t stmBuff[stmBuff_LENGTH];
    static uint8_t index = 0;
    while(Serial.available()) {
        char data = Serial.read();
        if (data == '\n') {
            Serial.write('\r');
            Serial.write(stmBuff[0]);
            switch (stmBuff[0]) {
                case 'R':
                    Serial.write(isWiFiReady ? 1 : 0);
                    break;
                    
                case 'T':
                    BUFFER32 curTime;
                    curTime.value = now();
                    Serial.write(curTime.buffer, 4);
                    break;
    
                case 'C':
                    Serial.write(temp[stmBuff[1]]);
                    Serial.write(cur_icon);
                    break;
    
                case 'F':
                    Serial.write(fore_min, FORECAST_DAY);
                    Serial.write(fore_max, FORECAST_DAY);
                    Serial.write(fore_icon, FORECAST_DAY);
                    break;
                
                default:
                    Serial.write(0);
                    break;
            }
            Serial.print('\n');
            index = 0;
        } else if (index != stmBuff_LENGTH) {
            stmBuff[index++] = data;
        }
    }    
}
