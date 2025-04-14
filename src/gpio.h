#ifndef _GPIO_H
#define _GPIO_H

#include <Arduino.h>
#include <variables.h>

#define TYPE_C32 15
#define TYPE_C1 16
#define TYPE_A 48
#define FAN 47

#define SWITCH_LEFT 39
#define SWITCH_ENTER 40
#define SWITCH_ENTER_NUM GPIO_NUM_40
#define SWITCH_RIGHT 41

void gpio_init();
void USBA_ON();
void USBA_OFF();
void USBC32_ON();
void USBC32_OFF();
void USBC1_ON();
void USBC1_OFF();
void FAN_ON();
void FAN_OFF();

void TempControl_Fan(float temperature);

#endif