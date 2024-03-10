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
    const char* ssid_;
    const char* password_;
    const char* mqttBrokerAddress_;
    int mqttBrokerPort_;

    static char mqtt_payload[256]; // Assuming a maximum payload size of 256 characters
    static bool automatic;
    static int sami;

public:
    MqttWifiManager(const char* ssid, const char* password, const char* mqttBrokerAddress, int mqttBrokerPort)
            : ssid_(ssid), password_(password), mqttBrokerAddress_(mqttBrokerAddress), mqttBrokerPort_(mqttBrokerPort),
              ipstack_(ssid, password), mqttClient_(ipstack_) {}

    bool connectWiFi() {
        return ipstack_.connect(mqttBrokerAddress_, mqttBrokerPort_)== 1;
    }

    bool connectMQTT() {
        MQTTPacket_connectData data = MQTTPacket_connectData_initializer;
        data.MQTTVersion = 3;
        data.clientID.cstring = (char*)"PicoW-sample";

        int rc = mqttClient_.connect(data);
        return rc == 0;
    }
    static void messageArrivedCallback(MQTT::MessageData& md) {
        MQTT::Message &message = md.message;

        // Copy the payload into a null-terminated string
        char payload_str[message.payloadlen + 1];
        memcpy(payload_str, message.payload, message.payloadlen);
        payload_str[message.payloadlen] = '\0';

        // Print MQTT message details
        printf("Message arrived: qos %d, retained %d, dup %d, packetid %d\n",
               message.qos, message.retained, message.dup, message.id);
        printf("Payload: %s\n", payload_str);

        // check for true or false
        if (strstr(payload_str, "true") != nullptr) {
            automatic = true;
            printf("Automatic: true\n");
        } else if (strstr(payload_str, "false") != nullptr) {
            automatic = false;
            printf("Automatic: false\n");
        } else {
            printf("Payload does not contain 'true' or 'false'\n");
        }

        // Search for "pressure" in the payload
        const char* pressure_key = "\"pressure\":";
        char* pressure_pos = strstr(payload_str, pressure_key);

        if (pressure_pos != nullptr) {
            // Extract the value of "pressure"
            int pressure;
            sscanf(pressure_pos + strlen(pressure_key), "%d", &pressure);
            printf("Pressure: %d\n", pressure);
            sami = pressure;
        } else {
            printf("Payload does not contain 'pressure' field\n");
        }
    }

    bool subscribe(const char* topic, MQTT::QoS qos) {
        return mqttClient_.subscribe(topic, qos, MqttWifiManager::messageArrivedCallback) == 0;
    }

    bool publish(const char* topic, const char* message, MQTT::QoS qos) {
        MQTT::Message msg;
        msg.qos = qos;
        msg.retained = false;
        msg.dup = false;
        msg.payload = (void*)message;
        msg.payloadlen = strlen(message)+1;

        return mqttClient_.publish(topic, msg) == 0;
    }

    void yield(int timeout) {
        mqttClient_.yield(timeout);
    }

    bool isConnected() {
        return mqttClient_.isConnected();
    }
};

#endif //VENTILATION_DEVICE_MQTTWIFIMANAGER_H
