#include "display.h"

// Inicialização do display SSD1306
Display::Display() : _ssd1306Display(nullptr), _buttons(nullptr) {
    _ssd1306Display = new Adafruit_SSD1306(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);
}

bool Display::init() {
    if (!_ssd1306Display->begin(SSD1306_SWITCHCAPVCC, 0x3C)) { // Endereço I2C padrão
        return false;
    }
    _ssd1306Display->clearDisplay();
    _ssd1306Display->display();
    return true;
}

void Display::showMessage(const String& title, const String& message) {
    clear();
    printText(title, 0, 0);
    printText(message, 0, 20);
    displayContent();
}

void Display::drawMenu(String menuItems[], uint8_t itemCount, uint8_t selectedItem) {
    clear();
    for (uint8_t i = 0; i < itemCount; i++) {
        String line = (i == selectedItem) ? "> " + menuItems[i] : "  " + menuItems[i];
        printText(line, 0, i * 10);
    }
    displayContent();
}

void Display::printText(const String& text, int x, int y) {
    _ssd1306Display->setCursor(x, y);
    _ssd1306Display->setTextSize(1);
    _ssd1306Display->setTextColor(SSD1306_WHITE);
    _ssd1306Display->print(text);
}

void Display::clear() {
    _ssd1306Display->clearDisplay();
}

void Display::displayContent() {
    _ssd1306Display->display();
}

void Display::inputText(String& input, const String& prompt) {
    input = "";
    char currentChar = 'a'; // Letra inicial
    while (true) {
        clear();
        printText(prompt, 0, 0);
        printText(String(currentChar), 0, 20);
        printText("Confirme com botão 2", 0, 30);
        displayContent();

        if (_buttons) {
            _buttons->update();
            if (_buttons->isButton1Pressed()) {
                // Próximo caractere
                if (currentChar < 'z') {
                    currentChar++;
                } else {
                    currentChar = 'a';
                }
            }
            if (_buttons->isButton2Pressed()) {
                // Selecionar caractere
                input += String(currentChar);
                break;
            }
        }
        delay(100);
    }
}
