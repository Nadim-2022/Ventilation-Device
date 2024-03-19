//
// Created by iamna on 16/03/2024.
//

#ifndef PICO_MODBUS_MENU_H
#define PICO_MODBUS_MENU_H


class Menu {
public:
    Menu();
    enum menuEvent {
        start,
        mainMenu,
        speedControl,
        pressureControl,
        status
    };
    virtual bool event(menuEvent e) = 0;
    virtual void show() = 0;
    virtual ~Menu() = default;
protected:
    /*bool rotaryTurned;
    bool rotaryPressed;
    bool buttonPressed;
    int counter = 0;*/

};


#endif //PICO_MODBUS_MENU_H
