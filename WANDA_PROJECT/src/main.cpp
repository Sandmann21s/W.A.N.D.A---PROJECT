#include <Arduino.h>
#include "display/display.h"
#include "buttons/buttons.h"
#include "seed/seed.h"
#include "crypto/crypto.h"
#include "communication/communication.h"
#include <Preferences.h>
#include "seed/wordlist.h"

// Definições dos botões para SSD1306
#define SSD1306_BUTTON_PIN1 1 // Botão "Próximo"
#define SSD1306_BUTTON_PIN2 2 // Botão "Selecionar"

// Instâncias das classes
Display display;
Buttons* buttonsInstance = nullptr;
SeedManager seedManager;
Crypto crypto;
Communication communication;
Preferences preferences;

// Prototipação das funções
void generateSeed();
void restoreSeed();
void settingsMenu();
void displaySeed(const String& mnemonic);
String selectWord(uint8_t wordNumber);

void setup() {
    Serial.begin(115200);
    while (!Serial) {
        ; // Aguarda conexão com o host
    }

    // Inicialização dos botões para SSD1306
    buttonsInstance = new Buttons(SSD1306_BUTTON_PIN1, SSD1306_BUTTON_PIN2);
    buttonsInstance->init();

    communication.init();

    if (!display.init()) {
        Serial.println("Falha ao inicializar o display.");
        while (true); // Loop infinito se falhar
    }

    // Associar a instância de Buttons à classe Display
    display.setButtons(buttonsInstance);

    // Exibir mensagem de boas-vindas
    display.showMessage("Hardware Wallet", "Bem-vindo!");
    delay(2000);
}

void loop() {
    String menuItems[] = {"Gerar Seed", "Restaurar Seed", "Configurações"};
    static uint8_t selectedItem = 0; // 'static' para preservar o valor entre chamadas de loop()
    uint8_t itemCount = sizeof(menuItems) / sizeof(menuItems[0]);

    buttonsInstance->update();

    if (buttonsInstance->isButton1Pressed()) {
        // Botão "Próximo" pressionado
        selectedItem = (selectedItem + 1) % itemCount;
    }

    if (buttonsInstance->isButton2Pressed()) {
        // Botão "Selecionar" pressionado
        switch (selectedItem) {
            case 0:
                generateSeed();
                break;
            case 1:
                restoreSeed();
                break;
            case 2:
                settingsMenu();
                break;
        }
        // Após a ação, volta ao menu principal
    }

    display.drawMenu(menuItems, itemCount, selectedItem);
    communication.handleRequests();
    delay(100); // Pequeno atraso para evitar uso excessivo da CPU
}

void generateSeed() {
    seedManager.generateSeed();
    String mnemonic = seedManager.getMnemonic();

    // Solicitar passphrase ao usuário
    String passphrase = "";
    display.inputText(passphrase, "Defina uma senha"); // Assegure-se de que Display::inputText está implementado

    crypto.setPassphrase(passphrase);

    uint8_t encryptedData[512];
    size_t length;
    if (crypto.encrypt(mnemonic, encryptedData, length)) {
        // Armazenar na NVS
        preferences.begin("wallet", false);
        preferences.putBytes("seed", encryptedData, length);
        preferences.end();

        display.showMessage("Sucesso", "Seed gerada e armazenada.");
    } else {
        display.showMessage("Erro", "Falha ao criptografar a seed.");
    }
    delay(2000);

    // Exibir a seed para o usuário
    displaySeed(mnemonic);
}

