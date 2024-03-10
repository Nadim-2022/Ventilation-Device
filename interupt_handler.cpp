/*
//
// Created by Aleksi Merilainen on 1.3.2024.
//
#include "interupt_handler.h"

//std::map<uint, InterruptHandler*> InterruptHandler::handlers = {};
std::map<uint8_t, InterruptHandler*> InterruptHandler::interruptHandlers;
void InterruptHandler::setupPin(uint8_t pin) {
    gpio_init(pin);
    gpio_set_dir(pin, GPIO_IN);
    gpio_pull_up(pin);
    gpio_set_irq_enabled_with_callback(pin, GPIO_IRQ_EDGE_FALL, true, &callback);
    interruptHandlers[pin] = this; // store this instance in the map
}

void InterruptHandler::callback(uint gpio, uint32_t events) {
    InterruptHandler* handler = interruptHandlers[gpio]; // look up the instance
    //uint16_t time2 = time_us_32() - handler->timestamp;
    handler->count = 2; // set the count for this instance
    handler->rotaryturned = true; // set the flag for this instance
}
uint16_t InterruptHandler::timestamp = time_us_64();
int InterruptHandler::count = 0;
bool InterruptHandler::rotaryturned = false;


int InterruptHandler::getCount() {
    return count;
}*/
