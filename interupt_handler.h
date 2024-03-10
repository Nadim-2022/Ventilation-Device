//
// Created by Aleksi Merilainen on 1.3.2024.
//

#ifndef VENTILATION_CONTROLLER_PROJECT_INTERUPT_HANDLER_H
#define VENTILATION_CONTROLLER_PROJECT_INTERUPT_HANDLER_H

#include "pico/stdlib.h"
#include "hardware/gpio.h"
#include "hardware/irq.h"
#include <iostream>

// Gpio 12 for rotary Button
class InterruptHandler {
private:
    static void callback(uint gpio, uint32_t events);
    static uint16_t timestamp;



public:
    int getCount();
    explicit InterruptHandler(uint8_t PinA);
    void setupPin(uint8_t pin);
    static int count;
    static bool rotaryturned;
};

#endif //VENTILATION_CONTROLLER_PROJECT_INTERUPT_HANDLER_H
