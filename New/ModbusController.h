//
// Created by iamna on 16/03/2024.
//

#ifndef PICO_MODBUS_MODBUSCONTROLLER_H
#define PICO_MODBUS_MODBUSCONTROLLER_H
#include <utility>
#include <map>
#include "pico/stdlib.h"
#include "hardware/i2c.h"
#include "ModbusClient.h"
#include "ModbusRegister.h"
#include "pico/time.h"
#include "hardware/timer.h"
#include "uart/PicoUart.h"

class ModbusController : public ModbusClient{
public:
    ModbusController( int uart_nr, int tx_pin, int rx_pin, int baud_rate);
    void addSensor(const std::string& sensorName, int serverAddress, int registerAddress);
    void writeSensor(const std::string& sensorName, uint16_t value);
    int readSensor(const std::string& sensorName);
    int readPressure();

private:
    int pressure;
    int sensorValue;
    uint8_t  data[1] = {0xF1};
    uint8_t  values[2] = {0};
    std::shared_ptr<PicoUart> uart;
    std::shared_ptr<ModbusClient> rtu_clients;
    std::map<std::string, std::shared_ptr<ModbusRegister>> registers;
};


#endif //PICO_MODBUS_MODBUSCONTROLLER_H
