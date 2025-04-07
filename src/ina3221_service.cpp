#include <ina3221_service.h>

INA3221 INA(0x43, &Wire);

float voltage_3221[3];
float current_3221[3];
float power_3221[3];
lv_timer_t* ina3221_timer;

void ina3221_init()
{
    Wire.begin(SDA3221_PIN, SCL3221_PIN);
    Wire.setClock(400000);
    INA.begin();
    INA.setShuntR(0, 0.010);
    INA.setShuntR(1, 0.010);
    INA.setShuntR(2, 0.010);
    ina3221_timer = lv_timer_create(ina3221_task, 100, NULL);
}

void ina3221_task(lv_timer_t *timer)
{
    for (int channel = 0; channel < 3; channel++)
    {
        voltage_3221[channel] = INA.getBusVoltage(channel);
        current_3221[channel] = INA.getCurrent(channel);
        power_3221[channel] = INA.getPower(channel);
        
        ina3221_process(channel + 1, voltage_3221[channel], current_3221[channel], power_3221[channel]);
    }
    
}

void ina3221_process(int channel, float voltage, float current, float power)
{

    // 电压：2位整数，2位小数
    int voltage_full = round(voltage * 100);
    int voltage_int = voltage_full / 100;
    int voltage_frac = voltage_full % 100;
    
    // 电流：1位整数，2位小数
    int current_full = round(current * 1000);
    int current_int = current_full / 1000;
    int current_frac = current_full % 1000;
    
    // 功率：3位整数，1位小数
    int power_full = round(power * 10);
    int power_int = power_full / 10;
    int power_frac = power_full % 10;

    switch (channel)
    {
    case 1:
        lv_label_set_text_fmt(ui_VoltageC1, "%02d.%02dV", voltage_int, voltage_frac);
        lv_label_set_text_fmt(ui_CurrentC1, "%01d.%03dA", current_int, current_frac);
        lv_label_set_text_fmt(ui_PowerC1, "%03d.%01dW", power_int, power_frac);
        break;
    case 2:
        lv_label_set_text_fmt(ui_VoltageC2, "%02d.%02dV", voltage_int, voltage_frac);
        lv_label_set_text_fmt(ui_CurrentC2, "%01d.%03dA", current_int, current_frac);
        lv_label_set_text_fmt(ui_PowerC2, "%03d.%01dW", power_int, power_frac);
        break;
    case 3:
        lv_label_set_text_fmt(ui_VoltageC3, "%02d.%02dV", voltage_int, voltage_frac);
        lv_label_set_text_fmt(ui_CurrentC3, "%01d.%03dA", current_int, current_frac);
        lv_label_set_text_fmt(ui_PowerC3, "%03d.%01dW", power_int, power_frac);
        break;
    
    default:
        break;
    }
}
