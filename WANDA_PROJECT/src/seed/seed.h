#ifndef SEED_H
#define SEED_H

#include <Arduino.h>
#include <vector> // Necessário para std::vector
#include "wordlist.h"

class SeedManager {
public:
    SeedManager();
    
    void generateSeed();
    void restoreSeed(const String& mnemonic);
    String getMnemonic() const;
    bool isValidMnemonic(const String& mnemonic) const;

private:
    String _mnemonic;
    
    void entropyToMnemonic(const uint8_t* entropy, size_t entropyLen, String& mnemonic);
    bool mnemonicToEntropy(const String& mnemonic, uint8_t* entropy, size_t& entropyLen) const;
    
    // Correção da assinatura da função para corresponder à definição em seed.cpp
    uint32_t binaryToInt(const std::vector<bool>& binary, size_t start, size_t length);
};

#endif // SEED_H
