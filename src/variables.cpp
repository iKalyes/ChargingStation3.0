#include <variables.h>

//ADC ADJUST//
float voltage0_adc = 10.85f;
float voltage1_adc = 10.85f;
float temperature_adc = 0.1f;
//SEETING SCREEN//
uint8_t brightness = 0;
uint16_t sleep_time = 0;
bool fan_switch = false;
uint8_t free_space;
//MAIN SCREEN//
bool USBC_Switch = false;
bool USBA_Switch = false;
//FOR WEB AND WEATHER//
uint16_t UpdateWeater_Time = 10; //10分钟更新
String CityCode = "101280101"; //广州
String qWeather_ApiHost = "";
String qWeather_Key = "";
uint8_t TimeZone = 8;
float SyncTime = 1;
String NTPServer = "pool.ntp.org";
config_type wifisetting ={{""},{""}};
bool wificonfig_flag = false;