
//
// Created by Aleksi Merilainen on 1.3.2024.
//
#include "interupt_handler.h"

std::map<uint, InterruptHandler*> InterruptHandler::handlers = {};
uint32_t InterruptHandler::timestamp = time_us_32();
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
   // uint16_t time2 = time_us_32() - timestamp;
    /*if (time_us_32() - timestamp > 500000) {
        timestamp = time_us_32();
        if (gpio == 10) {
            if (gpio_get(11)) {
                handler->count = 1;
                handler->rotaryturned = true;
            } else {
                handler->rotaryturned = true;
                handler->count = -1;
            }
        } else {
            handler->buttonPressed = true;
            handler->count = 1;
        }
    }
*/

        if (gpio == 10) {
            if (gpio_get(11)) {
                handler->count = 1;
                handler->rotaryturned = true;
            } else {
                handler->rotaryturned = true;
                handler->count = -1;
            }
        } else {
            if (time_us_32() - timestamp > 500000) {
                timestamp = time_us_32();
                handler->buttonPressed = true;
                handler->count = 1;
            }
        }
    }

