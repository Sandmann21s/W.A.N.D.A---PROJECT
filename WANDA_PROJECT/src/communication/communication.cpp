#include "communication.h"

Communication::Communication() {}

void Communication::init() {
    // Inicialização adicional, se necessário
}

void Communication::handleRequests() {
    if (Serial.available()) {
        String command = Serial.readStringUntil('\n');
        processCommand(command);
    }
}

void Communication::processCommand(const String& command) {
    // Exemplo de processamento de comandos simples
    if (command == "get_pubkey") {
        // Retornar a chave pública derivada da seed
        String pubkey = "EXEMPLO_DE_CHAVE_PÚBLICA";
        Serial.println(pubkey);
    } else {
        Serial.println("Comando desconhecido");
    }
}
