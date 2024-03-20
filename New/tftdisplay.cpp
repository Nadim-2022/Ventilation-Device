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

void TFTDisplay::mainMenu() {
    fill(0);
    text("Main Menu", 30, 2);
    text("Set Speed", 0, 20);
    text("Set Pressure", 0, 30);
    text( "MQTT & WiFi", 0, 42);
    text("Status", 0, 54);
    show();
}

void TFTDisplay::wifiandmqttmenu() {
    fill(0);
    text("Connect", 0, 15);
    text("New Credentials", 0, 30);
    text("Return", 0,45 );
    show();
}
// Now we have to implement the selectmenu function for 4 menu items
// The function should highlight the selected menu item

void TFTDisplay::selectwifiandmqttmenu(int selection) {
    if(selection == 0){
        rect(0, 11, 128, 15, 1);
        rect(0, 26, 128, 15, 0);
        rect(0, 41, 128, 15, 0);
    } else if(selection == 1){
        rect(0, 11, 128, 15, 0);
        rect(0, 26, 128, 15, 1);
        rect(0, 41, 128, 15, 0);
    } else if(selection == 2){
        rect(0, 11, 128, 15, 0);
        rect(0, 26, 128, 15, 0);
        rect(0, 41, 128, 15, 1);
    }
    show();
}
void TFTDisplay::selectmenu(int selection) {
    if(selection == 0){
        rect(0, 18, 128, 12, 1);
        rect(0, 28, 128, 12, 0);
        rect(0, 40, 128, 12, 0);
        rect(0, 52, 128, 12, 0);
    } else if(selection == 1){
        rect(0, 18, 128, 12, 0);
        rect(0, 28, 128, 12, 1);
        rect(0, 40, 128, 12, 0);
        rect(0, 52, 128, 12, 0);
    } else if(selection == 2){
        rect(0, 18, 128, 12, 0);
        rect(0, 28, 128, 12, 0);
        rect(0, 40, 128, 12, 1);
        rect(0, 52, 128, 12, 0);
    } else if(selection == 3){
        rect(0, 18, 128, 12, 0);
        rect(0, 28, 128, 12, 0);
        rect(0, 40, 128, 12, 0);
        rect(0, 52, 128, 12, 1);
    }
    show();
}
void TFTDisplay::displayMQTTandWifiMenu(int number) {
    fill(0);
    if (number == 0) {
        text("MQTT", 0, 0);
        text("WiFi", 0, 12);
    } else {
        text("MQTT", 0, 12);
        text("WiFi", 0, 0);
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

/*void TFTDisplay::displayKeyboard(int number) {

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
    *//*fill(0);*//*
    for (int i = start; i <= end; i++) {
        std::string c(1, static_cast<char>(i));
        text(c.c_str(), positionX, positionY);
        positionX+=12;
        positionY_ref ++;
        //std::cout << c.c_str() << std::endl;
        if (positionY_ref == 10) {  // After 12 characters, move to the next row
            positionY += 12;  // Adjust this value as needed to fit your display
            positionX = 4;
            positionY_ref = 0;
        }
    }

    *//*line(4, 52, 16, 52, 1);
    line(22, 52, 34, 52, 1);
    line(40, 52, 52, 52, 1);
    line(58, 52, 70, 52, 1);
    line(76, 52, 88, 52, 1);
    line(94, 52, 106, 52, 1);
    line(112, 52, 124, 52, 1);*//*

    line(4, 62, 16, 62, 1);
    line(22, 62, 34, 62, 1);
    line(40, 62, 52, 62, 1);
    line(58, 62, 70, 62, 1);
    line(76, 62, 88, 62, 1);
    line(94, 62, 106, 62, 1);
    line(112, 62, 124, 62, 1);

    show();
}*/
void TFTDisplay::displaychar(char c, int positionX, int positionY) {
    std::string s(1, c);
    rect( positionX, positionY, 10, 10, 0, true);
    text(s.c_str(), positionX, positionY);
    /*if(positionX >124){
        scroll(positionX-18, 0);
    }
*/
    show();
}
/*void TFTDisplay::displayString(std::string string) {
    int j = 4;
    if ( string.length() > 7){
        rect(0,52,128,10,0,true);
        for(int i = 0; i < string.length(); i++){
            displaychar(string[i], j, 52);
            j+=18;
        }
    } else{
        rect(0,52,128,10,0,true);
        for(int i = 0; i < string.length(); i++){
            displaychar(string[i], j, 52);
            j+=18;
        }
    }
    show();

}*/
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
void TFTDisplay::displayStatus(const int co2, const int temp, const int hum, const int fan, const int AP, const uint8_t *wifi_icon) {
    mono_vlsb wifi_sig(wifi_icon, 10, 7);
    std::string fs = "FS  : " + std::to_string(fan) + " %";
    std::string ap = "AP  : " + std::to_string(AP) + " Pa";
    std::string Co2 = "Co2 : " + std::to_string(co2) + " ppm";
    std::string tmp = "Temp: " + std::to_string(temp) + " C";
    std::string rh = "RH  : " + std::to_string(hum) + " %";

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
void TFTDisplay::displayString(const std::string& string) {
    rect(3, 52, 128, 10, 0, true);
    text(string.c_str(), 4, 52);
    show();
}

void TFTDisplay::displayString(const char* string) {
    rect(3, 52, 128, 10, 0, true);
    text(string, 4, 52);
    show();
}

void TFTDisplay::displayKeyboard(int number) {
    int positionX = 4;
    int positionY = 2;
    int positionY_ref = 0;
    int start = 0;
    int end = 0;

    // Select the appropriate array based on the number parameter
    const char* charArray = nullptr;
    int arraySize = 0;

    switch (number) {
        case 0:
            charArray = alphabet.data();
            arraySize = alphabet.size();
            break;
        case 1:
            charArray = lowercase_alphabet.data();
            arraySize = lowercase_alphabet.size();
            break;
        case 2:
            charArray = special_chars_numbers.data();
            arraySize = special_chars_numbers.size();
            break;
        default:
            // Handle invalid number parameter
            // might need to display an error message or do nothing
            return;
    }
    rect(0, 0, 128, 50, 0, true);
    // Iterate through the selected array and display characters
    for (int i = 0; i < arraySize; ++i) {
        std::string c(1, charArray[i]);
        text(c.c_str(), positionX, positionY);
        positionX += 12;
        positionY_ref++;
        if (positionY_ref == 10) {
            positionY += 12;
            positionX = 4;
            positionY_ref = 0;
        }
    }

    // Draw lines
    int lineX = 4;
    for (int i = 0; i < 7; i++) {
        line(lineX, 64, lineX + 12, 64, 1);
        lineX += 18;
    }

    show();
}
void TFTDisplay::credenttialsMenu() {
    fill(0);
    text("Wifi CredentialsMenu", 0, 15);
    text("MQTT CredentialsMenu", 0, 30);
    text("Return", 0,45 );
    show();
}
void TFTDisplay::selctcredenttials(int selection) {
    if(selection == 0){
        rect(0, 11, 128, 15, 1);
        rect(0, 26, 128, 15, 0);
        rect(0, 41, 128, 15, 0);
    } else if(selection == 1){
        rect(0, 11, 128, 15, 0);
        rect(0, 26, 128, 15, 1);
        rect(0, 41, 128, 15, 0);
    } else if(selection == 2){
        rect(0, 11, 128, 15, 0);
        rect(0, 26, 128, 15, 0);
        rect(0, 41, 128, 15, 1);
    }
    show();
}
void TFTDisplay::errorScreen() {
    fill(0);
    text("Error", 44, 5);
    text("Pressure Low", 0, 22);
    text("Please restart", 0, 32);
    text("The device", 0, 42);
    show();
}
