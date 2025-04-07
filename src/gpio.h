#ifndef _GPIO_H
#define _GPIO_H

#include <Arduino.h>
#include <variables.h>

#define TYPE_CA 15
#define TYPE_CB 16
#define TYPE_A 48
#define FAN 47

#define SWITCH_LEFT 39
#define SWITCH_ENTER 40
#define SWITCH_RIGHT 41

void gpio_init();
void USBA_ON();
void USBA_OFF();
void USBC_ON();
void USBC_OFF();
void FAN_ON();
void FAN_OFF();

#endif