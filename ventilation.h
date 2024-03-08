//
// Created by iamna on 05/03/2024.
//

#ifndef VENTILATION_DEVICE_VENTILATION_H
#define VENTILATION_DEVICE_VENTILATION_H
#include <utility>

#include "pico/stdlib.h"
#include "hardware/i2c.h"
#include "ModbusClient.h"
#include "ModbusRegister.h"
#include "pico/time.h"
#include "hardware/timer.h"
#include "uart/PicoUart.h"


class Ventilation:  public ModbusRegister{
public:
    Ventilation(std::shared_ptr<ModbusClient> rtu_client, int address, int value)
            : ModbusRegister(std::move(rtu_client), address, value) {
        // Initialization code here
    }
   void addSensor( std::shared_ptr<ModbusRegister> sensor){
        registers.push_back(sensor);
    }



private:
    std::shared_ptr<PicoUart> uart;
    std::shared_ptr<ModbusClient> rtu_clients;
    std::vector<std::shared_ptr<ModbusRegister>> registers;
};

#endif //VENTILATION_DEVICE_VENTILATION_H
