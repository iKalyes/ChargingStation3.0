#include <key.h>

static lv_indev_t* keypad_indev = NULL;

// 按键读取回调函数
static void keypad_read(lv_indev_drv_t* drv, lv_indev_data_t* data)
{
    static uint32_t last_key = 0;
    
    // 检测各按键状态
    if(!digitalRead(39)) {  // 右键按下
        data->state = LV_INDEV_STATE_PRESSED;
        data->key = LV_KEY_RIGHT;
        last_key = LV_KEY_RIGHT;
    }
    else if(!digitalRead(41)) {  // 左键按下
        data->state = LV_INDEV_STATE_PRESSED;
        data->key = LV_KEY_LEFT;
        last_key = LV_KEY_LEFT;
    }
    else if(!digitalRead(40)) {  // 确认键按下
        data->state = LV_INDEV_STATE_PRESSED;
        data->key = LV_KEY_ENTER;
        last_key = LV_KEY_ENTER;
    }
    else {
        data->state = LV_INDEV_STATE_RELEASED;
        data->key = last_key;
    }
}

// 初始化LVGL按键输入设备
void key_init()
{
    pinMode(39, INPUT); //右键
    pinMode(40, INPUT); //确认
    pinMode(41, INPUT); //左键
    
    // 注册LVGL输入设备
    static lv_indev_drv_t indev_drv;
    lv_indev_drv_init(&indev_drv);
    
    indev_drv.type = LV_INDEV_TYPE_KEYPAD;
    indev_drv.read_cb = keypad_read;
    
    keypad_indev = lv_indev_drv_register(&indev_drv);
}

// 获取按键输入设备指针
lv_indev_t* get_keypad_indev()
{
    return keypad_indev;
}