#include <gpio.h>

void gpio_init()
{
      pinMode(TYPE_CA, OUTPUT);
      pinMode(TYPE_CB, OUTPUT);
      pinMode(TYPE_A, OUTPUT);
      pinMode(FAN, OUTPUT);
      digitalWrite(TYPE_CA, LOW);
      digitalWrite(TYPE_CB, LOW);
      digitalWrite(TYPE_A, LOW);
}

void USBA_ON()
{
    USBA_Switch = true;
    digitalWrite(TYPE_A, HIGH);
}

void USBA_OFF()
{
    USBA_Switch = false;
    digitalWrite(TYPE_A, LOW);
}

void USBC_ON()
{
    USBC_Switch = true;
    digitalWrite(TYPE_CA, HIGH);
    digitalWrite(TYPE_CB, HIGH);
}

void USBC_OFF()
{
    USBC_Switch = false;
    digitalWrite(TYPE_CA, LOW);
    digitalWrite(TYPE_CB, LOW);
}

void FAN_ON()
{
    digitalWrite(FAN, HIGH);
    fan_switch = true;
}

void FAN_OFF()
{
    digitalWrite(FAN, LOW);
    fan_switch = false;
}