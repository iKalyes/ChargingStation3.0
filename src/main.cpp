#include <main.h>
#include <INA3221.h>

INA3221 INA(0x43, &Wire);

void setup()
{
    Serial.begin( 115200 ); /* prepare for possible serial debug */
    gpio_init(); /* Initialize the GPIO */
    flash_init(); /* Initialize the flash */

    display_init(); /* Initialize the display and UI */
    ina219_init(); /* Initialize the INA219 sensor */
    key_init(); /* Initialize the encoder input device */
    lvgl_group_init(); /* Initialize the LVGL group */

    load_web_config(); /* Load the web configuration */

    wificonnect(); /* Connect to the wifi */
    time_server_init(NTPServer.c_str(), TimeZone, SyncTime); /* Initialize the time server */
    weather_init(qWeather_Key, CityCode, qWeather_ApiHost); /* Initialize the weather server */

    Wire.begin(5, 4);
    Wire.setClock(400000);
    if (INA.begin() == false)
    {
        Serial.println("could not connect.");
    }
    else
    {
        Serial.print(INA.getDieID(), HEX);
        Serial.print("\t");
        Serial.print(INA.getManufacturerID(), HEX);
        Serial.print("\t");
        Serial.print(INA.getConfiguration(), HEX);
        Serial.println();
    }
    INA.setShuntR(0, 0.010);
    INA.setShuntR(1, 0.010);
    INA.setShuntR(2, 0.010);
}

int time_stamp = 0;
void loop()
{
    display_task(); /* let the GUI do its work */
    time_server_update();
    wificonfig();
    deep_sleep();

    if(millis() - time_stamp > 100)
    {
        time_stamp = millis();
        Serial.println("CHAN\tBUS\tSHUNT\tCURRENT\tPOWER");
        for (int channel = 0; channel < 3; channel++)
        {
          Serial.print(channel);
          Serial.print("\t");
          Serial.print(INA.getBusVoltage(channel), 3);
          Serial.print("\t");
          Serial.print(INA.getShuntVoltage_mV(channel), 3);
          Serial.print("\t");
          Serial.print(INA.getCurrent_mA(channel) / 1000, 3);
          Serial.print("\t");
          Serial.print(INA.getPower_mW(channel) / 1000, 3);
          Serial.println();
        }
        Serial.println();
    }
}
