#include <stdio.h>
#include <string.h>
#include <cmath>
#include "pico/stdlib.h"
#include "pico/time.h"
#include "hardware/timer.h"
#include "uart/PicoUart.h"

#include "IPStack.h"
#include "Countdown.h"
#include "MQTTClient.h"
#include "ModbusClient.h"
#include "ModbusRegister.h"
#include "ssd1306.h"
#include "ModbusController.h"
#include "Interrupt_Handler.h"
#include "tftdisplay.h"
#include <memory>
#include "MenuHandler.h"
#include "MqttWifiManager.h"
#include "eeprom.h"
// We are using pins 0 and 1, but see the GPIO function select table in the
// datasheet for information on which other pins can be used.
#define UART_NR 1
#define UART_TX_PIN 4
#define UART_RX_PIN 5


#define BAUD_RATE 9600

#define USE_MODBUS
#define USE_MQTT
#define USE_SSD1306

#define SSID_ADDR 0
#define PASSWORD_ADDR 64
#define MQTT_BROKER_IP_ADDR 128
#define MQTT_BROKER_PORT_ADDR 192

#define I2C1_SDA_PIN 14
#define I2C1_SCL_PIN 15

static const char *topicSUB = "controller/settings";
static const char *topicPUB = "controller/status";

int main(){

    stdio_init_all();
    i2c_init(i2c1, 400*1000);
    gpio_set_function(14, GPIO_FUNC_I2C);
    gpio_set_function(15, GPIO_FUNC_I2C);

    /*i2c_init(i2c0, 100000);
    gpio_set_function(16, GPIO_FUNC_I2C);
    gpio_set_function(17, GPIO_FUNC_I2C);*/
    bool connectNow = false;

    EEPROM eeprom(i2c0,16,17,0x50);
    uint16_t address = 0;
   /* std::string ssid;
    std::string password;
    std::string mqttBrokerAddress;
    int mqttBrokerPort;*/

    std::string ssid2 = "Nadim";
    std::string password2 = "nadimahmed";
    std::string mqttBrokerAddress2 = "172.20.10.12";
    int mqttBrokerPort2 = 1883;

    /*eeprom.write_string(0, ssid2);
    eeprom.write_string(64, password2);
    eeprom.write_string(128 , mqttBrokerAddress2);
    eeprom.write_number(192, mqttBrokerPort2);*/
    eeprom.erase_log(0);
    std::string ssid = eeprom.read_string(0);
    std::string password = eeprom.read_string(64);
    std::string mqttBrokerAddress = eeprom.read_string(128);
    int mqttBrokerPort = eeprom.read_number(192);
    if(!ssid.empty() && !password.empty() && !mqttBrokerAddress.empty() && mqttBrokerPort != 0){
        printf("SSID: %s\n", ssid.c_str());
        printf("Password: %s\n", password.c_str());
        printf("MQTT Broker Address: %s\n", mqttBrokerAddress.c_str());
        printf("MQTT Broker Port: %d\n", mqttBrokerPort);
        connectNow = true;
    }
    else{
        printf("Empty\n");
    }
    /*if(!password.empty()){
        printf("Password: %s\n", password.c_str());
    }
    else{
        printf("Password is empty\n");
    }
    if(!mqttBrokerAddress.empty()){
        printf("MQTT Broker Address: %s\n", mqttBrokerAddress.c_str());
    }
    else{
        printf("MQTT Broker Address is empty\n");
    }
    printf("MQTT Broker Port: %d\n", mqttBrokerPort);
*/


    uint8_t rotA = 10;
    uint8_t rotB = 11;
    uint8_t rotP = 12;
    uint8_t SW0 = 9;
    uint8_t  SW1 = 8;
    uint8_t  SW2 = 7;
    // Handle the rotary encoder and button
    Interrupt_Handler rothandlerA(rotA);
    //Interrupt_Handler rot2(rotB, true);
    Interrupt_Handler rothandlerP(rotP);
    Interrupt_Handler sw0(SW0);
    Interrupt_Handler sw1(SW1);
    Interrupt_Handler sw2(SW2);
    // Handle the display
    i2c_init(i2c1, 400*1000);
    gpio_set_function(14, GPIO_FUNC_I2C);
    gpio_set_function(15, GPIO_FUNC_I2C);
    std::shared_ptr<TFTDisplay> display = std::make_shared<TFTDisplay>(14, 15, i2c1);
    std::shared_ptr<ModbusController> controller = std::make_shared<ModbusController>(UART_NR, UART_TX_PIN, UART_RX_PIN, BAUD_RATE);
    controller->addSensor("fan", 1, 0);
    controller->addSensor("co2", 240, 256);
    controller->addSensor("t", 241, 257);
    controller->addSensor("rh", 241, 256);
    std::shared_ptr<MqttWifiManager> mqttWifiManager;
    MenuHandler menuHandler(controller, display, rothandlerA, rothandlerP, sw0, sw1, sw2, mqttWifiManager );

    // Create the MQTT and Wifi manager

    sleep_ms(1000);
    sleep_ms(50);
    controller->writeSensor("fan", 0);
    sleep_ms(1000);
    //controller->writeSensor("fan", 100);
    auto modbus_poll = make_timeout_time_ms(4000);
    auto modbus_poll2 = make_timeout_time_ms(8000);
    int count = 0;
    //MqttWifiManager mqttWifiManager("TP-Link_A2F", "nadimahmed","192.168.0.210",1883);
    bool autoMode = false;
    int setPressure = 0;
    int speed = 0;


    bool pullInternet = false;
    int rc;
    auto mqttpoll = make_timeout_time_ms(5000);

    while (true){
        if(connectNow){
            display->connecting();
            sleep_ms(1000);
            connectNow = false;
            mqttWifiManager = std::make_shared<MqttWifiManager>(ssid.c_str(), password.c_str(), mqttBrokerAddress.c_str(), mqttBrokerPort);
            rc = mqttWifiManager->connectWiFi();
            if (rc != 1) {
                printf("rc from TCP connect is %d\n", rc);
            }
            if (mqttWifiManager->connectMQTT()) {
                printf("Connected to MQTT broker\n");
                menuHandler.connected = true;
                pullInternet= true;
            } else {
                printf("Failed to connect to MQTT broker\n");
                display->notconnected();
                sleep_ms(1000);
            }
            if (mqttWifiManager->subscribe(topicSUB, MQTT::QOS0)) {
                printf("Subscribed to topic: %s\n", topicSUB);
            } else {
                printf("Failed to subscribe to topic: %s\n", topicSUB);
            }

        }
        if(menuHandler.connectNow){
            menuHandler.connectNow = false;
            connectNow = true;
            ssid = menuHandler.getSSID();
            password = menuHandler.getPassword();
            mqttBrokerAddress = menuHandler.getMqttBrokerAddress();
            mqttBrokerPort = menuHandler.getMqttBrokerPort();
            // write to the eeprom
            eeprom.write_string(0, ssid);
            eeprom.write_string(64, password);
            eeprom.write_string(128 , mqttBrokerAddress);
            eeprom.write_number(192, mqttBrokerPort);
        }
        if(mqttWifiManager->isNotification()){
            mqttWifiManager->setNotification(false);
            std::cout << "Automatic mode" << std::endl;
            autoMode = mqttWifiManager->isAutomatic();
            //menuHandler.setErrorTimer(120000);
            menuHandler.setAutoMode(autoMode);
            if (autoMode) {
                setPressure = mqttWifiManager->getvalue();
                menuHandler.setSetPressure(setPressure);
                menuHandler.seterrorcount();
            } else {
                speed = mqttWifiManager->getvalue();
                menuHandler.setSpeed(speed);
            }
        }
        /*if(time_reached(mqttpoll)){
            mqttpoll = delayed_by_ms(mqttpoll, 5000);
            mqttWifiManager->publish(topicPUB, "status", MQTT::QOS0);

        }*/
        menuHandler.event();
        if(pullInternet){
            cyw43_arch_poll(); // obsolete? - see below
            mqttWifiManager->yield(100);
        }

    }
  return 0;
}
