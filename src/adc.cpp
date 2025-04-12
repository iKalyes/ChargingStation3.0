#include <adc.h>

lv_timer_t* adc_timer;
float voltage0;
float voltage1;
float temperature;
float mcu_temperature;

// 添加移动平均滤波器的参数
const int TEMPERATURE_SAMPLES = 5; // 采样数量
float temperature_buffer[TEMPERATURE_SAMPLES]; // 存储温度的缓冲区
int temperature_index = 0; // 缓冲区索引
float temperature_sum = 0; // 温度总和
float temperature_average = 0; // 平均温度

void adc_init()
{
    analogReadResolution(12);
    adc_timer = lv_timer_create(adc_task, 200, NULL);

    // 初始化温度缓冲区
    for (int i = 0; i < TEMPERATURE_SAMPLES; i++) {
        temperature_buffer[i] = 25.0f; // 初始温度设置为25度
        temperature_sum += temperature_buffer[i];
    }
    temperature_average = temperature_sum / TEMPERATURE_SAMPLES;
}

float beta = 3950.0f; // NTC的B值
// 读取和平滑处理
void readVoltages() {
    // 读取原始值
    voltage0 = voltage0_adc * (float)(analogReadMilliVolts(ADC0_PIN)) * 1e-3;
    voltage1 = voltage1_adc * (float)(analogReadMilliVolts(ADC1_PIN)) * 1e-3;

    float ntc_voltage = analogReadMilliVolts(ADC2_PIN) * 3.3 / 4095.0;
    float r_ntc = R_DIV * ntc_voltage / (3.3 - ntc_voltage);
    float steinhart = r_ntc / R_NTC;
    steinhart = log(steinhart);
    steinhart /= beta;
    steinhart += 1.0 / (NTC_REF_TEMP + 273.15);
    steinhart = 1.0 / steinhart;
    temperature = temperature_adc * (steinhart - 273.15);
    // 更新移动平均滤波器
    temperature_sum -= temperature_buffer[temperature_index]; // 减去旧值
    temperature_sum += temperature; // 加上新值
    temperature_buffer[temperature_index] = temperature; // 存储新值
    temperature_index = (temperature_index + 1) % TEMPERATURE_SAMPLES; // 更新索引
    temperature_average = temperature_sum / TEMPERATURE_SAMPLES; // 计算平均值
}

void adc_task(lv_timer_t *timer)
{
    readVoltages();
    TempControl_Fan(temperature_average);
    
    int voltage0_full = round(voltage0 * 100);
    int voltage0_int = voltage0_full / 100;
    int voltage0_frac = voltage0_full % 100;
    
    int voltage1_full = round(voltage1 * 100);
    int voltage1_int = voltage1_full / 100;
    int voltage1_frac = voltage1_full % 100;;

    int temperature_full = round(temperature_average  * 100);
    int temperature_int = temperature_full / 100;
    int temperature_frac = temperature_full % 100;;
    
    lv_label_set_text_fmt(ui_VoltageUSBA1, "%02d.%02dV", voltage0_int, voltage0_frac);
    lv_label_set_text_fmt(ui_VoltageUSBA2, "%02d.%02dV", voltage1_int, voltage1_frac);

    lv_label_set_text_fmt(ui_VoltageUSBA1ADC, "%02d.%02dV", voltage0_int, voltage0_frac);
    lv_label_set_text_fmt(ui_VoltageUSBA2ADC, "%02d.%02dV", voltage1_int, voltage1_frac);

    lv_label_set_text_fmt(ui_SysTemp, "%02d.%02d℃", temperature_int, temperature_frac);
    lv_label_set_text_fmt(ui_SysTempAdjust, "%02d.%02d℃", temperature_int, temperature_frac);
}

float adc_get_voltage0()
{
    return voltage0;
}
float adc_get_voltage1()
{
    return voltage1;
}
float adc_get_temperature()
{
    return temperature_average;
}