//
// Created by iamna on 16/03/2024.
//

#include "Interrupt_Handler.h"



Interrupt_Handler::Interrupt_Handler(uint8_t PinA) {
    setupPin(PinA);
}

void Interrupt_Handler::setupPin(uint8_t pin) {
    gpio_init(pin);
    gpio_set_dir(pin, GPIO_IN);
    gpio_pull_up(pin);
    gpio_set_irq_enabled_with_callback(pin, GPIO_IRQ_EDGE_RISE, true,
                                       callback);
    handlers[pin] = this;

}

void Interrupt_Handler::callback(uint gpio, uint32_t events) {
    Interrupt_Handler* handler = handlers[gpio];
    if (gpio == 10) {
        if (time_us_32() - timestamp > 50000) {
            timestamp = time_us_32();
            if (gpio_get(11)) {
                handler->count = -1;
                handler->rotaryturned = true;
                std::cout << "Rotary turned 1" << std::endl;
            } else {
                handler->rotaryturned = true;
                handler->count = 1;
                std::cout << "Rotary turned -1" << std::endl;
            }
        }
    } else {
        if (time_us_32() - timestamp > 500000) {
            timestamp = time_us_32();
            handler->buttonPressed = true;
            handler->count = 1;
            std::cout << "Button pressed" << std::endl;
        }
    }
}

std::map<uint, Interrupt_Handler*> Interrupt_Handler::handlers = {};
uint32_t Interrupt_Handler::timestamp = time_us_32();
//int Interrupt_Handler::count = 0;
int Interrupt_Handler::getCount() {
    return count;
}