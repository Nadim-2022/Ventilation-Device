//
// Created by iamna on 16/03/2024.
//

#ifndef PICO_MODBUS_INTERRUPT_HANDLER_H
#define PICO_MODBUS_INTERRUPT_HANDLER_H


#include "pico/stdlib.h"
#include "hardware/gpio.h"
#include "hardware/irq.h"
#include <iostream>
#include <map>
#include <memory>
#include <vector>

class Observer {
public:
    virtual void update(bool rotaryTurned, bool buttonPressed, int value) = 0;
};

class Interrupt_Handler {
public:
    Interrupt_Handler(uint8_t PinA);
    void setupPin(uint8_t pin);
    static void callback(uint gpio, uint32_t events);
    int getCount();
    bool rotaryturned = false;
    bool buttonPressed = false;
    int count;

private:
    static std::map<uint, Interrupt_Handler*> handlers;
    static uint32_t timestamp;
    //static int count;
};

#endif //PICO_MODBUS_INTERRUPT_HANDLER_H
