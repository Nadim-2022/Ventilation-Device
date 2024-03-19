//
// Created by iamna on 19/03/2024.
//

#include "eeprom.h"
#include <cstring>
#include <cstdio>
#include <string>

#define LOG_ENTRY_SIZE 64
#define LOG_ENTRY_ADD_MAX 1024

eeprom::eeprom(i2c_inst *i2c, uint16_t device_address = 0x50) {
    this->i2c = i2c;
}
uint16_t eeprom::crc16(const uint8_t *data_p, size_t length) {
    uint8_t x;
    uint16_t crc = 0xFFFF;
    while (length--) {
        x = crc >> 8 ^ *data_p++;
        x ^= x >> 4;
        crc = (crc << 8) ^ ((uint16_t)(x << 12)) ^ ((uint16_t)(x << 5)) ^ ((uint16_t)x);
    }
    return crc;
}

void eeprom::writeByte(uint16_t address, uint8_t *data0, uint16_t length){
    uint8_t data[length + 2];
    data[0] = (address >> 8) & 0xFF;  // High byte of address
    data[1] = address & 0xFF;  // Low byte of address
    for (uint16_t i = 0; i < length; i++) {
        data[i + 2] = data[i];
    }
    i2c_write_blocking(i2c, device_address, data, length + 2, false);
    sleep_ms(10);  // Wait for internal write to complete
}
void eeprom::readByte(uint16_t address, uint8_t *values, uint16_t length) {
    uint8_t data[2];
    data[0] = (address >> 8) & 0xFF;  // High byte of address
    data[1] = address & 0xFF;  // Low byte of address
    i2c_write_blocking(i2c, device_address, data, 2, true);  // Send address
    i2c_read_blocking(i2c, device_address, values, length, false);  // Read values

}

void write_log(log_entry *le, uint16_t *address){
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
            //printf("addr: %d\n", *address);
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
void read_log(){
    uint8_t data[LOG_ENTRY_SIZE];
    uint16_t addr = 0;
    uint8_t read_data[LOG_ENTRY_SIZE];
    printf("LOG:\n");
    while (addr < LOG_ENTRY_ADD_MAX){
        eeprom_read_bytes(addr, read_data, LOG_ENTRY_SIZE);
        if(read_data[0] != 0) {
            if (strchr(read_data, '\0') && strchr(read_data, '\0') < &read_data[62]){
                if(crc16(read_data, 64) != 0    ) {
                    printf("%s\n", read_data);
                }
            }
        }
        //printf("%s\n", read_data);
        addr += LOG_ENTRY_SIZE;
    }
}
void erase_log(uint16_t address){
    //To erase all the log entries, write 0x00 to the all the byte of each log entry.
    uint8_t data[LOG_ENTRY_SIZE] = {0};
    uint8_t read_data[LOG_ENTRY_SIZE];
    while (address < LOG_ENTRY_ADD_MAX){
        eeprom_write_bytes(address, data, LOG_ENTRY_SIZE);
        address += LOG_ENTRY_SIZE;
    }
    address = 0;
}

