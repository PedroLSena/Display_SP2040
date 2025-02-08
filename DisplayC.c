#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/i2c.h"
#include "hardware/pio.h"
#include "hardware/clocks.h"
#include "hardware/gpio.h"
#include "inc/ssd1306.h"
#include "inc/font.h"
#include "matriz_led.pio.h"   // Programa PIO para controlar a matriz de LEDs

/*********************** Definições de pinos e constantes ************************/
// Para o display SSD1306 via I2C
#define I2C_PORT    i2c1
#define I2C_SDA     14
#define I2C_SCL     15
#define endereco    0x3C

// Pinos dos LEDs externos (indicadores)
#define led_pin_g   11   // LED Verde
#define led_pin_b   12   // LED Azul
#define led_pin_r   13   // LED Vermelho

// Pinos para a matriz de LEDs
#define LED_COUNT   25   // Matriz 5x5
#define LED_PIN     7    // Pino de dados para a matriz (via PIO)

// Estes valores são usados apenas para composição das cores no array de pixels
#define LED_R       13
#define LED_G       11
#define LED_B       12

// Pinos dos botões (usados para os LEDs externos)
#define BUTTON_A    5
#define BUTTON_B    6

/*********************** Variáveis globais para o display e botões ************************/
char last_char = '\0';  // Armazena o último caractere digitado

bool led_g_estado = false;  // Estado do LED Verde externo
bool led_b_estado = false;  // Estado do LED Azul externo

/*********************** Funções e variáveis para a Matriz de LEDs ************************/

// Estrutura que representa um pixel (ordem GRB)
struct pixel_t {
    uint8_t G, R, B;
};
typedef struct pixel_t pixel_t;
typedef pixel_t npLED_t;

npLED_t leds[LED_COUNT];
PIO np_pio = pio0;
uint sm = 0;

// Padrões dos números (0 a 9) para uma matriz 5x5
const uint8_t numbers[10][25] = {
    // 0
    {1,1,1,1,1,
     1,0,0,0,1,
     1,0,0,0,1,
     1,0,0,0,1,
     1,1,1,1,1},
    // 1
    {0,1,1,1,0,
     0,0,1,0,0,
     0,0,1,0,0,
     0,1,1,0,0,
     0,0,1,0,0},
    // 2
    {1,1,1,1,1,
     1,0,0,0,0,
     1,1,1,1,1,
     0,0,0,0,1,
     1,1,1,1,1},
    // 3
    {1,1,1,1,1,
     0,0,0,0,1,
     1,1,1,1,1,
     0,0,0,0,1,
     1,1,1,1,1},
    // 4
    {1,0,0,0,0,
     0,0,0,0,1,
     1,1,1,1,1,
     1,0,0,0,1,
     1,0,0,0,1},
    // 5
    {1,1,1,1,1,
     0,0,0,0,1,
     1,1,1,1,1,
     1,0,0,0,0,
     1,1,1,1,1},
    // 6
    {1,1,1,1,1,
     1,0,0,0,1,
     1,1,1,1,1,
     0,0,0,0,1,
     1,1,1,1,1},
    // 7
    {0,0,0,1,0,
     0,0,1,0,0,
     0,1,0,0,0,
     0,0,0,0,1,
     1,1,1,1,1},
    // 8
    {1,1,1,1,1,
     1,0,0,0,1,
     1,1,1,1,1,
     1,0,0,0,1,
     1,1,1,1,1},
    // 9
    {1,1,1,1,1,
     0,0,0,0,1,
     1,1,1,1,1,
     1,0,0,0,1,
     1,1,1,1,1}
};

// Define a cor do pixel (para este exemplo, verde com intensidade 50)
#define LED_COLOR_R 0
#define LED_COLOR_G 50
#define LED_COLOR_B 0

// Seta um pixel do array da matriz
void np_set_led(unsigned int index, uint8_t r, uint8_t g, uint8_t b) {
    leds[index].R = r;
    leds[index].G = g;
    leds[index].B = b;
}

// Limpa a matriz (apaga todos os pixels)
void np_clear(void) {
    for (unsigned int i = 0; i < LED_COUNT; i++) {
        np_set_led(i, 0, 0, 0);
    }
}

// Inicializa a matriz de LEDs usando o PIO
void np_init(unsigned int pin) {
    uint offset = pio_add_program(np_pio, &matriz_led_program);
    sm = pio_claim_unused_sm(np_pio, true);
    // Inicializa o programa PIO com a frequência de 800 KHz
    matriz_led_program_init(np_pio, sm, offset, pin, 800000);
    np_clear();
}

// Envia os dados dos pixels para a matriz via PIO (ordem GRB)
void np_write(void) {
    for (unsigned int i = 0; i < LED_COUNT; i++) {
        pio_sm_put_blocking(np_pio, sm, leds[i].G);
        pio_sm_put_blocking(np_pio, sm, leds[i].R);
        pio_sm_put_blocking(np_pio, sm, leds[i].B);
    }
    sleep_us(100);
}

