Hardware Wallet com ESP32-C3 e SSD1306


Índice
Visão Geral
Características
Requisitos de Hardware
Requisitos de Software
Instalação
Configuração
Estrutura do Projeto
Uso
Depuração
Resolução de Problemas
Contribuição
Licença
Contato
Visão Geral
Este projeto implementa uma Hardware Wallet utilizando um microcontrolador ESP32-C3 e um display SSD1306. A wallet permite a geração, armazenamento seguro e restauração de seeds (mnemonics) seguindo o padrão BIP39. Além disso, utiliza botões físicos para interação com o usuário.

Características
Geração de Seed: Cria uma seed de 128 bits e a converte em uma sequência de palavras mnemonics.
Restauração de Seed: Permite ao usuário inserir uma seed existente para restaurar a wallet.
Criptografia: Encripta a seed antes de armazená-la na memória não volátil (NVS) do ESP32.
Interface de Usuário: Navegação por menus utilizando botões físicos e exibição de informações no display SSD1306.
Armazenamento Seguro: Utiliza a biblioteca Preferences para armazenamento seguro de dados.
Requisitos de Hardware
Microcontrolador: ESP32-C3-DevKitM-1
Display: SSD1306 (128x64 pixels) conectado via I2C
Botões: Dois botões físicos para navegação (Próximo e Selecionar)
Conexões:
SSD1306:
VCC: 3.3V
GND: GND
SDA: GPIO21 (pino padrão do ESP32-C3)
SCL: GPIO22 (pino padrão do ESP32-C3)
Botões:
Botão 1 (Próximo): GPIO1
Botão 2 (Selecionar): GPIO2
Requisitos de Software
Plataforma de Desenvolvimento: Visual Studio Code com extensão PlatformIO
Framework: Arduino para ESP32
Bibliotecas:
Adafruit SSD1306
Adafruit GFX Library
Bounce2
Preferences
Wire
Instalação
Clone o Repositório:

bash
Copiar código
git clone https://github.com/seu-usuario/seu-repositorio.git
cd seu-repositorio
Abra o Projeto no Visual Studio Code:

Inicie o Visual Studio Code.
Vá para File > Open Folder e selecione o diretório do projeto clonado.
Instale as Extensões Necessárias:

Certifique-se de ter a extensão PlatformIO instalada no VS Code.
Instale as Bibliotecas Necessárias:

O platformio.ini está configurado para instalar automaticamente as bibliotecas necessárias. Para garantir, execute:

bash
Copiar código
pio lib install
Configuração
Verifique as Conexões de Hardware:

Assegure-se de que o display SSD1306 está corretamente conectado ao ESP32-C3 via I2C nos pinos SDA e SCL.
Conecte os botões físicos aos pinos GPIO1 e GPIO2.
Configuração do Projeto:

O arquivo platformio.ini já está configurado para o ambiente esp32c3_ssd1306. Se necessário, ajuste as configurações conforme seu hardware.

Estrutura do Projeto
css
Copiar código
seu-repositorio/
```
├── lib/
│   ├── Adafruit_SSD1306/
│   ├── Adafruit_GFX_Library/
│   ├── Bounce2/
│   ├── Preferences/
│   └── Wire/
├── src/
│   ├── buttons/
│   │   ├── buttons.h
│   │   └── buttons.cpp
│   ├── communication/
│   │   ├── communication.h
│   │   └── communication.cpp
│   ├── crypto/
│   │   ├── crypto.h
│   │   └── crypto.cpp
│   ├── display/
│   │   ├── display.h
│   │   └── display.cpp
│   ├── seed/
│   │   ├── seed.h
│   │   ├── seed.cpp
│   │   └── wordlist.h
│   └── main.cpp
├── platformio.ini
└── README.md
```
Uso
Compilar e Carregar o Firmware:

No VS Code com PlatformIO:
Abra a paleta de comandos (Ctrl+Shift+P).
Selecione PlatformIO: Build para compilar o projeto.
Conecte seu ESP32-C3 via USB.
Execute PlatformIO: Upload para carregar o firmware no dispositivo.
Monitorar a Saída Serial:

Abra a paleta de comandos (Ctrl+Shift+P).
Selecione PlatformIO: Monitor para visualizar mensagens de depuração e verificar o funcionamento do projeto.
Interagir com a Wallet:

Utilize os botões físicos para navegar nos menus:
Botão "Próximo": Navega pelas opções do menu.
Botão "Selecionar": Confirma a seleção atual.
Funcionalidades Disponíveis:

Gerar Seed:
Cria uma nova seed, encripta e armazena na memória.
Restaurar Seed:
Permite ao usuário inserir uma seed existente para restaurar a wallet.
Configurações:
Atualmente, esta funcionalidade não está implementada.
Depuração
Mensagens de Erro:

Utilize o Serial Monitor para visualizar mensagens de erro e status.
Verifique as Conexões:

Certifique-se de que todos os componentes estão conectados corretamente conforme o diagrama de pinagem.
Recompile Após Alterações:

Sempre recompile o projeto após fazer alterações no código ou nas configurações para garantir que não há erros de compilação.
Resolução de Problemas
Erro de Compilação: fatal error: Adafruit_GFX.h: No such file or directory:

Causa: A biblioteca Adafruit GFX não está instalada.
Solução: Assegure-se de que a biblioteca está incluída no platformio.ini e execute PlatformIO: Build novamente para instalar as dependências.
Display Não Inicializa:

Causa: Conexões de I2C incorretas ou endereço I2C errado.
Solução: Verifique as conexões de SDA e SCL e confirme o endereço I2C do display (geralmente 0x3C).
Botões Não Respondem:

Causa: Conexões de botões incorretas ou pinos definidos errados.
Solução: Verifique as conexões dos botões aos pinos GPIO1 e GPIO2 e confirme no código se os pinos estão corretos.
Contribuição
Contribuições são bem-vindas! Sinta-se à vontade para abrir issues ou enviar pull requests para melhorar este projeto.

Licença
Este projeto está licenciado sob a MIT License.

Contato
Autor: Sandmann
Email: sandmann21s@proton.me
GitHub: Sandmann21s
