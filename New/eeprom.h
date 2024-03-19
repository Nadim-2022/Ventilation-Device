//
// Created by iamna on 19/03/2024.
//


#ifndef PICO_MODBUS_EEPROM_H
#define PICO_MODBUS_EEPROM_H


#include <hardware/i2c.h>
#include <string>

class eeprom {
    public:
        eeprom(i2c_inst *i2c, uint16_t device_address);
        uint16_t crc16(const uint8_t *data_p, size_t length);
        void writeByte(uint16_t address, uint8_t *data0, uint16_t length);
        void readByte(uint16_t address, uint8_t *data, uint16_t length);
        void write_log(log_entry *le, uint16_t *address);
        std::string readFromEeprom(uint16_t address);

private:
        i2c_inst *i2c;
        uint16_t device_address;

};


#endif //PICO_MODBUS_EEPROM_H
