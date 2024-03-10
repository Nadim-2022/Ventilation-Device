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

    std::shared_ptr<PicoUart> uart;
    std::shared_ptr<ModbusClient> rtu_clients;
    std::map<std::string, std::shared_ptr<ModbusRegister>> registers;

private:

};

#endif //VENTILATION_DEVICE_VENTILATION_H
 /*  I2C_Display tft(14, 15, i2c1);

*//* ModbusManager modbus1(UART_NR, UART_TX_PIN, UART_RX_PIN, BAUD_RATE);
    modbus1.writeFan(500);
    sleep_ms(100);*//*
Ventilation vent(UART_NR, UART_TX_PIN, UART_RX_PIN, BAUD_RATE);
vent.addSensor("fan",1, 0);
vent.addSensor("Co2",240, 256);
vent.registers["fan"]->write(1000);
sleep_ms(100);

while (true) {
printf("Fan: %d\n", vent.registers["fan"]->read());
printf("Co2: %d\n", vent.registers["Co2"]->read());

sleep_ms(1000);
*//* printf("Relative Humidity: %d\n", modbus1.readRelativeHumidity());
 printf("Temperature: %d\n", modbus1.readTemperature());
 printf("CO2: %d\n", modbus1.readCO2());
 printf("Fan: %d\n", modbus1.readFan());
 sleep_ms(5000);*//*
}

return 0;
}
 */