#include "seed.h"
#include <vector> // Necessário para std::vector
#include <mbedtls/sha256.h>

SeedManager::SeedManager() {}

void SeedManager::generateSeed() {
    // Gerar entropia
    const size_t entropyLen = 16; // 128 bits
    uint8_t entropy[entropyLen];
    for (size_t i = 0; i < entropyLen; i++) {
        entropy[i] = static_cast<uint8_t>(esp_random());
    }

    // Converter entropia em mnemonic
    String mnemonic;
    entropyToMnemonic(entropy, entropyLen, mnemonic);
    _mnemonic = mnemonic;
}

void SeedManager::restoreSeed(const String& mnemonic) {
    if (isValidMnemonic(mnemonic)) {
        _mnemonic = mnemonic;
    } else {
        _mnemonic = "";
    }
}

String SeedManager::getMnemonic() const {
    return _mnemonic;
}

bool SeedManager::isValidMnemonic(const String& mnemonic) const {
    uint8_t entropy[32];
    size_t entropyLen = 0;
    return mnemonicToEntropy(mnemonic, entropy, entropyLen);
}

void SeedManager::entropyToMnemonic(const uint8_t* entropy, size_t entropyLen, String& mnemonic) {
    // Calcular o checksum
    uint8_t hash[32];
    mbedtls_sha256(entropy, entropyLen, hash, 0);

    // Concatenar entropia com checksum
    size_t checksumBits = entropyLen * 8 / 32;
    size_t totalBits = entropyLen * 8 + checksumBits;
    std::vector<bool> bits;

    for (size_t i = 0; i < entropyLen; i++) {
        for (int8_t j = 7; j >= 0; j--) {
            bits.push_back((entropy[i] >> j) & 1);
        }
    }

    for (size_t i = 0; i < checksumBits; i++) {
        bits.push_back((hash[0] >> (7 - i)) & 1);
    }

    // Dividir bits em palavras
    size_t numWords = totalBits / 11;
    mnemonic = "";
    for (size_t i = 0; i < numWords; i++) {
        uint32_t index = binaryToInt(bits, i * 11, 11);
        if (mnemonic.length() > 0) {
            mnemonic += " ";
        }
        mnemonic += BIP39_WORDLIST[index];
    }
}

bool SeedManager::mnemonicToEntropy(const String& mnemonic, uint8_t* entropy, size_t& entropyLen) const {
    // Dividir o mnemonic em palavras
    std::vector<String> words;
    size_t start = 0;
    for (size_t i = 0; i <= mnemonic.length(); i++) {
        if (i == mnemonic.length() || mnemonic.charAt(i) == ' ') {
            words.push_back(mnemonic.substring(start, i));
            start = i + 1;
        }
    }

    size_t numWords = words.size();
    if (numWords != 12 && numWords != 15 && numWords != 18 && numWords != 21 && numWords != 24) {
        return false;
    }

    size_t totalBits = numWords * 11;
    std::vector<bool> bits;

    // Converter palavras em bits
    for (size_t i = 0; i < numWords; i++) {
        int index = -1;
        for (size_t j = 0; j < 2048; j++) {
            if (words[i] == BIP39_WORDLIST[j]) {
                index = j;
                break;
            }
        }
        if (index == -1) {
            return false; // Palavra inválida
        }
        for (int j = 10; j >= 0; j--) {
            bits.push_back((index >> j) & 1);
        }
    }

    size_t checksumBits = totalBits % 32;
    entropyLen = (totalBits - checksumBits) / 8;
    for (size_t i = 0; i < entropyLen; i++) {
        entropy[i] = 0;
        for (int j = 0; j < 8; j++) {
            entropy[i] = (entropy[i] << 1) | bits[i * 8 + j];
        }
    }

    // Verificar o checksum
    uint8_t hash[32];
    mbedtls_sha256(entropy, entropyLen, hash, 0);

    for (size_t i = 0; i < checksumBits; i++) {
        bool bit = (hash[0] >> (7 - i)) & 1;
        if (bits[entropyLen * 8 + i] != bit) {
            return false; // Checksum inválido
        }
    }

    return true;
}

uint32_t SeedManager::binaryToInt(const std::vector<bool>& binary, size_t start, size_t length) {
    uint32_t value = 0;
    for (size_t i = start; i < start + length; i++) {
        value = (value << 1) | binary[i];
    }
    return value;
}
