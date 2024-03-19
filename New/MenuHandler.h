//
// Created by iamna on 16/03/2024.
//

#ifndef PICO_MODBUS_MENUHANDLER_H
#define PICO_MODBUS_MENUHANDLER_H

#include "pico/stdlib.h"
#include "hardware/i2c.h"
#include "hardware/uart.h"
#include "hardware/irq.h"
#include "hardware/gpio.h"
#include "pico/stdlib.h"
#include "pico/time.h"
#include "hardware/timer.h"
#include "tftdisplay.h"
#include "ModbusController.h"
#include "Menu.h"
#include "Interrupt_Handler.h"
#include "MqttWifiManager.h"

class MenuHandler{
private:
    std::shared_ptr<MqttWifiManager>& mqttWifiManager;
    std::shared_ptr<ModbusController> modbusController;
    std::shared_ptr<TFTDisplay> display;
    absolute_time_t modbus_poll = make_timeout_time_ms(3000);
    int speed = 0;
    int current_speed = 0;
    int max_speed = 1000;
    int pressure = 0;
    int setPressure = 0;
    int currentPressure = 0;
    int counter = 0;
    int menuRot = 0;
    int currentFanSpeed;
    bool errorFlag = false;
    bool mqttFlag = false;
    bool autoMode = false;
    int msg_count = 0;
    Interrupt_Handler &rotary;
    Interrupt_Handler &rotaryButton;
    Interrupt_Handler &button0;
    Interrupt_Handler &button1;
    Interrupt_Handler &button2;


    enum class MenuEventType {
        start,
        status,
        mainMenu,
        setSpeed,
        setPressure,
        controlPressure,
        wifiAndMqtt,
        connectWifiandMqtt,
        CredentialsMenu,
        enterCredentials,
        errorstatus

    };


    // Only for Wifi and Mqtt
    enum key{
        up,
        down
    }key = up;
    enum keyboard{
        capital = 0,
        small = 1,
        number = 2
    } keyboard = capital;
    std::array<char, 26> alphabet = {'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H', 'I', 'J', 'K', 'L', 'M', 'N', 'O', 'P', 'Q', 'R', 'S', 'T', 'U', 'V', 'W', 'X', 'Y', 'Z'};
    std::array<char, 26> lowercase_alphabet = {'a', 'b', 'c', 'd', 'e', 'f', 'g', 'h', 'i', 'j', 'k', 'l', 'm', 'n', 'o', 'p', 'q', 'r', 's', 't', 'u', 'v', 'w', 'x', 'y', 'z'};
    std::array<char, 36> special_chars_numbers = {'0', '1', '2', '3', '4', '5', '6', '7', '8', '9',
                                                  '!', '@', '#', '$', '%', '^', '&', '*', '(', ')',
                                                  '-', '_', '=', '+', '[', ']', '{', '}', ';', ':',
                                                  '\'', '\"', ',', '.', '<', '>'};
    std::vector<char> currentCharBuff;
    std::vector<char> finalssid;
    std::vector<char> password;
    std::vector<char> ip;
    std::vector<char> port;
    std:: string displyString;
    bool isWiFi = false;
    bool isMqtt = false;
    bool ssidEntered = false;
    bool passwordEntered = false;
    bool ipEntered = false;
    bool portEntered = false;
    int maxScroll = 0;
    char ssid2[25]= " ";
    char password2[25] = " ";
    char ip2[25] =" ";
    char port2[25] = "";
    bool credentialsEntered = false;

    int co2 =0;
    int rh = 0;
    int temp = 0;
    int fanSpeed = 0;

    const char* topicPUB = "controller/status";
    absolute_time_t mqttPoll = make_timeout_time_ms(5000);

public:
    MenuHandler(std::shared_ptr<ModbusController> modbusController, std::shared_ptr<TFTDisplay> display1,
                Interrupt_Handler &rotaryA,  Interrupt_Handler &rotaryB,
                Interrupt_Handler &button0, Interrupt_Handler &button1, Interrupt_Handler &button2, std::shared_ptr<MqttWifiManager>& mqttManager);

    void event();
    void setAutoMode(bool autoMode);
    void setSetPressure(int setPressure);
    void setSpeed(int speed);
    void sentMqtt();
    static MenuEventType menuEvent;
    void publishStatus(int msg_count, int fan, int setPressure, int sensorPressure, bool autoMode, bool errorflag, int co2, int rh, int t);
    void readAllSensors();
    std::string getSSID() const;
    std::string getPassword() const;
    std::string getMqttBrokerAddress() const;
    int getMqttBrokerPort() const;
    bool setConection();

    bool connectNow = false;
    bool connected = false;
};

#endif //PICO_MODBUS_MENUHANDLER_H
