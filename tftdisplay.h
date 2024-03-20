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
    void mainMenu();
    void wifiandmqttmenu();
    void selectwifiandmqttmenu(int selection);
    void selectmenu(int selection);
    void displayKeyboard(int number);
    void selectChar(int selection);
    void displayStatus(const int co2, const int temp, const int hum, const int fan, const int AP, const uint8_t *wifi_icon);
    void displaychar(char c, int x, int y);
    void displayString(const std::string& string);
    void displayString(const char* string);
    void displayMQTTandWifiMenu(int number);
    void credenttialsMenu();
    void selctcredenttials(int selection);
    void errorScreen();
    void connecting();
    void notconnected();
    const uint8_t wifi_signal[10] =
            {// font edit begin : monovlsb : 10 : 7
                    0x08, 0x04, 0x12, 0x09, 0x65, 0x65, 0x09, 0x12,
                    0x04, 0x08
// font edit end
            };


private:
    std::array<char, 26> alphabet = {'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H', 'I', 'J', 'K', 'L', 'M', 'N', 'O', 'P', 'Q', 'R', 'S', 'T', 'U', 'V', 'W', 'X', 'Y', 'Z'};
    std::array<char, 26> lowercase_alphabet = {'a', 'b', 'c', 'd', 'e', 'f', 'g', 'h', 'i', 'j', 'k', 'l', 'm', 'n', 'o', 'p', 'q', 'r', 's', 't', 'u', 'v', 'w', 'x', 'y', 'z'};
    std::array<char, 36> special_chars_numbers = {'0', '1', '2', '3', '4', '5', '6', '7', '8', '9',
                                                  '!', '@', '#', '$', '%', '^', '&', '*', '(', ')',
                                                  '-', '_', '=', '+', '[', ']', '{', '}', ';', ':',
                                                  '\'', '\"', ',', '.', '<', '>'};
    uint16_t width;

};


#endif //VENTILATION_DEVICE_TFTDISPLAY_H
