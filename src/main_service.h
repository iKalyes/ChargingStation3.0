#ifndef _MAIN_SERVICE_H_
#define _MAIN_SERVICE_H_

#include <Arduino.h>
#include <NTPClient.h>
#include <WiFiUdp.h>
#include <WiFi.h>
#include <esp_wifi.h>

#include <variables.h>
#include <display.h>
#include <weather.h>
#include <gpio.h>
#include <lvgl.h>
#include "ui/ui.h"

void time_server_init(const char* poolServerName, long timeOffset, float updateInterval);
void time_server_setting(const char* poolServerName, long timeOffset, float updateInterval);
void time_server_setsynctime(float updateInterval);
void time_server_refresh(lv_timer_t *timer);
void time_server_update();
void time_server_reconnect();
void time_server_forceupdate();

void weather_init(String apiKey, String location, String ApiHost);
void weather_update();

void sleep();
void reset_sleep_EpochTime();

#endif