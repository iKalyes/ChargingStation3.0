#ifndef _INA219_H
#define _INA219_H

#include <Arduino.h>
#include <lvgl.h>
#include "ui/ui.h"
#include <Adafruit_INA219.h>

#define SDA219_PIN 6
#define SCL219_PIN 7

void ina219_init();
void ina219_task(lv_timer_t *timer);

#endif