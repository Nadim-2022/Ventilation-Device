//
// Created by iamna on 01/03/2024.
//

#ifndef PICO_MODBUS_I2C_DISPLAY_H
#define PICO_MODBUS_I2C_DISPLAY_H
#include "ssd1306.h"
#include "pico/stdlib.h"
#include "hardware/i2c.h"
#include "hardware/gpio.h"

class I2C_Display {
public:
    I2C_Display(uint8_t sda, uint8_t scl, i2c_inst_t *i2c);
    void displayText(const char *text, const uint8_t *raspberry26x32);
    void welcomeScreen();
    void autoOrManual();
    void loginscreen();
    void keyboard();
    void existinguser();
    void connecting();
    void displayMenu(const char *textA, const char *textB);
    void displayControlPressureLevel(uint16_t pressure);
    void displayControlSpeed(uint16_t speed);
    void test(uint16_t letter);
    void displayStatus(const int co2, const int temp, const int hum, const int fan, const int ap, const uint8_t *wifi_icon);
    void displayKeyboard();


private:
    std::shared_ptr<ssd1306> display;
    uint16_t width;
};



#endif //PICO_MODBUS_I2C_DISPLAY_H
