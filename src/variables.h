#ifndef _VARIABLES_H
#define _VARIABLES_H

#include <Arduino.h>
#include <lvgl.h>
//ADC ADJUST//
extern float voltage0_adc;
extern float voltage1_adc;
extern float temperature_adc;
//SEETING SCREEN//
extern uint8_t brightness;
extern uint16_t sleep_time;
extern bool fan_switch;
extern uint8_t free_space;
//MAIN SCREEN//
extern bool USBC_Switch;
extern bool USBA_Switch;
//FOR WEB AND WEATHER//
extern uint16_t UpdateWeater_Time;
extern String CityCode;
extern String qWeather_Key;
extern String qWeather_ApiHost;
extern uint8_t TimeZone;
extern float SyncTime;
extern String NTPServer;
struct config_type
{
    char sta_ssid[32];
    char sta_pwd[64];
};
extern config_type wifisetting;
extern bool wificonfig_flag;

#endif