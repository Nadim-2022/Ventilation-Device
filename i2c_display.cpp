//
// Created by iamna on 01/03/2024.
//

#include "i2c_display.h"
#include "font_petme128_8x8.h"

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
    mono_vlsb rb(raspberry26x32, 13, 11);
    display->blit(rb, 114, 0);
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

void I2C_Display::loginscreen() {
    display->fill(0);
    display->text("New User", 30,22);
    display->text("Existing User", 15,42);
    display->show();
}

//Chatgpt's response
//This for the new user screen, but I named it keyboard since we might need it somewhere else so I made the name general

/*void keyboard() {
    // Define the keyboard layout using a 2D array
    char keyboard[4][13] = {
            {'1', '2', '3', '4', '5', '6', '7', '8', '9', '0', '-', '='},
            {'q', 'w', 'e', 'r', 't', 'y', 'u', 'i', 'o', 'p', '[', ']'},
            {'a', 's', 'd', 'f', 'g', 'h', 'j', 'k', 'l', ';', '\'', '\\'},
            {'z', 'x', 'c', 'v', 'b', 'n', 'm', ',', '.', '/'}
    };

    // Clear the display buffer.
    display->clear();

    // Set text size and color.
    display.setTextSize(1);
    display.setTextColor(SSD1306_WHITE);

    // Display the keyboard
    for (int i = 0; i < 4; ++i) {
        for (int j = 0; j < 13; ++j) {
            // Print each character at a specific position
            display.setCursor(j * 6, i * 10); // Each character occupies approximately 6x10 pixels
            display.print(keyboard[i][j]);
        }
    }

    // Display the buffer
    display.display();
}


void I2C_Display::existinguser(str username1, str username2) { //I don't know how many users are there going to be so I am just leaving this like this
    display->fill(0);
    display->text("1. " + username1, 0, 0);
    display->text("2. " + username2, 0, 32);
    display->show();
}
*/

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


void I2C_Display::displayStatus(const int co2, const int temp, const int hum, const int fan, const int AP, const uint8_t *wifi_icon) {
    mono_vlsb wifi_sig(wifi_icon, 10, 7);
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
    display->blit(wifi_sig, 114, 0);
    display->show();
}

void I2C_Display::test(uint16_t letter) {
    display->fill(0);
    display->text("1. User1", 0, 0);
    display->text("2. User2", 0, 32);
    display->show();
}

