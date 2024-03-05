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
    mono_vlsb rb(raspberry26x32, 26, 32);
    display->blit(rb, 20, 20);
    display->rect(15, 15, 35, 45, 1);
    display->line(60, 5, 120, 60, 1);
    display->line(60, 60, 120, 5, 1);
    display->show();

}

void I2C_Display::displayMenu(const char *textA, const char *textB) {
    display->fill(0);
    display->text(textA, 0, 30);
    display->text(textB, 0, 40);
    display->show();
}
void I2C_Display::displayControlPressureLevel(uint16_t pressure) {
    width = (pressure * 128) /120;
    display->fill(0);
    display->text("Air Pressure: ", 0, 0);
    display->text(std::to_string(pressure).c_str(), 0, 10);
    display->rect(0, 40,width , 20, 1, true);
    display->show();
}

void I2C_Display::displayControlSpeed(uint16_t speed) {
    width = (speed * 128) / 100;
    display->fill(0);
    display->text("Speed: ", 0, 0);
    display->text(std::to_string(speed).c_str(), 0, 10);
    display->rect(0, 40,width , 20, 1, true);
    display->show();
}
