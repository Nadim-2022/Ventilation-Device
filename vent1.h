//
// Created by iamna on 09/03/2024.
//
#include <utility>

#include "pico/stdlib.h"
#include "hardware/i2c.h"
#include "ModbusClient.h"
#include "ModbusRegister.h"
#include "pico/time.h"
#include "hardware/timer.h"
#include "uart/PicoUart.h"

#ifndef VENTILATION_DEVICE_VENT1_H
#define VENTILATION_DEVICE_VENT1_H
class Ventilation : public ModbusRegister{
public:
    explicit Ventilation(int uart_nr, int tx_pin, int rx_pin, int baud_rate)
            : ModbusRegister(  std::make_shared<ModbusClient>(std::make_shared<PicoUart>(uart_nr, tx_pin, rx_pin, baud_rate)), 0, 0, true) {
        uart = std::make_shared<PicoUart>(UART_NR, UART_TX_PIN, UART_RX_PIN, BAUD_RATE);
        rtu_clients = std::make_shared<ModbusClient>(uart);
    }
    void addSensor( std::shared_ptr<ModbusRegister> sensor){
        registers.push_back(sensor);
    }

    std::shared_ptr<PicoUart> uart;
    std::shared_ptr<ModbusClient> rtu_clients;
    std::vector<std::shared_ptr<ModbusRegister>> registers;

private:

};
// Implement the Ventilation class here

/*
2C_Display tft(14, 15, i2c1);

*/
/* ModbusManager modbus1(UART_NR, UART_TX_PIN, UART_RX_PIN, BAUD_RATE);
 modbus1.writeFan(500);
 sleep_ms(100);*//*

Ventilation vent(UART_NR, UART_TX_PIN, UART_RX_PIN, BAUD_RATE);
vent.addSensor(std::make_shared<ModbusRegister>(vent.rtu_clients, 1, 0));
vent.addSensor(std::make_shared<ModbusRegister>(vent.rtu_clients, 240, 256));
vent.registers[0]->write(100);
sleep_ms(100);
while (true) {
printf("Fan: %d\n", vent.registers[0]->read());
printf("Temperature: %d\n", vent.registers[1]->read());
sleep_ms(1000);
*/
/* printf("Relative Humidity: %d\n", modbus1.readRelativeHumidity());
 printf("Temperature: %d\n", modbus1.readTemperature());
 printf("CO2: %d\n", modbus1.readCO2());
 printf("Fan: %d\n", modbus1.readFan());
 sleep_ms(5000);*//*

}
*/


#endif //VENTILATION_DEVICE_VENT1_H
