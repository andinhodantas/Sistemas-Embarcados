#include <stdio.h> // Biblioteca padrão de entrada e saída
#include <string.h>
#include "hardware/adc.h" // Biblioteca para manipulação do ADC no RP2040
#include "hardware/pwm.h" // Biblioteca para controle de PWM no RP2040
#include "hardware/clocks.h"
#include "pico/stdlib.h" // Biblioteca padrão do Raspberry Pi Pico
#include "inc/ssd1306.h"

#define LED_G_PIN 11
#define LED_R_PIN 13
#define BUTTON_A 5
#define BUTTON_B 6

const uint I2C_SDA = 14;
const uint I2C_SCL = 15;
bool within_the_option = false;
int VRX;
int VRY;
const int ADC_CHANNEL_0 = 1; // Canal ADC para o eixo X
const int ADC_CHANNEL_1 = 0;

char *text[] = { // Mensagens a serem exibidas
    "AND",
    "OR",
    "NOT",
    "NAND",
    "NOR",
    "XOR",
    "XNOR"};

// Preparar área de renderização para o display (ssd1306_width pixels por ssd1306_n_pages páginas)
uint8_t ssd[ssd1306_buffer_length];
struct render_area frame_area = {
    start_column : 0,
    end_column : ssd1306_width - 1,
    start_page : 0,
    end_page : ssd1306_n_pages - 1
};

int i = 0;
// Função para mostrar as opções no display
void display_menu(int i)
{
    memset(ssd, 0, ssd1306_buffer_length);
    int y = 25; // Linha inicial

    // Linha inicial
    ssd1306_draw_string_scaled(ssd, 15, y, text[i], 3); // Mostra o texto com tamanho 3x

    y += 8;
    render_on_display(ssd, &frame_area);
}

// Função de inicialização dos botões, LEDs, OLED e joystick
void initializations()
{
    stdio_init_all();
    i2c_init(i2c1, ssd1306_i2c_clock * 1000);
    gpio_set_function(I2C_SDA, GPIO_FUNC_I2C);
    gpio_set_function(I2C_SCL, GPIO_FUNC_I2C);
    gpio_pull_up(I2C_SDA);
    gpio_pull_up(I2C_SCL);
    gpio_init(LED_R_PIN);
    gpio_init(LED_G_PIN);
    gpio_set_dir(LED_R_PIN, GPIO_OUT);
    gpio_set_dir(LED_G_PIN, GPIO_OUT);
    // Configuração do GPIO do Botão A e B como entrada com pull-up interno
    gpio_init(BUTTON_A);
    gpio_set_dir(BUTTON_A, GPIO_IN);
    gpio_pull_up(BUTTON_A);
    gpio_init(BUTTON_B);
    gpio_set_dir(BUTTON_B, GPIO_IN);
    gpio_pull_up(BUTTON_B);
}

// Função do movimento do joystick
void select_option()
{
    uint adc_y_raw = adc_read();

    // Movimenta para cima
    if (adc_y_raw < 100)
    {

        if (i >= 1)
        {
            i--;
            sleep_ms(100);
            display_menu(i);
            sleep_ms(50);
            printf("%d", i);
        }
        else
        {
            display_menu(i);
            sleep_ms(50);
        }
    }
    // Movimenta para baixo
    else if (adc_y_raw > 3000)
    {

        if (i <= 5)
        {
            i++;
            display_menu(i);
            sleep_ms(50);
        }
        else
        {
            display_menu(i);
            sleep_ms(50);
        }
    }
}

int main()
{

    initializations();

    // Inicializa o ADC e os pinos de entrada analógica
    adc_init();         // Inicializa o módulo ADC
    adc_gpio_init(VRX); // Configura o pino VRX (eixo X) para entrada ADC
    adc_gpio_init(VRY); // Configura o pino VRY (eixo Y) para entrada ADC

    ssd1306_init();

    calculate_render_area_buffer_length(&frame_area);
    uint16_t vrx_value, vry_value, sw_value; // Variáveis para armazenar os valores do joystick (eixos X e Y) e botão

    display_menu(i);

    while (1)
    {
        adc_select_input(1); // Seleciona entrada ADC1 (eixo Y)
        adc_select_input(0);

        select_option();

        // Atualiza o estado dos botões como nivel lógico alto (1)
        bool button_A_pressed = gpio_get(BUTTON_A);
        bool button_B_pressed = gpio_get(BUTTON_B);

        // Verifica se o menu está na opção "AND"
        if (strcmp(text[i], "AND") == 0)
        {
            if (button_A_pressed && button_B_pressed)
            {
                gpio_put(LED_R_PIN, 0);
                gpio_put(LED_G_PIN, 1);
            }
            else
            {
                gpio_put(LED_R_PIN, 1);
                gpio_put(LED_G_PIN, 0);
            }
        }

        // Verifica se o menu está na opção "OR"
        else if (strcmp(text[i], "OR") == 0)
        {
            if (!button_A_pressed && !button_B_pressed)
            {
                gpio_put(LED_R_PIN, 1);
                gpio_put(LED_G_PIN, 0);
            }
            else
            {
                gpio_put(LED_R_PIN, 0);
                gpio_put(LED_G_PIN, 1);
            }
        }

        // Verifica se o menu está na opção "NOT"
        else if (strcmp(text[i], "NOT") == 0)
        {
            if (button_A_pressed)
            {
                gpio_put(LED_R_PIN, 1);
                gpio_put(LED_G_PIN, 0);
            }
            else
            {
                gpio_put(LED_R_PIN, 0);
                gpio_put(LED_G_PIN, 1);
            }
        }

        // Verifica se o menu está na opção "NAND"
        else if (strcmp(text[i], "NAND") == 0)
        {
            if (button_A_pressed && button_B_pressed)
            {
                gpio_put(LED_R_PIN, 1);
                gpio_put(LED_G_PIN, 0);
            }
            else
            {
                gpio_put(LED_R_PIN, 0);
                gpio_put(LED_G_PIN, 1);
            }
        }
        // Verifica se o menu está na opção "NOR"
        else if (strcmp(text[i], "NOR") == 0)
        {
            if (!button_A_pressed && !button_B_pressed)
            {
                gpio_put(LED_R_PIN, 0);
                gpio_put(LED_G_PIN, 1);
            }
            else
            {
                gpio_put(LED_R_PIN, 1);
                gpio_put(LED_G_PIN, 0);
            }
        }
        // Verifica se o menu está na opção "XOR"
        else if (strcmp(text[i], "XOR") == 0)
        {
            if (button_A_pressed == button_B_pressed)
            {
                gpio_put(LED_R_PIN, 1);
                gpio_put(LED_G_PIN, 0);
            }
            else
            {
                gpio_put(LED_R_PIN, 0);
                gpio_put(LED_G_PIN, 1);
            }
        }

        // Verifica se o menu está na opção "XNOR"
        else if (strcmp(text[i], "XNOR") == 0)
        {
            if (button_A_pressed == button_B_pressed)
            {
                gpio_put(LED_R_PIN, 0);
                gpio_put(LED_G_PIN, 1);
            }
            else
            {
                gpio_put(LED_R_PIN, 1);
                gpio_put(LED_G_PIN, 0);
            }
        }

        sleep_ms(50); // Pequeno atraso para suavizar leituras
    }
}
