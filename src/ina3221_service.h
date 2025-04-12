#ifndef _INA3221_SERVICE_H_
#define _INA3221_SERVICE_H_

#include <Arduino.h>
#include <lvgl.h>
#include "ui/ui.h"
#include <INA3221.h>
#include <rgbled.h>

#define SDA3221_PIN 5
#define SCL3221_PIN 4

void ina3221_init();
void ina3221_task(lv_timer_t *timer);
void rgbled_task(lv_timer_t *timer);
void ina3221_process(int channel, float voltage, float current, float power);

float ina3221_get_voltage(int channel);
float ina3221_get_current(int channel);
float ina3221_get_power(int channel);

#endif