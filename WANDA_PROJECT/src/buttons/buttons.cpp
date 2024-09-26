#include "buttons.h"

Buttons::Buttons(uint8_t button1Pin, uint8_t button2Pin)
    : _button1Pin(button1Pin), _button2Pin(button2Pin) {}

void Buttons::init() {
    pinMode(_button1Pin, INPUT_PULLUP);
    pinMode(_button2Pin, INPUT_PULLUP);
    _debouncer1.attach(_button1Pin);
    _debouncer1.interval(25);
    _debouncer2.attach(_button2Pin);
    _debouncer2.interval(25);
}

void Buttons::update() {
    _debouncer1.update();
    _debouncer2.update();
}

bool Buttons::isButton1Pressed() {
    return _debouncer1.fell();
}

bool Buttons::isButton2Pressed() {
    return _debouncer2.fell();
}
