#include <iostream>
#include <string>
#include <map>
#include <vector>


/*
 * creaate a class that can parse a JSON string in to a map
 * for example the JSON string:
 * {
"auto":true,
"pressure": 5
}
    * would be parsed in to a map with the key value pairs: true, auto and 5, pressure
 *
 */
class JSONParser {
public:
JSONParser(std::string json);
std::map<std::string, std::string> getMap();
std::string getValue(std::string key);
std::string getJSON();
void setJSON(std::string json);
void setMap(std::map<std::string, std::string> map);
void setValue(std::string key, std::string value);
void printMap();
void printJSON();
void printValue(std::string key);
void removeValue(std::string key);
void removeMap();
void removeJSON();
bool getBooleanValue(const std::string& key);

private:
std::string json;
};