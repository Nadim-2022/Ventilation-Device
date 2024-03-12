//
// Created by iamna on 11/03/2024.
//
#include "interupt_handler.h"

InterruptHandler::InterruptHandler(uint8_t PinA) {
    setupPin(PinA);
}
void InterruptHandler::setupPin(uint8_t pin) {
    gpio_init(pin);
    gpio_set_dir(pin, GPIO_IN);
    gpio_pull_up(pin);
    gpio_set_irq_enabled_with_callback(pin, GPIO_IRQ_EDGE_FALL, true, &callback);

}

void InterruptHandler::callback(uint gpio, uint32_t events) {
    uint16_t time2 = time_us_32() - timestamp;
    if (!gpio_get(12)){
        count=2;
        std::cout << "button pushed: " << count << std::endl;
    }else if (!gpio_get(11)){
        rotaryturned = true;
        count=1;
        std::cout << "Interrupt count: " << count << std::endl;
    }else{
        rotaryturned = true;
        count=-1;
        std::cout << "Interrupt count: " << count << std::endl;
    }

    /*if (time2 > 10) {

       *//* if (!gpio_get(11)){
            count=1;
            std::cout << "Interrupt count: " << count << std::endl;
        }
        else if (!gpio_get(12)){
            count=2;
            std::cout << "button pushed: " << count << std::endl;
        }
        else{
            count=0;
           std::cout << "Interrupt count: " << count << std::endl;
        }
*//*
        timestamp = time_us_32();
    }*/
}
uint16_t InterruptHandler::timestamp = time_us_32();
int InterruptHandler::count = 0;
bool InterruptHandler::rotaryturned = false;

int InterruptHandler::getCount() {
    return count;
}

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