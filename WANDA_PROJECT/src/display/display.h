#ifndef DISPLAY_H
#define DISPLAY_H

#include <Arduino.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include "buttons/buttons.h"

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_RESET    -1

class Display {
public:
    Display();
    bool init();
    void showMessage(const String& title, const String& message);
    void drawMenu(String menuItems[], uint8_t itemCount, uint8_t selectedItem);
    void printText(const String& text, int x, int y);
    void clear();
    void displayContent();
    void inputText(String& input, const String& prompt);

    // Método para associar a classe Buttons
    void setButtons(Buttons* buttons) {
        _buttons = buttons;
    }

private:
    Adafruit_SSD1306* _ssd1306Display;
    Buttons* _buttons;
};

#endif // DISPLAY_H
