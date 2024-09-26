#ifndef BUTTONS_H
#define BUTTONS_H

#include <Arduino.h>
#include <Bounce2.h>

class Buttons {
public:
    Buttons(uint8_t button1Pin, uint8_t button2Pin);
    void init();
    void update();
    bool isButton1Pressed();
    bool isButton2Pressed();

private:
    uint8_t _button1Pin;
    uint8_t _button2Pin;
    Bounce _debouncer1;
    Bounce _debouncer2;
};

#endif // BUTTONS_H
