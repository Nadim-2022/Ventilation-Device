//
// Created by Aleksi Merilainen on 5.3.2024.
//
#include "EEPROM.h"
#include "pico/stdlib.h"
#include "hardware/i2c.h"
#include <cstdio>

#define SDA_PIN 16
#define SCL_PIN 17

EEPROM::EEPROM(i2c_inst *i2c, uint16_t device_address)
        : i2c(i2c), device_address(device_address)
{
    // I2C Initialization
    i2c_init(i2c, 100 * 1000);  // Initialize I2C at 100 kHz
    gpio_set_function(SDA_PIN, GPIO_FUNC_I2C);
    gpio_set_function(SCL_PIN, GPIO_FUNC_I2C);
    gpio_pull_up(SDA_PIN);
    gpio_pull_up(SCL_PIN);
}


EEPROM::~EEPROM()
{
    // Cleanup I2C associated pins
    gpio_set_function(SDA_PIN, GPIO_FUNC_SIO);
    gpio_set_function(SCL_PIN, GPIO_FUNC_SIO);
    gpio_pull_down(SDA_PIN);
    gpio_pull_down(SCL_PIN);
}
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

void EEPROM::writeToMemory(uint16_t memory_address, uint8_t data){
    uint8_t buffer[3];

    // Address for the EEPROM
    buffer[0] = (memory_address >> 8) & 0xFF; // high byte
    buffer[1] = memory_address & 0xFF;        // low byte

    // Data byte to be written
    buffer[2] = data;
    int result = i2c_write_blocking(this->i2c, this->device_address, buffer, 3, false);

    // Calculate the CRC
    uint16_t crc = crc16(&data, sizeof(data));

    // Write the CRC
    buffer[0] = ((memory_address + 1) >> 8) & 0xFF;  // high byte
    buffer[1] = (memory_address + 1) & 0xFF;         // low byte
    buffer[2] = crc; // CRC value
    result = i2c_write_blocking(this->i2c, this->device_address, buffer, 3, false);

    if (result != 3) {
        printf("Failed to write CRC at EEPROM address %u\n", memory_address + 1);
    } else {
        printf("CRC written successfully at EEPROM address %u.\n", memory_address + 1);
    }
}


uint8_t EEPROM::readFromMemory(uint16_t memory_address){

    // Prepare address buffer
    uint8_t address_buffer[2];
    address_buffer[0] = (memory_address >> 8) & 0xFF; // Memory address high byte
    address_buffer[1] = memory_address & 0xFF;        // Memory address low byte

    // Read data
    uint8_t data;
    i2c_write_blocking(this->i2c, this->device_address, address_buffer, 2, true);
    i2c_read_blocking(this->i2c, this->device_address, &data, 1, false);

    // Read CRC
    uint16_t stored_crc = 0;
    address_buffer[0] = ((memory_address + 1) >> 8) & 0xFF; // CRC address high byte
    address_buffer[1] = (memory_address + 1) & 0xFF;        // CRC address low byte
    i2c_write_blocking(this->i2c, this->device_address, address_buffer, 2, true);
    i2c_read_blocking(this->i2c, this->device_address, reinterpret_cast<uint8_t *>(&stored_crc), 2, false);

    // Validate CRC
    uint16_t computed_crc = crc16(&data, sizeof(data));
    if (computed_crc != stored_crc) {
        // Print error or handle situation accordingly
        printf("CRC check failed for EEPROM address %u.\n", memory_address);
    }

    return data;
}

