#include "crypto.h"
#include <cstring>          // Para memset e memcpy
#include <mbedtls/md.h>     // Para funções de hash
#include <mbedtls/pkcs5.h>  // Para PBKDF2

Crypto::Crypto() {
    memset(_key, 0, sizeof(_key));
}

void Crypto::setPassphrase(const String& passphrase) {
    deriveKey(passphrase);
}

void Crypto::deriveKey(const String& passphrase) {
    // Deriva a chave usando PBKDF2 com SHA-256
    const uint8_t salt[] = "hardware_wallet_salt"; // Sal fixo para simplificação
    const size_t saltLen = strlen((const char*)salt);
    const int iterations = 1000;

    // Inicializar o contexto de hash
    mbedtls_md_context_t md_ctx;
    mbedtls_md_init(&md_ctx);

    const mbedtls_md_info_t* md_info = mbedtls_md_info_from_type(MBEDTLS_MD_SHA256);
    if (md_info == nullptr) {
        Serial.println("Erro: Tipo de hash SHA256 não suportado.");
        memset(_key, 0, sizeof(_key));
        return;
    }

    // Configurar o contexto com o tipo de hash
    if (mbedtls_md_setup(&md_ctx, md_info, 1) != 0) { // 1 para HMAC
        Serial.println("Erro: Falha ao configurar o contexto de hash.");
        mbedtls_md_free(&md_ctx);
        memset(_key, 0, sizeof(_key));
        return;
    }

    // Derivar a chave usando PBKDF2
    int ret = mbedtls_pkcs5_pbkdf2_hmac(
        &md_ctx,                                      // Contexto de hash
        (const unsigned char*)passphrase.c_str(),     // Senha
        passphrase.length(),                          // Comprimento da senha
        salt,                                         // Sal
        saltLen,                                      // Comprimento do sal
        iterations,                                   // Iterações
        sizeof(_key) * 8,                             // Comprimento da chave em bits
        _key                                          // Saída da chave derivada
    );

    if (ret != 0) {
        Serial.println("Erro na derivação da chave");
        memset(_key, 0, sizeof(_key));
    }

    // Liberar o contexto de hash
    mbedtls_md_free(&md_ctx);
}

bool Crypto::encrypt(const String& data, uint8_t* encryptedData, size_t& length) {
    if (data.length() == 0) return false;

    // Inicializar vetor de inicialização (IV)
    uint8_t iv[16];
    for (int i = 0; i < 16; i++) {
        iv[i] = (uint8_t)esp_random();
    }

    // Copiar IV para os primeiros 16 bytes do encryptedData
    memcpy(encryptedData, iv, 16);

    // Preparar dados com padding PKCS#7
    size_t dataLen = data.length();
    size_t paddedLen = ((dataLen + 15) / 16) * 16;
    uint8_t* buffer = new uint8_t[paddedLen];
    memcpy(buffer, data.c_str(), dataLen);
    uint8_t padding = paddedLen - dataLen;
    memset(buffer + dataLen, padding, padding);

    // Criptografar usando AES-256-CBC
    mbedtls_aes_context aes;
    mbedtls_aes_init(&aes);
    if (mbedtls_aes_setkey_enc(&aes, _key, 256) != 0) {
        Serial.println("Erro ao definir a chave AES");
        mbedtls_aes_free(&aes);
        delete[] buffer;
        return false;
    }

    uint8_t prevCipher[16];
    memcpy(prevCipher, iv, 16);

    for (size_t i = 0; i < paddedLen; i += 16) {
        // XOR com o bloco anterior (CBC mode)
        for (int j = 0; j < 16; j++) {
            buffer[i + j] ^= prevCipher[j];
        }

        // Criptografar o bloco
        if (mbedtls_aes_crypt_ecb(&aes, MBEDTLS_AES_ENCRYPT, buffer + i, encryptedData + 16 + i) != 0) {
            Serial.println("Erro na criptografia AES");
            mbedtls_aes_free(&aes);
            delete[] buffer;
            return false;
        }

        // Atualizar o bloco anterior
        memcpy(prevCipher, encryptedData + 16 + i, 16);
    }

    mbedtls_aes_free(&aes);

    // Definir o comprimento total (IV + dados criptografados)
    length = 16 + paddedLen;

    delete[] buffer;
    return true;
}

bool Crypto::decrypt(const uint8_t* encryptedData, size_t length, String& data) {
    if (length <= 16) return false; // Deve conter pelo menos o IV

    // Extrair IV
    const uint8_t* iv = encryptedData;
    const uint8_t* cipherData = encryptedData + 16;
    size_t cipherLen = length - 16;

    // Preparar buffer para os dados descriptografados
    uint8_t* buffer = new uint8_t[cipherLen];
    memset(buffer, 0, cipherLen);

    // Descriptografar usando AES-256-CBC
    mbedtls_aes_context aes;
    mbedtls_aes_init(&aes);
    if (mbedtls_aes_setkey_dec(&aes, _key, 256) != 0) {
        Serial.println("Erro ao definir a chave AES para descriptografia");
        mbedtls_aes_free(&aes);
        delete[] buffer;
        return false;
    }

    uint8_t prevCipher[16];
    memcpy(prevCipher, iv, 16);

    for (size_t i = 0; i < cipherLen; i += 16) {
        // Descriptografar o bloco
        uint8_t decryptedBlock[16];
        if (mbedtls_aes_crypt_ecb(&aes, MBEDTLS_AES_DECRYPT, cipherData + i, decryptedBlock) != 0) {
            Serial.println("Erro na descriptografia AES");
            mbedtls_aes_free(&aes);
            delete[] buffer;
            return false;
        }

        // XOR com o bloco anterior (CBC mode)
        for (int j = 0; j < 16; j++) {
            buffer[i + j] = decryptedBlock[j] ^ prevCipher[j];
        }

        // Atualizar o bloco anterior
        memcpy(prevCipher, cipherData + i, 16);
    }

    mbedtls_aes_free(&aes);

    // Remover padding PKCS#7
    uint8_t padding = buffer[cipherLen - 1];
    if (padding == 0 || padding > 16) {
        delete[] buffer;
        return false;
    }

    size_t originalLen = cipherLen - padding;
    buffer[originalLen] = '\0'; // Assegurar terminação da string

    data = String((char*)buffer);
    delete[] buffer;
    return true;
}
