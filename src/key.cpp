#include <key.h>

static lv_indev_t* keypad_indev = NULL;
// 添加一个变量跟踪slider是否被选中
static bool slider_selected = false;
// 添加一个变量来防止抖动
static unsigned long last_enter_press_time = 0;
static const unsigned long debounce_delay = 200;  // 200ms防抖延时

// 按键读取回调函数
static void keypad_read(lv_indev_drv_t* drv, lv_indev_data_t* data)
{
    static uint32_t last_key = 0;
    
    // 获取当前聚焦的对象
    lv_obj_t* focused_obj = NULL;
    lv_group_t* group = lv_group_get_default();
    if(group) {
        focused_obj = lv_group_get_focused(group);
    }
    
    bool is_slider = focused_obj ? lv_obj_check_type(focused_obj, &lv_slider_class) : false;
    
    // 检测各按键状态
    if(!digitalRead(39)) {  
        data->state = LV_INDEV_STATE_PRESSED;
        // 只有当slider被选中时才使用LEFT/RIGHT调整值
        if(is_slider && slider_selected) {
            data->key = LV_KEY_LEFT;  // 对slider使用LEFT键减小值
            last_key = LV_KEY_LEFT;
        } else {
            data->key = LV_KEY_PREV;  // 否则移动焦点
            last_key = LV_KEY_PREV;
        }
    }
    else if(!digitalRead(41)) {
        data->state = LV_INDEV_STATE_PRESSED;
        // 只有当slider被选中时才使用LEFT/RIGHT调整值
        if(is_slider && slider_selected) {
            data->key = LV_KEY_RIGHT;  // 对slider使用RIGHT键增加值
            last_key = LV_KEY_RIGHT;
        } else {
            data->key = LV_KEY_NEXT;  // 否则移动焦点
            last_key = LV_KEY_NEXT;
        }
    }
    else if(!digitalRead(40)) {  // 确认键按下
        data->state = LV_INDEV_STATE_PRESSED;
        data->key = LV_KEY_ENTER;
        last_key = LV_KEY_ENTER;
        
        // 防抖动检测
        if (millis() - last_enter_press_time > debounce_delay) {
            // 当确认键被按下且当前对象是slider时，切换slider的选中状态
            if(is_slider) {
                slider_selected = !slider_selected;
                if (slider_selected) {
                    lv_obj_add_state(focused_obj, LV_STATE_EDITED);
                } else {
                    lv_obj_clear_state(focused_obj, LV_STATE_EDITED);
                }
            }
            last_enter_press_time = millis();  // 记录本次按键时间
        }
    }
    else {
        data->state = LV_INDEV_STATE_RELEASED;
        data->key = last_key;
    }
}

// 初始化LVGL按键输入设备
void key_init()
{
    pinMode(39, INPUT);
    pinMode(40, INPUT);
    pinMode(41, INPUT);
    
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