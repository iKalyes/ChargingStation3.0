#include <lvgl_group.h>

lv_group_t* group;

void lvgl_group_init()
{
    group = lv_group_create();
    lv_group_set_default(group);
    lv_indev_set_group(get_keypad_indev(), group);

    lv_group_add_obj(group, ui_USBCSwitch);
    lv_group_add_obj(group, ui_USBASwitch);
    lv_group_add_obj(group, ui_Weather);
    lv_group_add_obj(group, ui_Setting);
}

void lvgl_group_to_setting()
{
    lv_group_remove_all_objs(group);
    
    // 等待输入设备释放后再添加对象到组
    lv_indev_wait_release(get_keypad_indev());
    
    lv_group_add_obj(group, ui_WiFiWebPage);
    lv_group_add_obj(group, ui_AdvancedSetting);
    lv_group_add_obj(group, ui_FanSwitch);
    lv_group_add_obj(group, ui_SliderSleepTime);
    lv_group_add_obj(group, ui_SliderBrightness);
    lv_group_add_obj(group, ui_Back);
}

void lvgl_group_to_advancedsetting()
{
    lv_group_remove_all_objs(group);
    
    // 等待输入设备释放后再添加对象到组
    lv_indev_wait_release(get_keypad_indev());
    
    lv_group_add_obj(group, ui_USBA1Adjust);
    lv_group_add_obj(group, ui_USBA2Adjust);
    lv_group_add_obj(group, ui_ThermometerAdjust);
    lv_group_add_obj(group, ui_ThermometerControl);
    lv_group_add_obj(group, ui_AdvancedSettingBack);
}

void lvgl_group_to_wifi()
{
    lv_group_remove_all_objs(group);
    
    // 等待输入设备释放后再添加对象到组
    lv_indev_wait_release(get_keypad_indev());
    
    lv_group_add_obj(group, ui_WIFIStart);
    lv_group_add_obj(group, ui_WIFIReset);
    lv_group_add_obj(group, ui_WIFIBack);
}

void lvgl_group_to_weather()
{
    lv_group_remove_all_objs(group);
    
    // 等待输入设备释放后再添加对象到组
    lv_indev_wait_release(get_keypad_indev());
    
    lv_group_add_obj(group, ui_WeatherBack);
    lv_group_add_obj(group, ui_WeatherSetting);
}

void lvgl_group_to_main()
{
    lv_group_remove_all_objs(group);
    
    // 等待输入设备释放后再添加对象到组
    lv_indev_wait_release(get_keypad_indev());
    
    lv_group_add_obj(group, ui_USBCSwitch);
    lv_group_add_obj(group, ui_USBASwitch);
    lv_group_add_obj(group, ui_Weather);
    lv_group_add_obj(group, ui_Setting);
}