//
// Created by iamna on 09/03/2024.
//

#include "MqttWifiManager.h"
char MqttWifiManager::mqtt_payload[256] = {0};
bool MqttWifiManager::automatic = false;
bool MqttWifiManager::notification = false;
int MqttWifiManager::MqttValue = 0;

bool MqttWifiManager::connectWiFi() {
    return ipstack_.connect(mqttBrokerAddress_, mqttBrokerPort_)== 1;
}
int MqttWifiManager::getvalue(){
    return MqttValue;
}
bool MqttWifiManager::isAutomatic(){
    return automatic;
}
bool MqttWifiManager::connectMQTT() {
    MQTTPacket_connectData data = MQTTPacket_connectData_initializer;
    data.MQTTVersion = 3;
    data.clientID.cstring = (char*)"PicoW-sample";

    int rc = mqttClient_.connect(data);
    return rc == 0;
}

void MqttWifiManager::messageArrivedCallback(MQTT::MessageData& md) {
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
    const char* speed_key = "\"speed\":";
    char* pressure_pos = strstr(payload_str, pressure_key);
    char* speed_pos = strstr(payload_str, speed_key);

    if (pressure_pos != nullptr) {
        int pressure;
        sscanf(pressure_pos + strlen(pressure_key), "%d", &pressure);
        printf("Pressure: %d\n", pressure);
        MqttValue = pressure;
    } else if (speed_pos != nullptr) {
        int speed;
        sscanf(speed_pos + strlen(speed_key), "%d", &speed);
        printf("Speed: %d\n", speed);
        MqttValue = speed;
    }else {
        printf("Payload does not contain 'pressure' field\n");
    }

    notification = true;
}

bool MqttWifiManager::subscribe(const char* topic, MQTT::QoS qos) {
    return mqttClient_.subscribe(topic, qos, MqttWifiManager::messageArrivedCallback) == 0;
}

bool MqttWifiManager::publish(const char* topic, const char* message, MQTT::QoS qos) {
    printf("Publishing message: %s, topic: %s\n", message, topic);
    MQTT::Message msg;
    msg.qos = qos;
    msg.retained = false;
    msg.dup = false;
    msg.payload = (void*)message;
    msg.payloadlen = strlen(message);
    int rc = mqttClient_.publish(topic, msg);
    if (rc != 0) {
        printf("Failed to publish message, return code: %d\n", rc);
    }
    return rc == 0;
}

void MqttWifiManager::yield(int timeout) {
    mqttClient_.yield(timeout);
}

bool MqttWifiManager::isConnected() {
    return mqttClient_.isConnected();
}
bool MqttWifiManager::isNotification() {
    return notification;
}
void MqttWifiManager::setNotification(bool value) {
    notification = value;
}