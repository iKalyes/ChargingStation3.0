#ifndef _WIFI_SERVICE_H
#define _WIFI_SERVICE_H

#include <Arduino.h>
#include <WiFi.h>
#include <WebServer.h>
#include <WiFiManager.h>
#include <esp_wifi.h>

#include <variables.h>
#include <main_service.h>
#include <display.h>
#include <lvgl_group.h>
#include <gpio.h>
#include <flash.h>
#include <lvgl.h>
#include <ui/ui.h>

void Webconfig();
void saveParamCallback();

void wificonfig();
void wificonnect();
void wifireset();

void sleep_unconnect();

#endif