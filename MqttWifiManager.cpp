//
// Created by iamna on 09/03/2024.
//

#include "MqttWifiManager.h"
char MqttWifiManager::mqtt_payload[256] = {0};
bool MqttWifiManager::automatic = false;
bool MqttWifiManager::notification = false;
int MqttWifiManager::MqttValue = 0;