Comunicação Serial
Vídeo de ensaio: https://youtube.com/shorts/osiNejsofkg
Este projeto demonstra a integração de diversas tecnologias e periféricos utilizando o Raspberry Pi Pico (ou placa compatível). Nele, você encontrará:

Controle de LEDs WS2812 em uma matriz 5x5 para exibir dígitos (0–9).
Display SSD1306 128x64 para exibir caracteres e mensagens.
Entrada de caracteres via Serial Monitor para exibição no display.
Interrupções com debouncing para tratamento de botões que alternam o estado de LEDs RGB (verde e azul).
Envio de informações via UART para monitoramento no Serial Monitor.
Funcionalidades do Projeto
Modificação da Biblioteca font.h

Adicione caracteres minúsculos à biblioteca font.h. Use sua criatividade para definir os novos caracteres.
Observação: A modificação deve ser realizada diretamente no arquivo font.h para que os caracteres minúsculos sejam exibidos corretamente no display.

Entrada de Caracteres via PC

Utilize o Serial Monitor (por exemplo, via VS Code) para digitar caracteres.
Cada caractere digitado é exibido no display SSD1306.
Se um número entre 0 e 9 for digitado, o símbolo correspondente é exibido na matriz 5x5 WS2812.
Interação com o Botão A

Pressionar o Botão A alterna o estado do LED RGB Verde (ligado/desligado).
A operação é registrada de duas formas:
Uma mensagem informativa é exibida no display SSD1306.
Um texto descritivo é enviado ao Serial Monitor.
Interação com o Botão B

Pressionar o Botão B alterna o estado do LED RGB Azul (ligado/desligado).
A operação é registrada de duas formas:
Uma mensagem informativa é exibida no display SSD1306.
Um texto descritivo é enviado ao Serial Monitor.
Requisitos do Projeto
Uso de Interrupções:
Todas as funcionalidades relacionadas aos botões são implementadas utilizando rotinas de interrupção (IRQ).

Debouncing:
Implementação de tratamento de bouncing dos botões via software (200 ms).

Controle de LEDs:
Demonstração do controle de LEDs comuns (RGB) e LEDs endereçáveis WS2812 (matriz 5x5).

Utilização do Display 128x64:
Exibição de informações utilizando fontes maiúsculas e minúsculas (após modificação da biblioteca font.h).

Envio de Informação pela UART:
Comunicação serial via UART para monitoramento das operações.

Organização e Comentários no Código:
O código está modularizado e comentado para facilitar a compreensão e manutenção.

Hardware Necessário
Microcontrolador: Raspberry Pi Pico (ou placa compatível com o SDK Pico).
Display: SSD1306 128x64 (conectado via I2C).
Matriz de LEDs WS2812: Matriz 5x5 (25 LEDs) controlada via PIO.
LEDs RGB: LEDs comuns para indicação (verde, azul e vermelho).
Botões: Dois botões para alternar os estados dos LEDs.
Protoboard e cabos para realizar as conexões.
Pinagem e Conexões
Display SSD1306 (I2C):

SDA: GPIO 14
SCL: GPIO 15
Endereço I2C: 0x3C
Matriz de LEDs WS2812:

Pino de dados: GPIO 7
LEDs RGB (indicadores):

LED Verde: GPIO 11
LED Azul: GPIO 12
LED Vermelho: GPIO 13 (utilizado também na matriz, conforme necessário)
Botões:

Botão A: GPIO 5
Botão B: GPIO 6
Instalação e Compilação
Pré-requisitos:

Instale o SDK do Raspberry Pi Pico.
Configure o ambiente de desenvolvimento (VS Code, CMake, etc.).
Certifique-se de possuir as bibliotecas necessárias, como pico/stdlib.h, hardware/i2c.h, hardware/pio.h, hardware/gpio.h, hardware/clocks.h e outras.
Configuração do Projeto:

Clone este repositório para sua máquina.
Verifique se os arquivos:
inc/ssd1306.h
inc/font.h (modificado para incluir os caracteres minúsculos)
matriz_led.pio.h
estão na estrutura correta do projeto.
Configure o CMakeLists.txt para incluir todos os arquivos fonte e dependências.
Compilação:

Crie uma pasta de build e configure o projeto:
bash
Copiar
Editar
mkdir build && cd build
cmake ..
make
Grave o firmware no Raspberry Pi Pico utilizando os procedimentos do SDK.
Uso
Conexão:

Monte o hardware conforme a pinagem descrita.
Conecte o Raspberry Pi Pico ao computador via USB.
Execução:

Abra o Serial Monitor (por exemplo, no VS Code) com a taxa de transmissão configurada (geralmente 115200 baud).
Digite um caractere:
Se for um dígito (0 a 9), a matriz WS2812 exibirá o símbolo correspondente.
Outros caracteres serão exibidos no display SSD1306.
Pressione o Botão A para alternar o LED RGB Verde.
Pressione o Botão B para alternar o LED RGB Azul.
As operações serão registradas tanto no display quanto via Serial Monitor.
Considerações Finais
Customização:
Você pode modificar a biblioteca font.h para incluir novos caracteres minúsculos ou alterar a fonte conforme necessário.

Expansão:
Este projeto pode ser expandido para outras aplicações envolvendo comunicação serial, displays gráficos e controle de LEDs endereçáveis.

Contribuições:
Sinta-se à vontade para contribuir com melhorias, correções ou adaptações.

Este projeto serve como base para o aprendizado e aplicação de técnicas de controle de hardware, interrupções, comunicação I2C e UART, e controle de LEDs WS2812 com o Raspberry Pi Pico.

Happy coding!