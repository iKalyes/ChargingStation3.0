#include <ina219.h>

Adafruit_INA219 ina219;

float voltage_219;
float current_219;
float power_219;
lv_timer_t* ina219_timer;

void ina219_init()
{
  Wire1.begin(SDA219_PIN, SCL219_PIN);
  Wire1.setClock(400000);
  ina219.begin(&Wire1);
  ina219_timer = lv_timer_create(ina219_task, 100, NULL);
}

void ina219_task(lv_timer_t *timer)
{
    voltage_219 = ina219.getBusVoltage_V();
    current_219 = ina219.getCurrent_mA() / 1000 * 2;
    power_219 = ina219.getPower_mW() / 1000;

    // 电压：2位整数，2位小数
    int voltage_full = round(voltage_219 * 100);
    int voltage_int = voltage_full / 100;
    int voltage_frac = voltage_full % 100;
    
    // 电流：2位整数，2位小数
    int current_full = round(current_219 * 100);
    int current_int = current_full / 100;
    int current_frac = current_full % 100;
    
    // 功率：3位整数，1位小数
    int power_full = round(power_219 * 10);
    int power_int = power_full / 10;
    int power_frac = power_full % 10;
    
    if(voltage_219 >= 0 && current_219 >= 0 && power_219 >= 0)
    {
      lv_label_set_text_fmt(ui_Voltage, "%02d.%02dV", voltage_int, voltage_frac);
      lv_label_set_text_fmt(ui_Current, "%02d.%02dA", current_int, current_frac);
      lv_label_set_text_fmt(ui_Power, "%03d.%01dW", power_int, power_frac);
  
      lv_label_set_text_fmt(ui_WVoltage, "%02d.%02dV", voltage_int, voltage_frac);
      lv_label_set_text_fmt(ui_WCurrent, "%02d.%02dA", current_int, current_frac);
      lv_label_set_text_fmt(ui_WPower, "%03d.%01dW", power_int, power_frac);
    }
    else
    {
      lv_label_set_text(ui_Voltage, "00.00V");
      lv_label_set_text(ui_Current, "00.00A");
      lv_label_set_text(ui_Power, "00.00W");
  
      lv_label_set_text(ui_WVoltage, "00.00V");
      lv_label_set_text(ui_WCurrent, "00.00A");
      lv_label_set_text(ui_WPower, "000.0W");
    }
}