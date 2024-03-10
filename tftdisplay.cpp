//
// Created by iamna on 09/03/2024.
//

#include <iostream>
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

void TFTDisplay::selectChar(int selection) {

    int positionY = 0;

    if (selection > 9 && selection <= 19) {
        selection = selection-10;
        positionY += 12;
    }
    else if (selection > 19 ) {
        selection = selection-20;
        positionY += 24;
    }

    int positionX = selection * 12;

    rect(2 + (positionX), positionY, 12, 12, 1);
    show();
    rect(2 + (positionX), positionY, 12, 12, 0);

}

void TFTDisplay::displayKeyboard(int number) {

    int capitalStart = 65;
    int capitalEnd = 90;
    int smallStart = 97;
    int smallEnd = 122;
    int  specialcharStart = 33;
    int specialcharEnd = 64;
    int positionX = 4;
    int positionY = 2;
    int positionY_ref =0;
    int start = 0;
    int end = 0;

    if (number == 0){
        start = capitalStart;
        end = capitalEnd;
    }else if (number == 1 ){
        start = smallStart;
        end = smallEnd;
    } else if (number == 2){
        start = specialcharStart;
        end = specialcharEnd;
    }
    fill(0);
    for (int i = start; i <= end; i++) {
        std::string c(1, static_cast<char>(i));
        text(c.c_str(), positionX, positionY);
        positionX+=12;
        positionY_ref ++;
        std::cout << c.c_str() << std::endl;
        if (positionY_ref == 10) {  // After 12 characters, move to the next row
            positionY += 12;  // Adjust this value as needed to fit your display
            positionX = 4;
            positionY_ref = 0;
        }
    }

    line(4, 62, 16, 62, 1);
    line(22, 62, 34, 62, 1);
    line(40, 62, 52, 62, 1);
    line(58, 62, 70, 62, 1);
    line(76, 62, 88, 62, 1);
    line(94, 62, 106, 62, 1);
    line(112, 62, 124, 62, 1);

    show();
}
    /*

    show();
}*/
/*void TFTDisplay::displayKeyboard() {
    int positionX = 0;
    int positionY = 0;
    std::string c(1, static_cast<char>(0x41));
    text(c.c_str(), 0, 0);
    show();
}*/
