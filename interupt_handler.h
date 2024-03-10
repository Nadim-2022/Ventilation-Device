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
    static uint16_t timestamp;
    static int count;
};

std::map<uint, InterruptHandler*> InterruptHandler::handlers = {};
uint16_t InterruptHandler::timestamp = time_us_64();
int InterruptHandler::count = 0;
int InterruptHandler::getCount() {
    return count;
}

InterruptHandler::InterruptHandler(uint8_t PinA) {
    setupPin(PinA);
}

void InterruptHandler::setupPin(uint8_t pin) {
    gpio_init(pin);
    gpio_set_dir(pin, GPIO_IN);
    gpio_pull_up(pin);
    gpio_set_irq_enabled_with_callback(pin, GPIO_IRQ_EDGE_FALL, true, &callback);
    handlers[pin] = this; // store this instance in the map
}

void InterruptHandler::callback(uint gpio, uint32_t events) {
    InterruptHandler* handler = handlers[gpio]; // look up the instance
    uint16_t time2 = time_us_32() - timestamp;
    // if not rothandlerA then count = -1
    // if not rothandlerA then count = -1
    if (gpio == 10) { // replace rotA with the actual GPIO pin number for rotA
        if (gpio_get(11)) {
            handler->count = 1; // set the count for this instance
            handler->rotaryturned = true; // set the flag for this instance
        } else {
            handler->rotaryturned = true;
            handler->count = -1;
        }
    } else {
        handler->buttonPressed = true;
        handler->count = 1;
    }
    //handler->count = 1; // set the count for this instance
    // set the flag for this instance
}
#endif //VENTILATION_CONTROLLER_PROJECT_INTERUPT_HANDLER_H
