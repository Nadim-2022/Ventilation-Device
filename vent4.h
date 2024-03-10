//
// Created by iamna on 09/03/2024.
//

#ifndef VENTILATION_DEVICE_VENT4_H
#define VENTILATION_DEVICE_VENT4_H
class ModbusManager {
private:
    std::shared_ptr<ModbusClient> client_;
    ModbusRegister relativeHumidity_;
    ModbusRegister temperature_;
    ModbusRegister co2_;
    ModbusRegister fan_;

public:
    ModbusManager(int uartNr, int txPin, int rxPin, int baudRate)
            : client_(std::make_shared<ModbusClient>(std::make_shared<PicoUart>(uartNr, txPin, rxPin, baudRate))),
              relativeHumidity_(client_, 241, 256),
              temperature_(client_, 241, 257),
              co2_(client_, 240, 256),
              fan_(client_, 1, 0) {}

    int readRelativeHumidity() { return relativeHumidity_.read(); }
    int readTemperature() { return temperature_.read(); }
    int readCO2() { return co2_.read(); }
    int readFan() { return fan_.read(); }

    void writeFan(int value) { fan_.write(value); }
};
/*int main() {
    ModbusManager modbusManager(UART_NR, UART_TX_PIN, UART_RX_PIN, BAUD_RATE);

    int humidity = modbusManager.readRelativeHumidity();
    int temperature = modbusManager.readTemperature();
    int co2 = modbusManager.readCO2();
    int fanValue = modbusManager.readFan();

    // Use the values as needed

    return 0;
}*/
#endif //VENTILATION_DEVICE_VENT4_H
