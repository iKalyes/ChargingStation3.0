#ifndef _KEY_H
#define _KEY_H

#include <Arduino.h>
#include <variables.h>
#include <lvgl.h>
#include "ui/ui.h"

void key_init();
lv_indev_t* get_keypad_indev();

#endif