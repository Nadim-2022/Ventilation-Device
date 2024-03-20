//
// Created by iamna on 09/03/2024.
//

#ifndef VENTILATION_DEVICE_MQTTWIFIMANAGER_H
#define VENTILATION_DEVICE_MQTTWIFIMANAGER_H
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

class MqttWifiManager {
private:
    IPStack ipstack_;
    MQTT::Client<IPStack, Countdown> mqttClient_;
    const char* ssid_{};
    const char* password_{};
    const char* mqttBrokerAddress_{};
    int mqttBrokerPort_{};

    static char mqtt_payload[256]; // Assuming a maximum payload size of 256 characters
    static bool automatic;
    static bool notification;
    static int MqttValue;
public:

    MqttWifiManager(const char* ssid, const char* password, const char* mqttBrokerAddress, int mqttBrokerPort)
            : ssid_(ssid), password_(password), mqttBrokerAddress_(mqttBrokerAddress), mqttBrokerPort_(mqttBrokerPort),
              ipstack_(ssid, password), mqttClient_(ipstack_) {

    }

    bool connectWiFi();
    int getvalue();
    bool isAutomatic();
    bool connectMQTT();

    static void messageArrivedCallback(MQTT::MessageData& md);

    bool subscribe(const char* topic, MQTT::QoS qos);

    bool publish(const char* topic, const char* message, MQTT::QoS qos);
    void yield(int timeout);

    bool isConnected();
    bool isNotification();
    void setNotification(bool value);
};

#endif //VENTILATION_DEVICE_MQTTWIFIMANAGER_H
