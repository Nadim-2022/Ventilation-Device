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

void EEPROM::writeToMemory(uint16_t memory_address, uint8_t data)
{
    uint8_t buffer[3];
    buffer[0] = (memory_address >> 8) & 0xFF;
    buffer[1] = memory_address & 0xFF;
    buffer[2] = data;

    int result = i2c_write_blocking(I2C_PORT, EEPROM_ADDRESS, buffer, 3, false);
    if (result != 3) {
        printf("Failed to write data at address %u\n", memory_address);
    }
    sleep_ms(5);
}

uint8_t EEPROM::readFromMemory(uint16_t memory_address)
{
    uint8_t address_buffer[2];
    address_buffer[0] = (memory_address >> 8) & 0xFF; // high byte
    address_buffer[1] = memory_address & 0xFF; // low byte
    uint8_t data = 0;

    if (i2c_write_blocking(I2C_PORT, EEPROM_ADDRESS, address_buffer, 2, true) != 2) {
        printf("Failed to write at address %u\n", memory_address);
        return data;
    }

    if (i2c_read_blocking(I2C_PORT, EEPROM_ADDRESS, &data, 1, false) != 1) {
        printf("Failed to read at address %u\n", memory_address);
    }

    return data;
}

