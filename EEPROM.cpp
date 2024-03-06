#include "EEPROM.h"
#include "hardware/i2c.h"
#include <cstdio>

#define SDA_PIN 16
#define SCL_PIN 17
#define EEPROM_SIZE 2048
#define I2C_WAIT_TIME 5

EEPROM::EEPROM(i2c_inst *i2c, uint16_t device_address)
        : i2c(i2c), device_address(device_address)
{
    i2c_init(i2c, 100 * 1000);
    gpio_set_function(SDA_PIN, GPIO_FUNC_I2C);
    gpio_set_function(SCL_PIN, GPIO_FUNC_I2C);
    gpio_pull_up(SDA_PIN);
    gpio_pull_up(SCL_PIN);
}

EEPROM::~EEPROM()
{
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

void EEPROM::writeToMemory(uint16_t memory_address, uint8_t data)
{
    if (memory_address > EEPROM_SIZE - 3) {
        printf("Memory address exceeds EEPROM size.\n");
        return;
    }

    this->writeByte(memory_address, data); // Write the data
    sleep_ms(I2C_WAIT_TIME);

    uint16_t crc = crc16(&data, 1);  // Calculate CRC

    uint8_t crc_highByte = (crc >> 8) & 0xFF, crc_lowByte = crc & 0xFF; // Split CRC into 2 bytes

    this->writeByte(memory_address+1, crc_highByte); // Write the CRC high byte
    sleep_ms(I2C_WAIT_TIME);

    this->writeByte(memory_address+2, crc_lowByte); // Write the CRC low byte
    sleep_ms(I2C_WAIT_TIME);
}

void EEPROM::clearEEPROM() {
    uint8_t fill = 0xFF;
    for (uint16_t i = 0; i < EEPROM_SIZE-2; i+=3)
    {
        this->writeToMemory(i, fill);
    }
}

uint8_t EEPROM::readFromMemory(uint16_t memory_address){
    uint8_t data;
    this->read_byte(memory_address, data); // Read in the data
    return data;
}

bool EEPROM::writeByte(uint16_t memory_address, uint8_t data){
    uint8_t buffer[3];
    buffer[0] = (memory_address >> 8) & 0xFF;
    buffer[1] = memory_address & 0xFF;
    buffer[2] = data;

    int result = i2c_write_blocking(this->i2c, this->device_address, buffer, 3, false);

    if(result != 3) {
        printf("Failed to write data at EEPROM address %u\n", memory_address);
        return false;
    } else {
        printf("Data written successfully at EEPROM address %u.\n", memory_address);
        return true;
    }
}

bool EEPROM::read_byte(uint16_t memory_address, uint8_t &data){
    uint8_t address_buffer[2];
    address_buffer[0] = (memory_address >> 8) & 0xFF;
    address_buffer[1] = memory_address & 0xFF;

    if (i2c_write_blocking(this->i2c, this->device_address, address_buffer, 2, true) != 2) {
        return false;
    }

    if (i2c_read_blocking(this->i2c, this->device_address, &data, 1, false) != 1) {
        return false;
    }

    return true;
}