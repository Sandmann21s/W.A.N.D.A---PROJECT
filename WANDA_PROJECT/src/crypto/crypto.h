#ifndef CRYPTO_H
#define CRYPTO_H

#include <Arduino.h>
#include <mbedtls/aes.h>
#include <mbedtls/sha256.h>
#include <mbedtls/pkcs5.h>

class Crypto {
public:
    Crypto();
    void setPassphrase(const String& passphrase);
    bool encrypt(const String& data, uint8_t* encryptedData, size_t& length);
    bool decrypt(const uint8_t* encryptedData, size_t length, String& data);

private:
    uint8_t _key[32]; // Chave de 256 bits
    void deriveKey(const String& passphrase);
};

#endif // CRYPTO_H
