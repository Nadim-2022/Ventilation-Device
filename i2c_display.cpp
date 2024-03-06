//
// Created by iamna on 01/03/2024.
//

#include "i2c_display.h"


I2C_Display::I2C_Display(uint8_t sda, uint8_t scl, i2c_inst_t *i2c) {
    i2c_init(i2c, 100000);
    gpio_set_function(sda, GPIO_FUNC_I2C);
    gpio_set_function(scl, GPIO_FUNC_I2C);
    gpio_pull_up(sda);
    gpio_pull_up(scl);
    width = 0;
    display = std::make_shared<ssd1306>(i2c);
}

void I2C_Display::displayText(const char *text, const uint8_t *raspberry26x32) {
    display->fill(0);
    display->text(text, 0, 0);
    mono_vlsb rb(raspberry26x32, 20, 20);
    display->blit(rb, 30, 30);
    display->show();

}

void I2C_Display::displayMenu(const char *textA, const char *textB) {
    display->fill(0);
    display->text(textA, 0, 30);
    display->text(textB, 0, 40);
    display->show();
}
void I2C_Display::displayControlPressureLevel(uint16_t pressure) {
    std::string APmessage = std::to_string(pressure) + " Pa";
    width = (pressure * 128) /120;
    display->fill(0);
    display->text("Air Pressure: ", 0, 0);
    display->text(APmessage, 45, 20);
    display->rect(0, 40,width , 20, 1, true);
    display->show();
}

void I2C_Display::displayControlSpeed(uint16_t speed) {
    std::string FSmessage = std::to_string(speed) + " %";
    width = (speed * 128) / 100;
    display->fill(0);
    display->text("Fan Speed: ", 0, 0);
    display->text(FSmessage, 45, 20);
    display->rect(0, 40,width , 20, 1, true);
    display->show();
}

void I2C_Display::welcomeScreen() {
    display->fill(0);
    display->text("Welcome to Group", 0, 22);
    display->text("3's Ventilation", 0, 32);
    display->text("Project", 38, 42);
    display->show();
}

void I2C_Display::autoOrManual() {
    display->fill(0);
    display->text("Automatic", 44, 16);
    display->text("Manual", 44, 32);
    display->show();
}

void I2C_Display::connecting() {
    display->fill(0);
    display->text("Connecting", 0, 32);
    display->show();
    sleep_ms(500);
    display->text("Connecting.", 0, 32);
    display->show();
    sleep_ms(500);
    display->fill(0);
    display->text("Connecting..", 0, 32);
    display->show();
    sleep_ms(500);
    display->fill(0);
    display->text("Connecting...", 0, 32);
    display->show();
    sleep_ms(500);
}

void I2C_Display::displayStatus(const int co2 = 0, const int temp = 0, const int hum = 0, const int fan = 0, const int AP = 0) {
    std::string fs = "FS  : " + std::to_string(fan) + " %";
    std::string ap = "AP  : " + std::to_string(AP) + " Pa";
    std::string Co2 = "Co2 : " + std::to_string(co2) + " ppm";
    std::string tmp = "Temp: " + std::to_string(temp) + " C";
    std::string rh = "RH  : " + std::to_string(hum) + " g/kG";

    display->fill(0);
    display->text("Status", 0, 0);
    display->line(0, 10, 128, 10, 1);
    display->text(Co2, 0, 13);
    display->text(rh, 0, 24);
    display->text(tmp, 0, 35);
    display->text(fs, 0, 46);
    display->text(ap, 0, 57);
    display->show();
}
