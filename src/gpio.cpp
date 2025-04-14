#include <gpio.h>

void gpio_init()
{
      pinMode(TYPE_C32, OUTPUT);
      pinMode(TYPE_C1, OUTPUT);
      pinMode(TYPE_A, OUTPUT);
      digitalWrite(TYPE_C32, LOW);
      digitalWrite(TYPE_C1, LOW);
      digitalWrite(TYPE_A, LOW);

      ledcSetup(1, 5000, 8);
      ledcAttachPin(FAN, 1);
      ledcWrite(1, 0); 
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

void USBC32_ON()
{
    USBC32_Switch = true;
    digitalWrite(TYPE_C32, HIGH);
}

void USBC32_OFF()
{
    USBC32_Switch = false;
    digitalWrite(TYPE_C32, LOW);
}

void USBC1_ON()
{
    USBC1_Switch = true;
    digitalWrite(TYPE_C1, HIGH);
}

void USBC1_OFF()
{
    USBC1_Switch = false;
    digitalWrite(TYPE_C1, LOW);
}

void FAN_ON()
{
    ledcWrite(1, 255);
    fan_switch = true;
}

void FAN_OFF()
{
    ledcWrite(1, 0);
    fan_switch = false;
}

void TempControl_Fan(float temperature)
{
    if(tempcontrol_fan == true && fan_switch == true)
    {
        if(temperature > 35.0f)
        {
            // 将占空比从 128 映射到 255，温度从 30 度映射到 40 度
            int dutyCycle = map(temperature, 35.0f, 45.0f, 128, 255);
            // 限制占空比在 128 到 255 之间
            dutyCycle = constrain(dutyCycle, 128, 255);
            Serial.println(dutyCycle);
            ledcWrite(1, dutyCycle);
        }
        else if(temperature >45.0f)
        {
            ledcWrite(1, 255);
        }
        else
        {
            ledcWrite(1, 0);
        }
    }
    else if(tempcontrol_fan == false && fan_switch == true)
    {
        ledcWrite(1, 255);
    }
    else
    {
        ledcWrite(1, 0);
    }
}