void displaySeed(const String& mnemonic) {
    // Dividir o mnemonic em palavras individuais
    String words[24]; // Até 24 palavras
    int wordIndex = 0;
    int start = 0;
    for (int i = 0; i <= mnemonic.length(); i++) {
        if (i == mnemonic.length() || mnemonic.charAt(i) == ' ') {
            words[wordIndex++] = mnemonic.substring(start, i);
            start = i + 1;
        }
    }

    // Exibir as palavras em páginas
    uint8_t totalPages = (wordIndex + 3) / 4; // 4 palavras por página
    uint8_t currentPage = 0;

    while (true) {
        display.clear();
        display.printText("Seed:", 0, 0);
        for (uint8_t i = 0; i < 4; i++) {
            uint8_t wordNum = currentPage * 4 + i;
            if (wordNum < wordIndex) {
                String wordLine = String(wordNum + 1) + ". " + words[wordNum];
                display.printText(wordLine, 0, (i + 1) * 10);
            }
        }
        display.displayContent();

        buttonsInstance->update();
        if (buttonsInstance->isButton1Pressed()) {
            // Próxima página
            currentPage = (currentPage + 1) % totalPages;
        }
        if (buttonsInstance->isButton2Pressed()) {
            // Sair
            break;
        }
        delay(100);
    }
}

void restoreSeed() {
    const uint8_t totalWords = 12; // Ajuste conforme necessário
    String mnemonicWords[totalWords];

    for (uint8_t i = 0; i < totalWords; i++) {
        String word = selectWord(i + 1);
        if (word == "") {
            display.showMessage("Erro", "Processo cancelado.");
            return;
        }
        mnemonicWords[i] = word;
    }

    // Reconstruir o mnemonic
    String mnemonic = mnemonicWords[0];
    for (uint8_t i = 1; i < totalWords; i++) {
        mnemonic += " " + mnemonicWords[i];
    }

    if (seedManager.isValidMnemonic(mnemonic)) {
        seedManager.restoreSeed(mnemonic);
        display.showMessage("Seed Restaurada", "Seed válida.");
    } else {
        display.showMessage("Erro", "Seed inválida.");
    }
    delay(2000);
}

String selectWord(uint8_t wordNumber) {
    // Acessar a lista de palavras BIP39
    const char* const* wordList = BIP39_WORDLIST;
    const uint16_t wordListSize = 2048;

    char currentLetter = 'a';
    uint16_t filteredIndices[2048];
    uint16_t filteredCount = 0;

    // Filtrar palavras que começam com currentLetter
    for (uint16_t i = 0; i < wordListSize; i++) {
        if (wordList[i][0] == currentLetter) {
            filteredIndices[filteredCount++] = i;
        }
    }

    while (true) {
        // Exibir letra atual
        display.clear();
        display.printText("Palavra " + String(wordNumber), 0, 0);
        display.printText("Letra: " + String(currentLetter), 0, 20);
        display.displayContent();

        buttonsInstance->update();
        if (buttonsInstance->isButton1Pressed()) {
            // Próxima letra
            if (currentLetter < 'z') {
                currentLetter++;
            } else {
                currentLetter = 'a';
            }

            // Atualizar filtro
            filteredCount = 0;
            for (uint16_t i = 0; i < wordListSize; i++) {
                if (wordList[i][0] == currentLetter) {
                    filteredIndices[filteredCount++] = i;
                }
            }
        }

        if (buttonsInstance->isButton2Pressed()) {
            // Selecionar palavra
            break;
        }
        delay(100);
    }

    // Navegar pelas palavras filtradas
    uint16_t selectedIndex = 0;
    while (true) {
        display.clear();
        display.printText("Selecione a palavra", 0, 0);
        if (filteredCount > 0) {
            display.printText(wordList[filteredIndices[selectedIndex]], 0, 20);
        } else {
            display.printText("Nenhuma palavra", 0, 20);
        }
        display.displayContent();

        buttonsInstance->update();
        if (buttonsInstance->isButton1Pressed()) {
            // Próxima palavra
            if (filteredCount > 0) {
                selectedIndex = (selectedIndex + 1) % filteredCount;
            }
        }
        if (buttonsInstance->isButton2Pressed()) {
            // Palavra selecionada
            if (filteredCount > 0) {
                return String(wordList[filteredIndices[selectedIndex]]);
            } else {
                return "";
            }
        }
        delay(100);
    }

    return "";
}

void settingsMenu() {
    display.showMessage("Configurações", "Função não implementada.");
    delay(2000);
}
