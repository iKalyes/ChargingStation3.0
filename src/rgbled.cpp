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

// Some functions of our own for creating animated effects -----------------

// Fill strip pixels one after another with a color. Strip is NOT cleared
// first; anything there will be covered pixel by pixel. Pass in color
// (as a single 'packed' 32-bit value, which you can get by calling
// strip.Color(red, green, blue) as shown in the loop() function above),
// and a delay time (in milliseconds) between pixels.
void colorWipe(uint32_t color) {
    for(int i=0; i<strip.numPixels(); i++) { // For each pixel in strip...
      strip.setPixelColor(i, color);         //  Set pixel's color (in RAM)
      strip.show();                          //  Update strip to match
    }
  }
  
  // Theater-marquee-style chasing lights. Pass in a color (32-bit value,
  // a la strip.Color(r,g,b) as mentioned above), and a delay time (in ms)
  // between frames.
  void theaterChase(uint32_t color) {
    for(int a=0; a<10; a++) {  // Repeat 10 times...
      for(int b=0; b<3; b++) { //  'b' counts from 0 to 2...
        strip.clear();         //   Set all pixels in RAM to 0 (off)
        // 'c' counts up from 'b' to end of strip in steps of 3...
        for(int c=b; c<strip.numPixels(); c += 3) {
          strip.setPixelColor(c, color); // Set pixel 'c' to value 'color'
        }
        strip.show(); // Update strip with new contents
      }
    }
  }
  
  // Rainbow cycle along whole strip. Pass delay time (in ms) between frames.
  void rainbow() {
    // Hue of first pixel runs 5 complete loops through the color wheel.
    // Color wheel has a range of 65536 but it's OK if we roll over, so
    // just count from 0 to 5*65536. Adding 256 to firstPixelHue each time
    // means we'll make 5*65536/256 = 1280 passes through this loop:
    for(long firstPixelHue = 0; firstPixelHue < 5*65536; firstPixelHue += 256) {
      // strip.rainbow() can take a single argument (first pixel hue) or
      // optionally a few extras: number of rainbow repetitions (default 1),
      // saturation and value (brightness) (both 0-255, similar to the
      // ColorHSV() function, default 255), and a true/false flag for whether
      // to apply gamma correction to provide 'truer' colors (default true).
      strip.rainbow(firstPixelHue);
      // Above line is equivalent to:
      // strip.rainbow(firstPixelHue, 1, 255, 255, true);
      strip.show(); // Update strip with new contents
    }
  }
  
  // Rainbow-enhanced theater marquee. Pass delay time (in ms) between frames.
  void theaterChaseRainbow() {
    int firstPixelHue = 0;     // First pixel starts at red (hue 0)
    for(int a=0; a<30; a++) {  // Repeat 30 times...
      for(int b=0; b<3; b++) { //  'b' counts from 0 to 2...
        strip.clear();         //   Set all pixels in RAM to 0 (off)
        // 'c' counts up from 'b' to end of strip in increments of 3...
        for(int c=b; c<strip.numPixels(); c += 3) {
          // hue of pixel 'c' is offset by an amount to make one full
          // revolution of the color wheel (range 65536) along the length
          // of the strip (strip.numPixels() steps):
          int      hue   = firstPixelHue + c * 65536L / strip.numPixels();
          uint32_t color = strip.gamma32(strip.ColorHSV(hue)); // hue -> RGB
          strip.setPixelColor(c, color); // Set pixel 'c' to value 'color'
        }
        strip.show();                // Update strip with new contents
        firstPixelHue += 65536 / 90; // One cycle of color wheel over 90 frames
      }
    }
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