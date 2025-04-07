#ifndef _ADC_H
#define _ADC_H

#define ADC0_PIN 1
#define ADC1_PIN 2
#define ADC2_PIN 3

#define R_NTC 100000
#define R_DIV 10000
#define NTC_REF_TEMP 25


#include <Arduino.h>
#include <variables.h>
#include <lvgl.h>
#include "ui/ui.h"

void adc_init();
void adc_task(lv_timer_t *timer);

#endif
