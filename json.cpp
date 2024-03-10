#include "json.h"

JSONParser::JSONParser(std::string json) {
    this->json = json;
}

std::map<std::string, std::string> JSONParser::getMap() {
    std::map<std::string, std::string> json_map;
    std::string key;
    std::string value;
    std::string temp;
    bool key_found = false;
    bool value_found = false;
    for (int i = 0; i < json.length(); i++) {
        if (json[i] == '"') {
            if (key_found) {
                key = temp;
                temp = "";
                key_found = false;
            } else {
                key_found = true;
            }
        } else if (json[i] == ':') {
            value_found = true;
        } else if (json[i] == ',' || json[i] == '}') {
            value = temp;
            temp = "";
            json_map[key] = value;
            value_found = false;
        } else if (json[i] != ' ' && json[i] != '{') {
            temp += json[i];
        }
    }
    return json_map;
}

std::string JSONParser::getValue(std::string key) {
    std::map<std::string, std::string> json_map = getMap();
    return json_map[key];
}

std::string JSONParser::getJSON() {
    return json;
}

void JSONParser::setJSON(std::string json) {
    this->json = json;
}

void JSONParser::setMap(std::map<std::string, std::string> map) {
    std::string new_json = "{";
    for (const auto &it : map) {
        new_json += "\"" + it.first + "\":" + it.second + ",";
    }
    new_json.pop_back();
    new_json += "}";
    json = new_json;
}

void JSONParser::setValue(std::string key, std::string value) {
    std::map<std::string, std::string> json_map = getMap();
    json_map[key] = value;
    setMap(json_map);
}

void JSONParser::printMap() {
    std::map<std::string, std::string> json_map = getMap();
    for (const auto &it : json_map) {
        std::cout << it.first << " : " << it.second << std::endl;
    }
}

void JSONParser::printJSON() {
    std::cout << json << std::endl;
}

void JSONParser::printValue(std::string key) {
    std::map<std::string, std::string> json_map = getMap();
    std::cout << json_map[key] << std::endl;
}

void JSONParser::removeValue(std::string key) {
    std::map<std::string, std::string> json_map = getMap();
    json_map.erase(key);
    setMap(json_map);
}

void JSONParser::removeMap() {
    std::map<std::string, std::string> json_map = getMap();
    json_map.clear();
    setMap(json_map);
}

void JSONParser::removeJSON() {
    json = "";
}

bool JSONParser::getBooleanValue(const std::string& key) {
    std::string value = this->getValue(key);
    return (value == "true") ? true : false;
}

