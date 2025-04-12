#ifndef _WEB_SERVER_H_
#define _WEB_SERVER_H_

#include <AsyncTCP.h>
#include <ESPAsyncWebServer.h>
#include <AsyncWebSocket.h>
#include <ArduinoJson.h>
#include <Arduino.h>

#include <lvgl.h>
#include "ui/ui.h"

#include <variables.h>
#include <ina219.h>
#include <ina3221_service.h>
#include <adc.h>
#include <gpio.h>
#include <flash.h>

void setTypeCOutput(bool enabled);
void setUSBAOutput(bool enabled);
void setFan(bool enabled);
void setFanTempControl(bool enabled);
void setChargeLed(bool enabled);
void sendSwitchStates(AsyncWebSocketClient *client = nullptr);
void toggleSwitch(const String& switchType);
void setSwitch(const String& switchType, bool state);
void setAllSwitches(bool state);
void handleWebSocketMessage(void *arg, uint8_t *data, size_t len);
void onEvent(AsyncWebSocket *server, AsyncWebSocketClient *client, AwsEventType type,
             void *arg, uint8_t *data, size_t len);
void setupWebSocket();
void setupWebServer();
void sendSensorData();
void web_server_run();

#endif