// Atualiza a matriz para exibir o dígito recebido (desenha os pixels "ligados" com a cor definida)
void update_led_matrix(int digit) {
    np_clear();
    for (int i = 0; i < LED_COUNT; i++) {
        if (numbers[digit][i]) {
            np_set_led(i, LED_COLOR_R, LED_COLOR_G, LED_COLOR_B);
        }
    }
    np_write();
}

/*********************** Funções para botões e LEDs externos ************************/

// Callback de interrupção para os botões (usa debounce de ~200ms)
void button_irq(uint gpio, uint32_t events) {
    uint32_t current_time = to_ms_since_boot(get_absolute_time());
    static uint32_t last_press_time = 0;
    if (current_time - last_press_time < 200)
        return;
    last_press_time = current_time;
    
    if (gpio == BUTTON_A) {
        led_g_estado = !led_g_estado;
        gpio_put(led_pin_g, led_g_estado);
        printf("LED Verde %s\n", led_g_estado ? "Ligado" : "Desligado");
    } else if (gpio == BUTTON_B) {
        led_b_estado = !led_b_estado;
        gpio_put(led_pin_b, led_b_estado);
        printf("LED Azul %s\n", led_b_estado ? "Ligado" : "Desligado");
    }
}

// Inicializa os botões com pull-up e configura a IRQ
void init_buttons() {
    gpio_init(BUTTON_A);
    gpio_set_dir(BUTTON_A, GPIO_IN);
    gpio_pull_up(BUTTON_A);
    
    gpio_init(BUTTON_B);
    gpio_set_dir(BUTTON_B, GPIO_IN);
    gpio_pull_up(BUTTON_B);
    
    gpio_set_irq_enabled_with_callback(BUTTON_A, GPIO_IRQ_EDGE_FALL, true, &button_irq);
    gpio_set_irq_enabled_with_callback(BUTTON_B, GPIO_IRQ_EDGE_FALL, true, &button_irq);
}

/*********************** Funções para atualizar o display SSD1306 ************************/

// Exibe os estados dos LEDs externos e o caractere digitado
void display_update(ssd1306_t* ssd) {
    ssd1306_fill(ssd, false);
    
    ssd1306_draw_string(ssd, "LED Verde: ", 8, 10);
    ssd1306_draw_string(ssd, led_g_estado ? "ON" : "OFF", 90, 10);
    
    ssd1306_draw_string(ssd, "LED Azul: ", 8, 50);
    ssd1306_draw_string(ssd, led_b_estado ? "ON" : "OFF", 90, 50);
    
    // Exibe o caractere digitado
    if (last_char != '\0') {
        ssd1306_draw_string(ssd, "Caractere:", 8, 30);
        // Cria uma string com o caractere
        char str[2] = {last_char, '\0'};
        ssd1306_draw_string(ssd, str, 90, 30);
    }
    
    ssd1306_send_data(ssd);
}

/*********************** Função principal ************************/

int main() {
    stdio_init_all();  // Inicializa USB/serial
    
    // Inicializa o I2C (400 KHz) para o display SSD1306
    i2c_init(I2C_PORT, 400 * 1000);
    gpio_set_function(I2C_SDA, GPIO_FUNC_I2C);
    gpio_set_function(I2C_SCL, GPIO_FUNC_I2C);
    gpio_pull_up(I2C_SDA);
    gpio_pull_up(I2C_SCL);
    
    // Inicializa os botões
    init_buttons();
    
    // Configura os pinos dos LEDs externos
    gpio_init(led_pin_g);
    gpio_set_dir(led_pin_g, GPIO_OUT);
    
    gpio_init(led_pin_b);
    gpio_set_dir(led_pin_b, GPIO_OUT);
    
    gpio_init(led_pin_r);
    gpio_set_dir(led_pin_r, GPIO_OUT);
    
    // Inicializa o display SSD1306
    ssd1306_t ssd;
    ssd1306_init(&ssd, WIDTH, HEIGHT, false, endereco, I2C_PORT);
    ssd1306_config(&ssd);
    ssd1306_send_data(&ssd);
    
    ssd1306_fill(&ssd, false);
    ssd1306_send_data(&ssd);
    
    // Inicializa a matriz de LEDs (controlada via PIO no pino LED_PIN)
    np_init(LED_PIN);
    np_clear();
    np_write();
    
    while (true) {
        // Verifica se há caractere digitado via USB
        if (stdio_usb_connected()) {
            if (scanf("%c", &last_char) == 1) {
                printf("Caractere digitado: %c\n", last_char);
                // Se o caractere for um dígito (0 a 9), atualiza a matriz de LEDs
                if (last_char >= '0' && last_char <= '9') {
                    int digit = last_char - '0';
                    update_led_matrix(digit);
                }
            }
        }
        
        // Atualiza o display SSD1306 com os estados dos LEDs e o último caractere
        display_update(&ssd);
        sleep_ms(100);
    }
    
    return 0;
}
