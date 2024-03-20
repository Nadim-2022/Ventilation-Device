//
// Created by iamna on 19/03/2024.
//

#include "eeprom.h"
#include <cstring>
#include <cstdio>
#include <string>

uint16_t EEPROM::crc16(const uint8_t *data_p, size_t length) {
    uint8_t x;
    uint16_t crc = 0xFFFF;
    while (length--) {
        x = crc >> 8 ^ *data_p++;
        x ^= x >> 4;
        crc = (crc << 8) ^ ((uint16_t)(x << 12)) ^ ((uint16_t)(x << 5)) ^ ((uint16_t)x);
    }
    return crc;
}
void EEPROM::eeprom_write_bytes(uint16_t addr, const uint8_t* values, uint16_t length) {
    uint8_t data[length + 2];
    data[0] = (addr >> 8) & 0xFF;  // High byte of address
    data[1] = addr & 0xFF;  // Low byte of address
    for (uint16_t i = 0; i < length; i++) {
        data[i + 2] = values[i];
    }
    i2c_write_blocking(i2c0, DEVADDR, data, length + 2, false);
    sleep_ms(10);  // Wait for internal write to complete
}
void EEPROM::eeprom_read_bytes(uint16_t addr, uint8_t* values, uint16_t length) {
    uint8_t data[2];
    data[0] = (addr >> 8) & 0xFF;  // High byte of address
    data[1] = addr & 0xFF;  // Low byte of address
    i2c_write_blocking(i2c0, DEVADDR, data, 2, true);  // Send address
    i2c_read_blocking(i2c0, DEVADDR, values, length, false);  // Read values
}
void EEPROM::write_log(log_entry *le, uint16_t *address){
    uint8_t data[LOG_ENTRY_SIZE]; // 64 bytes
    size_t le_size = strlen(le->message);
    if(le_size > 0 && le_size < 62) {
        memcpy(data, le->message, le_size);
        data[le_size] = '\0';
        uint16_t crc = crc16(data, 62);
        data[le_size + 1] = (uint8_t)(crc >> 8);
        data[le_size + 2] = (uint8_t) crc;
        uint8_t read_data[LOG_ENTRY_SIZE];
        if(*address < LOG_ENTRY_ADD_MAX){
            eeprom_read_bytes(*address, read_data, LOG_ENTRY_SIZE);
            if(read_data[0] == 0){
                eeprom_write_bytes(*address, data, LOG_ENTRY_SIZE);
            }
            *address += LOG_ENTRY_SIZE;
        }
        else{
            printf("delete\n");
            *address = 0;
            erase_log( *address);
        }
    }
}
void EEPROM::read_log(){
    uint8_t data[LOG_ENTRY_SIZE];
    uint16_t addr = 0;
    uint8_t read_data[LOG_ENTRY_SIZE];
    printf("LOG:\n");
    while (addr < LOG_ENTRY_ADD_MAX){
        eeprom_read_bytes(addr, read_data, LOG_ENTRY_SIZE);
        if(read_data[0] != 0) {
            if (strchr((char*)read_data, '\0') && strchr((char*)read_data, '\0') < (char*)&read_data[62]){
                if(crc16(read_data, 64) != 0    ) {
                    printf("%s\n", read_data);
                }
            }
        }
        addr += LOG_ENTRY_SIZE;
    }
}
void EEPROM::erase_log(uint16_t address){
    uint8_t data[LOG_ENTRY_SIZE] = {0};
    uint8_t read_data[LOG_ENTRY_SIZE];
    while (address < LOG_ENTRY_ADD_MAX){
        eeprom_write_bytes(address, data, LOG_ENTRY_SIZE);
        std::cout << "Log erased: "<< address << std::endl;
        address += LOG_ENTRY_SIZE;
    }
    address = 0;
    std::cout << "Log erased Done" << std::endl;
}
/*void write_string(uint16_t address, const std::string &str) {
    if (str.size() > LOG_ENTRY_SIZE - 1) {
        printf("String is too long to write to EEPROM.\n");
        return;
    }

    uint8_t data[LOG_ENTRY_SIZE] = {0};
    memcpy(data, str.c_str(), str.size());
    eeprom_write_bytes(address, data, LOG_ENTRY_SIZE);
}

std::string read_string(uint16_t address) {
    uint8_t data[LOG_ENTRY_SIZE] = {0};
    eeprom_read_bytes(address, data, LOG_ENTRY_SIZE);

    // Assuming the string is null-terminated in the EEPROM
    std::string str(reinterpret_cast<char*>(data));
    return str;
}*/
void EEPROM::write_string(uint16_t address, const std::string &str) {
    if (str.size() > LOG_ENTRY_SIZE - 3) { // Reserve 2 bytes for CRC
        printf("String is too long to write to EEPROM.\n");
        return;
    }
    std::string::size_type str_size = str.size();
    uint8_t data[LOG_ENTRY_SIZE] = {0};
    if (str_size > 0 && str_size < 62  ) {
        memcpy(data, str.c_str(), str.size());
        // add null terminator
        data[str_size] = '\0';

        // Compute and append CRC
        uint16_t crc = crc16(data, str.size());
        data[str.size() + 1] = (uint8_t)(crc >> 8);
        data[str.size() + 2] = (uint8_t) crc;

        eeprom_write_bytes(address, data, LOG_ENTRY_SIZE);
    }

}

std::string EEPROM::read_string(uint16_t address) {
    uint8_t data[LOG_ENTRY_SIZE] = {0};
    eeprom_read_bytes(address, data, LOG_ENTRY_SIZE);

    // Extract CRC from data
    uint16_t crc = ((uint16_t)data[LOG_ENTRY_SIZE - 2] << 8) | data[LOG_ENTRY_SIZE - 1];
    // size of data
    size_t le_size = strlen((char*)data);
    // Check CRC
    /*if (crc16(data, 64) != 0) {
        printf("CRC check failed.\n");
        return "";
    }*/
    // Assuming the string is null-terminated in the EEPROM
    std::string str(reinterpret_cast<char*>(data));
    return str;
}
void EEPROM::write_number(uint16_t address, uint16_t number) {
    uint8_t data[2];
    data[0] = (number >> 8) & 0xFF;  // High byte of number
    data[1] = number & 0xFF;  // Low byte of number
    eeprom_write_bytes(address, data, 2);
}
uint16_t EEPROM::read_number(uint16_t address) {
    uint8_t data[64];
    eeprom_read_bytes(address, data, 64);

    // Combine high and low bytes into a number
    uint16_t number = ((uint16_t)data[0] << 8) | data[1];
    return  number;
}