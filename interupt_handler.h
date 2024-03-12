//
// Created by Aleksi Merilainen on 1.3.2024.
//

#ifndef VENTILATION_CONTROLLER_PROJECT_INTERUPT_HANDLER_H
#define VENTILATION_CONTROLLER_PROJECT_INTERUPT_HANDLER_H

#include "pico/stdlib.h"
#include "hardware/gpio.h"
#include "hardware/irq.h"
#include <iostream>
#include <map>


#include <map>

class InterruptHandler {
public:
    InterruptHandler(uint8_t PinA);
    void setupPin(uint8_t pin);
    static void callback(uint gpio, uint32_t events);
    int getCount();
    bool rotaryturned = false; // make this non-static
    bool buttonPressed = false;

private:
    static std::map<uint, InterruptHandler*> handlers;
    static uint32_t timestamp;
    static int count;
};


#endif //VENTILATION_CONTROLLER_PROJECT_INTERUPT_HANDLER_H
