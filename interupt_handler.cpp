//
// Created by Aleksi Merilainen on 1.3.2024.
//
#include "interupt_handler.h"

InterruptHandler::InterruptHandler(uint8_t PinA0, uint8_t pinC0) {
    setupPin(PinA0);
    setupPin(pinC0);
}
void InterruptHandler::setupPin(uint8_t pin) {
    gpio_init(pin);
    gpio_set_dir(pin, GPIO_IN);
    gpio_pull_up(pin);
    gpio_set_irq_enabled_with_callback(pin, GPIO_IRQ_EDGE_RISE, true, &callback);
}

void InterruptHandler::callback(uint gpio, uint32_t events) {
    uint16_t time2 = time_us_32() - timestamp;
    if (time2 > 6000) {
        if (!gpio_get(11)){
            count++;
            std::cout << "Interrupt count: " << count << std::endl;
        } else{
            count --;
            std::cout << "Interrupt count: " << count << std::endl;
        }
        timestamp = time_us_32();
    }
}
uint16_t InterruptHandler::timestamp = time_us_64();
int InterruptHandler::count = 0;