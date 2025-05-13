/**
 * Autor: Elmer Carvalho
 * Acompanhador de Consumo de Água - Webserver Raspberry Pi Pico W
 */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "pico/stdlib.h"
#include "pico/cyw43_arch.h"
#include "lwip/pbuf.h"
#include "lwip/tcp.h"
#include "lwip/netif.h"
#include "hardware/i2c.h"
#include "hardware/pio.h"
#include "hardware/clocks.h"
#include "./lib/ssd1306.h"
#include "pio_matrix.pio.h"


// Credenciais Wi-Fi
#define WIFI_SSID "NOME DA SUA REDE"
#define WIFI_PASSWORD "SENHA DA SUA REDE"

// Configurações do Acompanhador
#define TIME_LIMIT_MS 5000 // 5 segundos para testes
#define EMOJI_HAPPY "😊"
#define EMOJI_SAD "😞"
#define MESSAGE_HAPPY "Muito bem!"
#define MESSAGE_SAD "Deixe de preguiça! Vá se hidratar!"

#define LED_GREEN 11
#define LED_RED 13

// Comunicação Serial I2C
#define I2C_PORT i2c1
#define I2C_SDA_PIN 14
#define I2C_SCL_PIN 15

// Definições do Display
#define SSD_ADDR 0x3C
#define SSD_WIDTH 128
#define SSD_HEIGHT 64

// Matriz de LEDs
#define MATRIZ_LEDS_PIN 7
#define NUM_LEDS 25

ssd1306_t ssd;
PIO pio = pio0;
uint sm;

// HTML para estado Feliz
static const char *STATE_1_HTML =
    "<!DOCTYPE html>\n"
    "<html>\n"
    "<head>\n"
    "    <meta charset=\"UTF-8\">\n"
    "    <title>Acompanhador de Hidratação - Feliz</title>\n"
    "    <style>\n"
    "        body { font-family: Arial; text-align: center; background-color: #e6f3e6; }\n"
    "        h1 { font-size: 36px; margin-top: 20px; }\n"
    "        button { font-size: 24px; padding: 15px 30px; margin: 10px; background-color: #4CAF50; color: white; border: none; border-radius: 5px; cursor: pointer;}\n"
    "        .status { font-size: 48px; margin: 20px; }\n"
    "        .message { font-size: 24px; color: #333; }\n"
    "    </style>\n"
    "    <meta http-equiv=\"refresh\" content=\"10\">\n"
    "</head>\n"
    "<body>\n"
    "    <h1>Acompanhador de Hidratação</h1>\n"
    "    <form action=\"./drink\"><button>Bebi Água!</button></form>\n"
    "    <p class=\"status\">" EMOJI_HAPPY "</p>\n"
    "    <p class=\"message\">" MESSAGE_HAPPY "</p>\n"
    "</body>\n"
    "</html>\n";

// HTML para estado Triste
static const char *STATE_2_HTML =
    "<!DOCTYPE html>\n"
    "<html>\n"
    "<head>\n"
    "    <meta charset=\"UTF-8\">\n"
    "    <title>Acompanhador de Hidratação - Triste</title>\n"
    "    <style>\n"
    "        body { font-family: Arial; text-align: center; background-color: #f3e6e6; }\n"
    "        h1 { font-size: 36px; margin-top: 20px; }\n"
    "        button { font-size: 24px; padding: 15px 30px; margin: 10px; background-color: #4CAF50; color: white; border: none; border-radius: 5px; cursor: pointer;}\n"
    "        .status { font-size: 48px; margin: 20px; }\n"
    "        .message { font-size: 24px; color: #333; }\n"
    "    </style>\n"
    "    <meta http-equiv=\"refresh\" content=\"10\">\n"
    "</head>\n"
    "<body>\n"
    "    <h1>Acompanhador de Hidratação</h1>\n"
    "    <form action=\"./drink\"><button>Bebi Água!</button></form>\n"
    "    <p class=\"status\">" EMOJI_SAD "</p>\n"
    "    <p class=\"message\">" MESSAGE_SAD "</p>\n"
    "</body>\n"
    "</html>\n";

// Variáveis de estado
static bool is_happy = true;
static absolute_time_t last_drink;

// Prototipagem
static err_t tcp_server_accept(void *arg, struct tcp_pcb *newpcb, err_t err);
static err_t tcp_server_recv(void *arg, struct tcp_pcb *tpcb, struct pbuf *p, err_t err);
static err_t tcp_server_sent(void *arg, struct tcp_pcb *tpcb, u16_t len);
void clean_peripherals();

