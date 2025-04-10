#ifndef _RGBLED_H_
#define _RGBLED_H_

#include <Adafruit_NeoPixel.h>
#include <Arduino.h>
#include <variables.h>

// Which pin on the Arduino is connected to the NeoPixels?
// On a Trinket or Gemma we suggest changing this to 1:
#define LED_PIN 42

// How many NeoPixels are attached to the Arduino?
#define LED_COUNT 3

void RGB_init();
void RGB_Power(uint8_t i, float power);
void RGB_clear();
void theaterChaseRainbow();
void rainbow(uint8_t wait);
void rainbowCycle(uint8_t wait);
void theaterChase(uint32_t c, uint8_t wait);

#endif