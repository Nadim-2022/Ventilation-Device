//
// Created by iamna on 05/03/2024.
//

#ifndef VENTILATION_DEVICE_VENTILATION_H
#define VENTILATION_DEVICE_VENTILATION_H
#include <utility>
#include <map>

#include "pico/stdlib.h"
#include "hardware/i2c.h"
#include "ModbusClient.h"
#include "ModbusRegister.h"
#include "pico/time.h"
#include "hardware/timer.h"
#include "uart/PicoUart.h"

#define UART_NR 1
#define UART_TX_PIN 4
#define UART_RX_PIN 5


#define BAUD_RATE 9600
class Ventilation : public ModbusRegister{
public:
    explicit Ventilation(int uart_nr, int tx_pin, int rx_pin, int baud_rate)
            : ModbusRegister(  std::make_shared<ModbusClient>(std::make_shared<PicoUart>(uart_nr, tx_pin, rx_pin, baud_rate)), 0, 0, true) {
        uart = std::make_shared<PicoUart>(UART_NR, UART_TX_PIN, UART_RX_PIN, BAUD_RATE);
        rtu_clients = std::make_shared<ModbusClient>(uart);
    }
    void addSensor(const std::string& sensorName, int serverAddress, int registerAddress){
        auto sensor = std::make_shared<ModbusRegister>(rtu_clients, serverAddress, registerAddress);
        registers[sensorName] = sensor;
    }

    void writeSensor(const std::string& sensorName, int value){
        registers[sensorName]->write(value);
        sleep_ms(10);
    }
    int readSensor(const std::string& sensorName){
        return registers[sensorName]->read();
    }

    int readPressure(){
        i2c_write_blocking(i2c1, 64, data, 1, false);  // Send address
        sleep_ms(10);
        i2c_read_blocking(i2c1, 64, values, 2, false);  // Read values
        sleep_ms(100);
        pressure = ( (values[0] << 8) | values[1]) /240 *0.95;
        pressure = (pressure <= 0) ? 0 : (pressure >= 125) ? 125 : pressure;
        return pressure;
    }

private:
    int pressure;
    uint8_t  data[1] = {0xF1};
    uint8_t  values[2] = {0};
    std::shared_ptr<PicoUart> uart;
    std::shared_ptr<ModbusClient> rtu_clients;
    std::map<std::string, std::shared_ptr<ModbusRegister>> registers;
};

#endif //VENTILATION_DEVICE_VENTILATION_H
