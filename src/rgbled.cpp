#include <rgbled.h>

unsigned long last_breath_time = 0;  // 记录上次呼吸效果更新时间
uint16_t breath_brightness = 512;     // 当前呼吸亮度值
bool breath_increasing = false;      // 亮度是否在增加

// Declare our NeoPixel strip object:
Adafruit_NeoPixel strip(LED_COUNT, LED_PIN, NEO_GRB + NEO_KHZ800);
// Argument 1 = Number of pixels in NeoPixel strip
// Argument 2 = Arduino pin number (most are valid)
// Argument 3 = Pixel type flags, add together as needed:
//   NEO_KHZ800  800 KHz bitstream (most NeoPixel products w/WS2812 LEDs)
//   NEO_KHZ400  400 KHz (classic 'v1' (not v2) FLORA pixels, WS2811 drivers)
//   NEO_GRB     Pixels are wired for GRB bitstream (most NeoPixel products)
//   NEO_RGB     Pixels are wired for RGB bitstream (v1 FLORA pixels, not v2)
//   NEO_RGBW    Pixels are wired for RGBW bitstream (NeoPixel RGBW products)

void RGB_init()
{
    strip.begin();           // INITIALIZE NeoPixel strip object (REQUIRED)
    strip.show();            // Turn OFF all pixels ASAP
    strip.setBrightness(255); // Set BRIGHTNESS to about 1/5 (max = 255)
}

  void RGB_Power(uint8_t i, float power)
  {
    // 如果总开关关闭或功率小于1瓦，LED不亮
    // 对于序号为0和1的LED，功率小于5瓦也不亮
    if (!led_enabled || power < 1.0 || (i <= 1 && power < 5.0)) {
      strip.setPixelColor(i, 0);  // 设置为黑色（关闭）
      strip.show();
      return;
    }

      // 呼吸效果亮度调整
      if (breath_increasing) {
        breath_brightness += 1;
        if (breath_brightness >= 510)
        {
          breath_brightness = 510; // 设置最大亮度为510
          breath_increasing = false;
        }
      } else {
        breath_brightness -= 1;
        if (breath_brightness <= 100)
        {
          breath_brightness = 100; // 设置最小亮度为100
          breath_increasing = true;
        }
      }
    
    // 根据灯珠号码和功率设置不同的最大功率限制
    float MAX_POWER;
    if (i < 2 && power >= 5.0) {
      MAX_POWER = 105.0; // 105瓦
    } else {
      MAX_POWER = 30.0;  // 30瓦
    }
    
    if (power > MAX_POWER) power = MAX_POWER;
    
    // 将功率映射到色相值
    // 色相: 42667(240°,蓝色,冷色) -> 0(0°,红色,暖色)
    // 功率: 1瓦 -> MAX_POWER瓦
    float ratio = (power - 1.0) / (MAX_POWER - 1.0);  // 计算功率在有效范围内的比例
    uint16_t hue = 42667 - (uint16_t)(ratio * 42667);  // 映射到色相值
    
    // 转换HSV到RGB颜色，应用呼吸效果的亮度
    uint32_t color = strip.gamma32(strip.ColorHSV(hue, 255, breath_brightness / 2));
    
    // 设置指定LED的颜色
    strip.setPixelColor(i, color);
    strip.show();
  }
  
  void RGB_clear()
  {
    strip.clear(); // Set all pixels in RAM to 0 (off)
    strip.show();  // Update strip to match
  }