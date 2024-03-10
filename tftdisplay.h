//
// Created by iamna on 09/03/2024.
//

#ifndef VENTILATION_DEVICE_TFTDISPLAY_H
#define VENTILATION_DEVICE_TFTDISPLAY_H

#include "ssd1306.h"
#include "pico/stdlib.h"
#include "hardware/i2c.h"
#include "hardware/gpio.h"

class TFTDisplay : public ssd1306{
public:
    TFTDisplay(uint16_t sda, uint16_t scl, i2c_inst *i2c);
    void displayText(const char* text, const uint8_t* font);
    void displayMenu(const char* textA, const char* textB);
    void displayControlPressureLevel(uint16_t pressure);
    void displayControlSpeed(uint16_t speed);
    void welcomeScreen();
    void autoOrManual();
    void loginscreen();
    void connectTowifi();
    void selectmenu(int selection);
    void displayKeyboard(int number);
    void selectChar(int selection);
    void displayStatus(const int co2, const int temp, const int hum, const int fan, const int AP, const uint8_t *wifi_icon);

        private:
   uint16_t width;
};


#endif //VENTILATION_DEVICE_TFTDISPLAY_H
