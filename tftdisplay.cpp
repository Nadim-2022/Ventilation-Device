//
// Created by iamna on 09/03/2024.
//

#include "tftdisplay.h"
#include "font_petme128_8x8.h"

TFTDisplay::TFTDisplay(uint16_t sda, uint16_t scl, i2c_inst* i2c): ssd1306(i2c) {
    width = 0;
}
void TFTDisplay::displayText(const char *string, const uint8_t *font) {
    fill(0);
    text(string, 0, 0);
    mono_vlsb rb(font, 13, 11);
    blit(rb, 114, 0);
    show();
}

void TFTDisplay::displayMenu(const char *textA, const char *textB) {
    fill(0);
    text(textA, 0, 30);
    text(textB, 0, 40);
    show();
}

void TFTDisplay::displayControlPressureLevel(uint16_t pressure) {
    std::string APmessage = std::to_string(pressure) + " Pa";
    width = (pressure * 128) /120;
    fill(0);
    text("Air Pressure: ", 0, 0);
    text(APmessage, 45, 20);
    rect(0, 40,width , 20, 1, true);
    show();
}

void TFTDisplay::displayControlSpeed(uint16_t speed) {
    std::string FSmessage = std::to_string(speed) + " %";
    width = (speed * 128) / 100;
    fill(0);
    text("Fan Speed: ", 0, 0);
    text(FSmessage, 45, 20);
    rect(0, 40,width , 20, 1, true);
    show();
}

void TFTDisplay::welcomeScreen() {
    fill(0);
    text("Welcome to Group", 0, 22);
    text("3's Ventilation", 0, 32);
    text("Project", 38, 42);
    show();
}

void TFTDisplay::autoOrManual() {
    fill(0);
    text("Auto", 0, 22);
    text("Manual", 0, 42);
    show();
}

void TFTDisplay::loginscreen() {
    fill(0);
    text("Existing User", 0, 22);
    text("New User", 0, 32);
    show();
}
void TFTDisplay::connectTowifi() {
    fill(0);
    text("Connect", 0, 22);
    text("New Credentials", 0, 42);
    show();
}

void TFTDisplay::selectmenu(int selection) {
    if(selection == 1){
        rect(0, 18, 128, 16, 1);
        rect(0, 38, 128, 16, 0);
    } else{
        rect(0, 18, 128, 16, 0);
        rect(0, 38, 128, 16, 1);
    }
    show();
}
void TFTDisplay::displayStatus(const int co2, const int temp, const int hum, const int fan, const int AP, const uint8_t *wifi_icon) {
    mono_vlsb wifi_sig(wifi_icon, 10, 7);
    std::string fs = "FS  : " + std::to_string(fan) + " %";
    std::string ap = "AP  : " + std::to_string(AP) + " Pa";
    std::string Co2 = "Co2 : " + std::to_string(co2) + " ppm";
    std::string tmp = "Temp: " + std::to_string(temp) + " C";
    std::string rh = "RH  : " + std::to_string(hum) + " g/kG";

    fill(0);
    text("Status", 0, 0);
    line(0, 10, 128, 10, 1);
    text(Co2, 0, 13);
    text(rh, 0, 24);
    text(tmp, 0, 35);
    text(fs, 0, 46);
    text(ap, 0, 57);
    blit(wifi_sig, 114, 0);
    show();
}