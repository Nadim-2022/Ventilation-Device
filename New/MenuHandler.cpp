//
// Created by iamna on 16/03/2024.
//

#include <iostream>
#include <utility>
#include "MenuHandler.h"
#include "pico/stdio.h"
#include "pico/stdlib.h"



MenuHandler::MenuHandler(std::shared_ptr<ModbusController> modbusController, std::shared_ptr<TFTDisplay> display1,
                         Interrupt_Handler &rotaryA, Interrupt_Handler &rotaryB,
                         Interrupt_Handler &button0, Interrupt_Handler &button1, Interrupt_Handler &button2,std::shared_ptr<MqttWifiManager>& mqttManager)
        : modbusController(std::move(modbusController)), display(std::move(display1)), rotary(rotaryA),  rotaryButton(rotaryB),
          button0(button0), button1(button1), button2(button2), mqttWifiManager(mqttManager) {

    pressure = 0;
    setPressure = 0;
    currentPressure = 0;
    speed = 0;
    current_speed = 0;
    max_speed = 1000;
    counter = 0;
    menuRot = 0;
    currentFanSpeed = 0;
    maxScroll = 0;

}
void MenuHandler::setAutoMode(bool getMode) {
    this->mqttFlag = true;
   this->autoMode = getMode;
}

void MenuHandler::setSetPressure(int getPressure) {
   this->setPressure = getPressure;
}

void MenuHandler::setSpeed(int getSpeed) {
   this->speed = getSpeed;
}
void MenuHandler::readAllSensors(){
    currentFanSpeed = modbusController->readSensor("fan")/10;
    pressure = modbusController->readPressure();
    co2 = modbusController->readSensor("co2");
    rh = modbusController->readSensor("rh") / 10;
    temp = modbusController->readSensor("t") / 10;
}
std::string MenuHandler::getSSID() const {
    return ssid2;
}
std::string MenuHandler::getPassword() const {
    return password2;
}
std::string MenuHandler::getMqttBrokerAddress() const {
    return ip2;
}
int MenuHandler::getMqttBrokerPort() const {
    return atoi(port2);
}
bool MenuHandler::setConection(){
    return connectNow;
}

void MenuHandler::publishStatus(int msg_count0, int fan, int setPressure0, int sensorPressure0, bool autoMode0, bool errorflag0, int co20, int rh0, int temp0) {
    char buf[256];
    sprintf(buf,R"({"nr": %d, "speed": %d, "setpoint": %d, "pressure": %d, "auto": %s,
                     "error": %s, "co2": %d, "rh": %d, "temp": %d})",
                     msg_count0, fan, setPressure0, sensorPressure0, autoMode0 ? "true" : "false",
                     errorflag0 ? "true" : "false", co20, rh0, temp0);
    //printf("Publishing from MenuHandler, mqttManager: %p, topicPUB: %s\n", mqttWifiManager.get(), topicPUB);
    mqttWifiManager->publish(topicPUB, buf, MQTT::QOS0);
    printf("Publishing: %s\n", buf);
}


MenuHandler::MenuEventType MenuHandler::menuEvent;