int main() {
    stdio_init_all();
    gpio_init(LED_GREEN);
    gpio_init(LED_RED);
    gpio_set_dir(LED_GREEN, GPIO_OUT);
    gpio_set_dir(LED_RED, GPIO_OUT);
    gpio_put(LED_GREEN, is_happy);

    clean_peripherals();


    while (cyw43_arch_init()) {
        printf("Falha ao inicializar Wi-Fi\n");
        sleep_ms(100);
        return -1;
    }

    cyw43_arch_enable_sta_mode();
    printf("Conectando ao Wi-Fi...\n");

    while (cyw43_arch_wifi_connect_timeout_ms(WIFI_SSID, WIFI_PASSWORD, CYW43_AUTH_WPA2_AES_PSK, 20000)) {
        printf("Falha ao conectar ao Wi-Fi\n");
        sleep_ms(100);
        return -1;
    }

    printf("Conectado ao Wi-Fi\n");
    if (netif_default) {
        printf("IP do dispositivo: %s\n", ipaddr_ntoa(&netif_default->ip_addr));
    }

    struct tcp_pcb *server = tcp_new();
    if (!server) {
        printf("Falha ao criar servidor TCP\n");
        return -1;
    }

    if (tcp_bind(server, IP_ADDR_ANY, 80) != ERR_OK) {
        printf("Falha ao associar servidor TCP à porta 80\n");
        return -1;
    }

    server = tcp_listen(server);
    tcp_accept(server, tcp_server_accept);
    printf("Servidor ouvindo na porta 80\n");

    last_drink = get_absolute_time();
    static bool last_state = true;

    while (true) {
        if (absolute_time_diff_us(last_drink, get_absolute_time()) / 1000 > TIME_LIMIT_MS) {
            is_happy = false;
        }

        if (is_happy != last_state) {
            printf("\n\nEstado mudou para %s\n\n", is_happy ? "Feliz" : "Triste");
            last_state = is_happy;
            gpio_put(LED_GREEN, is_happy);
            gpio_put(LED_RED, !is_happy);
        }

        cyw43_arch_poll();
        sleep_ms(100);
    }

    cyw43_arch_deinit();
    return 0;
}

static err_t tcp_server_accept(void *arg, struct tcp_pcb *newpcb, err_t err) {
    tcp_recv(newpcb, tcp_server_recv);
    return ERR_OK;
}

static err_t tcp_server_recv(void *arg, struct tcp_pcb *tpcb, struct pbuf *p, err_t err) {
    if (!p) {
        tcp_close(tpcb);
        tcp_recv(tpcb, NULL);
        return ERR_OK;
    }

    char *request = (char *)malloc(p->len + 1);
    if (!request) {
        pbuf_free(p);
        return ERR_MEM;
    }

    memcpy(request, p->payload, p->len);
    request[p->len] = '\0';

    printf("Request: %s\n", request);

    tcp_recved(tpcb, p->tot_len);
    pbuf_free(p);

    char *full_response = (char *)malloc(2048);
    if (!full_response) {
        free(request);
        return ERR_MEM;
    }

    size_t total_len = 0;

    if (strstr(request, "GET /drink") != NULL) {
        is_happy = true;
        last_drink = get_absolute_time();
        printf("Botão clicado: Estado Feliz\n");

        total_len = snprintf(full_response, 2048,
            "HTTP/1.1 302 Found\r\n"
            "Location: /\r\n"
            "Content-Length: 0\r\n"
            "Connection: close\r\n"
            "\r\n");

    } else if (strstr(request, "GET /favicon.ico") != NULL) {
        total_len = snprintf(full_response, 2048,
            "HTTP/1.1 204 No Content\r\n"
            "Connection: close\r\n"
            "\r\n");

    } else if (strstr(request, "GET /") != NULL) {
        const char *html_body = is_happy ? STATE_1_HTML : STATE_2_HTML;
        size_t body_len = strlen(html_body);

        total_len = snprintf(full_response, 2048,
            "HTTP/1.1 200 OK\r\n"
            "Content-Type: text/html; charset=UTF-8\r\n"
            "Content-Length: %zu\r\n"
            "Cache-Control: no-cache\r\n"
            "Connection: close\r\n"
            "\r\n"
            "%s", body_len, html_body);

        printf("Enviando HTML: %s (corpo: %zu bytes, total: %zu bytes)\n",
               is_happy ? "Feliz" : "Triste", body_len, total_len);

    } else {
        total_len = snprintf(full_response, 2048,
            "HTTP/1.1 404 Not Found\r\n"
            "Content-Length: 0\r\n"
            "Connection: close\r\n"
            "\r\n");
    }

    free(request);

    err_t wr_err = tcp_write(tpcb, full_response, total_len, TCP_WRITE_FLAG_COPY);
    if (wr_err == ERR_OK) {
        tcp_sent(tpcb, tcp_server_sent);
        tcp_output(tpcb);
    } else {
        tcp_close(tpcb);
    }

    free(full_response);
    return wr_err;
}

static err_t tcp_server_sent(void *arg, struct tcp_pcb *tpcb, u16_t len) {
    return tcp_close(tpcb);
}

void clean_peripherals() {
    // --- Inicializar matriz de LEDs ---
    sm = pio_claim_unused_sm(pio, true);
    uint offset = pio_add_program(pio, &pio_matrix_program);
    pio_matrix_program_init(pio, sm, offset, MATRIZ_LEDS_PIN);
    pio_sm_set_enabled(pio, sm, true);

    // Limpar matriz de LEDs (enviando 0 para cada LED)
    for (uint i = 0; i < NUM_LEDS; i++) {
        pio_sm_put_blocking(pio, sm, 0);
    }

    // --- Inicializar I2C para o display OLED ---
    i2c_init(I2C_PORT, 400000);
    gpio_set_function(I2C_SDA_PIN, GPIO_FUNC_I2C);
    gpio_set_function(I2C_SCL_PIN, GPIO_FUNC_I2C);
    gpio_pull_up(I2C_SDA_PIN);
    gpio_pull_up(I2C_SCL_PIN);

    // Inicializar e configurar o display SSD1306
    ssd1306_init(&ssd, SSD_WIDTH, SSD_HEIGHT, false, SSD_ADDR, I2C_PORT);
    ssd1306_config(&ssd);

    // Limpar display OLED
    ssd1306_fill(&ssd, false);
    ssd1306_send_data(&ssd);
}
