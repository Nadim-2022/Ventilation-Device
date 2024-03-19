//
// Created by iamna on 16/03/2024.
//

#include "ModbusController.h"

ModbusController::ModbusController( int uart_nr, int tx_pin, int rx_pin, int baud_rate) : ModbusClient(std::make_shared<PicoUart>(uart_nr, tx_pin, rx_pin, baud_rate)) {
    uart = std::make_shared<PicoUart>(uart_nr, tx_pin, rx_pin, baud_rate);
    rtu_clients = std::make_shared<ModbusClient>(uart);
    pressure = 0;
    sensorValue = 0;
}
void ModbusController::addSensor(const std::string& sensorName, int serverAddress, int registerAddress){
    auto sensor = std::make_shared<ModbusRegister>(rtu_clients, serverAddress, registerAddress);
    registers[sensorName] = sensor;
}

void ModbusController::writeSensor(const std::string& sensorName, uint16_t value){
    registers[sensorName]->write(value);
    sleep_ms(20);
}
int ModbusController::readSensor(const std::string& sensorName){
    sensorValue = registers[sensorName]->read();
    sleep_ms(100);
    return sensorValue;
}

int ModbusController::readPressure(){
    i2c_write_blocking(i2c1, 64, data, 1, false);  // Send address
    sleep_ms(10);
    i2c_read_blocking(i2c1, 64, values, 2, false);  // Read values
    sleep_ms(100);
    pressure = ( (values[0] << 8) | values[1]) /240 *0.95;
    pressure = (pressure <= 0) ? 0 : (pressure > 127) ? 0 : pressure;
    return pressure;
}