void MenuHandler::event() {
    if(mqttFlag){
        mqttFlag = false;
        if(autoMode){
            menuEvent = MenuHandler::MenuEventType::controlPressure;
        }else{
            autoMode = false;
            menuEvent = MenuHandler::MenuEventType::status;
            modbusController->writeSensor("fan", speed * 10);
        }
    }
    // Only send MQTT message if the message
    if(connected) {
        if (time_reached(mqttPoll)) {
            printf("Sending MQTT message\n");
            mqttPoll = delayed_by_ms(mqttPoll, 5000);
            publishStatus(++msg_count, currentFanSpeed, autoMode ? setPressure : speed, pressure, autoMode, errorFlag,
                          co2, rh, temp);
        }
    }
    switch (menuEvent) {
        case MenuHandler::MenuEventType::start: // Start the system and display the welcome screen
            display->welcomeScreen();
            sleep_ms(2000);
            display->mainMenu();
            display->selectmenu(0);
            menuEvent = MenuHandler::MenuEventType::mainMenu;
            rotaryButton.buttonPressed = false;
            break;
        case MenuHandler::MenuEventType::status: // Display the status of the system and the sensors and the fan speed
            if(rotaryButton.buttonPressed){
                rotaryButton.buttonPressed = false;
                display->mainMenu();
                display->selectmenu(0);
                menuEvent = MenuHandler::MenuEventType::mainMenu;
            }else {

                currentFanSpeed = modbusController->readSensor("fan")/10;
                pressure = modbusController->readPressure();
                co2 = modbusController->readSensor("co2");
                rh = modbusController->readSensor("rh") / 10;
                temp = modbusController->readSensor("t") / 10;

                display->displayStatus(co2,
                                       temp,
                                       rh,
                                       currentFanSpeed,
                                       pressure, display->wifi_signal);
            }
            break;
        case MenuHandler::MenuEventType::mainMenu: // Main menu to select the different options
            if (rotary.rotaryturned) {
                //std::cout << "Rotary turned in  mebu" << std::endl;
                rotary.rotaryturned  = false;
                menuRot += rotary.getCount();
                if (menuRot > 3) menuRot = 0;
                if (menuRot < 0) menuRot = 3;
                display->selectmenu(menuRot);
            }
            if(rotaryButton.buttonPressed){
                rotaryButton.buttonPressed = false;
                switch (menuRot) {
                    case 0:
                        menuEvent = MenuHandler::MenuEventType::setSpeed;
                        display->displayControlSpeed(speed);
                        break;
                    case 1:
                        menuEvent = MenuHandler::MenuEventType::setPressure;
                        display->displayControlPressureLevel(pressure);
                        break;
                    case 2:
                        menuEvent = MenuHandler::MenuEventType::wifiAndMqtt;
                        rotaryButton.buttonPressed = false;
                        display->wifiandmqttmenu();
                        display->selectwifiandmqttmenu(0);
                        menuRot = 3;
                        break;
                    case 3:
                        menuEvent = MenuHandler::MenuEventType::status;
                        break;
                }
            }
            /*if(rotaryButton.buttonPressed && menuRot == 3){
                rotaryButton.buttonPressed = false;
                menuEvent = MenuHandler::MenuEventType::status;
            }*/
            break;
        case MenuEventType::setSpeed:
            if (rotary.rotaryturned) {
                rotary.rotaryturned = false;
                speed += rotary.getCount();
                if (speed >= 100) speed = 100;
                if (speed <= 0) speed = 0;
                display->displayControlSpeed(speed);
                modbusController->writeSensor("fan", speed * 10);
            }
            if (rotaryButton.buttonPressed) {
                rotaryButton.buttonPressed = false;
                display->mainMenu();
                display->selectmenu(0);
                menuEvent = MenuHandler::MenuEventType::mainMenu;
            }
            break;

        case MenuEventType::setPressure:
            if (rotary.rotaryturned) {
                rotary.rotaryturned = false;
                pressure += rotary.getCount();
                if (pressure >= 120) pressure = 120;
                if (pressure <= 0) pressure = 0;
                display->displayControlPressureLevel(pressure);
                //modbusController->writePressure(pressure);
            }
            if (rotaryButton.buttonPressed) {
                rotaryButton.buttonPressed = false;
                setPressure = pressure;
                /*display->mainMenu();
                display->selectmenu(0);*/
                currentPressure = 0;
                menuEvent = MenuHandler::MenuEventType::controlPressure;
                speed = (speed < 9) ? 9 : speed;
                modbusController->writeSensor("fan",speed);
                currentFanSpeed = modbusController->readSensor("fan")/10;
                display->displayStatus(modbusController->readSensor("co2"),
                                       modbusController->readSensor("t") / 10,
                                       modbusController->readSensor("rh") / 10,
                                       currentFanSpeed,
                                       modbusController->readPressure(), display->wifi_signal);
            }
            break;

        case MenuEventType::controlPressure:
            currentPressure = modbusController->readPressure();
            if(currentPressure < setPressure-2){
                speed ++;
                if (speed >= 100) speed = 100;
                modbusController->writeSensor("fan", speed * 10);
                std::cout << " speed: " << speed << std::endl;
                std::cout << "currrent air Pressure: " << modbusController->readPressure() << " "<<"setPressure"<< setPressure<< std::endl;
                std::cout << "currrent fan ssssspeed: " << modbusController->readSensor("fan") << std::endl;
                sleep_ms(50);
            }else if(currentPressure > setPressure+2){
                speed --;
                if (speed <= 0) speed = 0;
                modbusController->writeSensor("fan", speed * 10);
                sleep_ms(50);
            }
            pressure = currentPressure;
            if(time_reached(modbus_poll)){
                currentFanSpeed = modbusController->readSensor("fan")/10;
                //currentPressure = modbusController->readPressure();
                modbus_poll = delayed_by_ms(modbus_poll,3000);
                display->displayStatus(modbusController->readSensor("co2"),
                                       modbusController->readSensor("t") / 10,
                                       modbusController->readSensor("rh") / 10,
                                       currentFanSpeed,
                                       currentPressure, display->wifi_signal);
            }
            if(rotaryButton.buttonPressed){
                rotaryButton.buttonPressed = false;
                display->mainMenu();
                display->selectmenu(0);
                menuEvent = MenuHandler::MenuEventType::mainMenu;
            }
            break;
        case MenuEventType::wifiAndMqtt: // Display the wifi and mqtt menu and return to the main menu
            if (rotary.rotaryturned) {
                rotary.rotaryturned  = false;
                menuRot += rotary.getCount();
                if (menuRot > 2) menuRot = 0;
                if (menuRot < 0) menuRot = 2;
                display->selectwifiandmqttmenu(menuRot);
            }
            if(rotaryButton.buttonPressed){
                printf("Button pressed in wifiMenu\n");
                rotaryButton.buttonPressed = false;
                switch (menuRot) {
                    case 0:
                        menuEvent = MenuHandler::MenuEventType::connectWifiandMqtt;

                        break;
                    case 1:
                        menuEvent = MenuEventType::CredentialsMenu;
                        menuRot = 0;
                        display->credenttialsMenu();
                        display->selctcredenttials(0);
                        break;
                    case 2:
                        menuEvent = MenuEventType::mainMenu;
                        display->mainMenu();
                        display->selectmenu(0);
                        break;
                }
            }
            break;
        case MenuHandler::MenuEventType::connectWifiandMqtt: // Connect to wifi and mqtt nothing else and return to main menu
            credentialsEntered = true;
            connectNow = true;
            menuEvent = MenuEventType::mainMenu;
            display->mainMenu();
            display->selectmenu(0);
            break;

        case MenuEventType::CredentialsMenu:
            if (rotary.rotaryturned) {
                rotary.rotaryturned  = false;
                menuRot += rotary.getCount();
                if (menuRot > 2) menuRot = 0;
                if (menuRot < 0) menuRot = 2;
                display->selctcredenttials(menuRot);
            }
            if(rotaryButton.buttonPressed){
                rotaryButton.buttonPressed = false;
                switch (menuRot) {
                    case 0:
                        menuEvent = MenuEventType::enterCredentials;
                        isWiFi = true;
                        display->fill(0);
                        key = up;
                        break;
                    case 1:
                        menuEvent = MenuEventType::enterCredentials;
                        isMqtt = true;
                        display->fill(0);
                        key = up;
                        break;
                    case 2:
                        display->wifiandmqttmenu();
                        display->selectwifiandmqttmenu(0);
                        menuEvent = MenuEventType::wifiAndMqtt;
                        break;
                }
            }
            break;

        case MenuEventType::enterCredentials:
            switch (key) {
                case up:
                    display->fill(0);
                    if(isWiFi){
                        if(!ssidEntered){
                            display->text("Write SSID", 0, 20);
                            display->show();
                            sleep_ms(4000);
                        }
                        else if(!passwordEntered){
                            display->text("Write Password", 0, 20);
                            display->show();
                            sleep_ms(4000);
                        }
                    }else if(isMqtt){
                        if(!ipEntered){
                            display->text("Write IP", 0, 20);
                            display->show();
                            sleep_ms(4000);
                        }
                        else if(!portEntered){
                            display->text("Write Port", 0, 20);
                            display->show();
                            sleep_ms(4000);
                        }
                    }
                    keyboard= capital;
                    display->displayKeyboard(keyboard);
                    display->selectChar(0);
                    maxScroll = alphabet.size()-1;         //alphabet.size() - 1;
                    menuRot = 0;
                    key = down;
                    break;
                case down:
                    // Change the keyboard to the next one Capital -> Small -> Number
                    if (button2.buttonPressed) {
                        button2.buttonPressed = false;
                        if (keyboard == capital) {
                            maxScroll = lowercase_alphabet.size() - 1;                //lowercase_alphabet.size() - 1;
                            keyboard = small;
                        } else if (keyboard == small) {
                            maxScroll = special_chars_numbers.size() - 1;               //special_chars_numbers.size() - 1;
                            keyboard = number;
                        } else {
                            keyboard = capital;
                            maxScroll = alphabet.size() - 1;               //alphabet.size() - 1;
                        }
                        display->displayKeyboard(keyboard);
                    }
                    // Select the character from the keyboard
                    if (rotary.rotaryturned) {
                        rotary.rotaryturned = false;
                        menuRot += rotary.getCount();
                        if (menuRot > maxScroll) {
                            menuRot = 0;
                        } else if (menuRot < 0) {
                            menuRot = maxScroll;
                        }
                        display->selectChar(menuRot);
                        printf("menuRot: %d\n", menuRot);
                    }
                    // Limit The menuRot to the size of the array
                   /* if (keyboard == capital && menuRot >= alphabet.size()) {
                        menuRot = alphabet.size() - 1;
                    } else if (keyboard == small && menuRot >= lowercase_alphabet.size()) {
                        menuRot = lowercase_alphabet.size() - 1;
                    } else if (keyboard == number && menuRot >= special_chars_numbers.size()) {
                        menuRot = special_chars_numbers.size() - 1;
                    }*/
                    // Add the selected character to the buffer
                    if (rotaryButton.buttonPressed) {
                        rotaryButton.buttonPressed = false;
                        if (keyboard == capital) {
                            currentCharBuff.push_back(alphabet[menuRot]);
                        } else if (keyboard == small) {
                            currentCharBuff.push_back(lowercase_alphabet[menuRot]);
                        } else {
                            currentCharBuff.push_back(special_chars_numbers[menuRot]);
                        }
                        displyString = std::string(currentCharBuff.begin(), currentCharBuff.end());
                        display->displayString(displyString);
                        display->selectChar(menuRot);
                        std::cout << displyString << std::endl;

                    }
                    // Erase the last character
                    if (button1.buttonPressed) {
                        button1.buttonPressed = false;
                        if (!currentCharBuff.empty()) {
                            currentCharBuff.pop_back();
                            displyString = std::string(currentCharBuff.begin(), currentCharBuff.end());
                            display->displayString(displyString);
                        }
                    }
                    if (button0.buttonPressed) { // Save the entered string
                        button0.buttonPressed = false;
                        if (!displyString.empty()) { // If the Entered string is not empty
                            displyString = std::string(currentCharBuff.begin(), currentCharBuff.end());
                            //std::cout << displyString << std::endl;
                            if(!ssidEntered){ // If the ssid is not entered save it to ssid2
                                std::copy(currentCharBuff.begin(), currentCharBuff.end(), ssid2);
                                ssid2[displyString.size()] = '\0';
                                currentCharBuff.clear();
                                ssidEntered = true;
                                key = up;
                            }
                            else if(!passwordEntered){ // If the password is not entered save it to password2
                                std::copy(currentCharBuff.begin(), currentCharBuff.end(), password2);
                                password2[displyString.size()] = '\0';
                                currentCharBuff.clear();
                                passwordEntered = true;
                                isWiFi = false;
                                menuEvent = MenuEventType::CredentialsMenu;
                                display->credenttialsMenu();
                                display->selctcredenttials(0);
                            }
                            else if(!ipEntered){ // If the ip is not entered save it to ip2
                                std::copy(currentCharBuff.begin(), currentCharBuff.end(), ip2);
                                ip2[displyString.size()] = '\0';
                                currentCharBuff.clear();
                                ipEntered = true;
                                key = up;
                            }
                            else if(!portEntered){ // If the port is not entered save it to port2
                                std::copy(currentCharBuff.begin(), currentCharBuff.end(), port2);
                                port2[displyString.size()] = '\0';
                                currentCharBuff.clear();
                                portEntered = true;
                                isMqtt = false;
                                menuEvent = MenuEventType::CredentialsMenu;
                                display->credenttialsMenu();
                                display->selctcredenttials(0);
                            }
                            printf("SSID: %s\n", ssid2);
                            printf("Password: %s\n", password2);
                            printf("IP: %s\n", ip2);
                            printf("Port: %s\n", port2);

                        }
                    }


                    break;

            }
            break;

        case MenuEventType::errorstatus:
            display->errorScreen();
            sleep_ms(4000);
            break;


    }

}
