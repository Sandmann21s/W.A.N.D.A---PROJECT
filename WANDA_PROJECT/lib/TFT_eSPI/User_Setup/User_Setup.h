// User_Setup.h for TTGO T-Display

#pragma once

// Defina o driver do display
#define ILI9341_DRIVER // Driver para o ILI9341

// Definição dos pinos conforme a pinagem do TTGO T-Display
#define TFT_CS   5    // Chip Select
#define TFT_DC   16   // Data/Command
#define TFT_RST  17   // Reset (pode ser conectado ao RST do ESP32-C3)

// Definição de pinos SPI
#define TFT_MOSI 23   // MOSI
#define TFT_SCLK 18   // SCLK
#define TFT_MISO 19   // MISO (não utilizado, mas pode ser definido)

// Definição de pinos para Touch (se aplicável)
#define TOUCH_CS 4    // Chip Select do Touch (se aplicável)

// Define a rotação do display
#define TFT_ROTATION  3

// Outras configurações podem ser adicionadas conforme necessário

// Remova comentários das seguintes linhas se estiver usando touch
// #define TOUCH_ADA
// #define TOUCH_POINT_CALIBRATION
