//
// Created by Aleksi Merilainen on 5.3.2024.
//

#ifndef EEPROM_H
#define EEPROM_H

#include <cstdint>
#include "hardware/gpio.h"
#include "hardware/i2c.h"

#define I2C_PORT i2c0
#define EEPROM_ADDRESS 0x50

class EEPROM
{
public:
    explicit EEPROM(i2c_inst *i2c, uint16_t device_address = EEPROM_ADDRESS);
    ~EEPROM();

    void writeToMemory(uint16_t memory_address, uint8_t data);
    uint8_t readFromMemory(uint16_t memory_address);
    uint16_t crc16(const uint8_t *data_p, size_t length);

private:
    i2c_inst *i2c;                 // I2C instance
    uint16_t device_address;       // Device I2C address
};

#endif // EEPROM_H