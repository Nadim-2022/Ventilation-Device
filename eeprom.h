//
// Created by iamna on 19/03/2024.
//


#ifndef PICO_MODBUS_EEPROM_H
#define PICO_MODBUS_EEPROM_H


#include <hardware/i2c.h>
#include <string>
#include <cstring>
#include <iostream>
#include "pico/stdlib.h"

#define Baudrate 100000


class EEPROM {
private:
    static const uint16_t LOG_ENTRY_ADD_MAX = 256;
    static const uint16_t LOG_ENTRY_SIZE = 64;
    const uint16_t DEVADDR = 0x50;
    i2c_inst_t *i2c;
    uint16_t crc16(const uint8_t *data_p, size_t length);
    void eeprom_write_bytes(uint16_t addr, const uint8_t* values, uint16_t length);

    void eeprom_read_bytes(uint16_t addr, uint8_t* values, uint16_t length);
    struct log_entry {
        char message[64];
    };
public:
    EEPROM(i2c_inst_t *i2c,uint8_t sda, uint8_t scl, uint16_t devaddr);
    ~EEPROM();
    void write_log(log_entry *le, uint16_t *address);
    void read_log();
    void erase_log(uint16_t address);
    void write_string(uint16_t address, const std::string &str);

    std::string read_string(uint16_t address);
    void write_number(uint16_t address, uint16_t number);

    uint16_t read_number(uint16_t address);
};

#endif //PICO_MODBUS_EEPROM_H
