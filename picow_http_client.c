#include <stdio.h>
#include "pico/stdlib.h"
#include "pico/cyw43_arch.h"
#include "example_http_client_util.h"

#define WIFI_SSID "nome da sua rede" //subistitua pelo nome da sua rede
#define WIFI_PASSWORD "sua senha" //substitua pela senha 

#define HOST "192.168.xx.xx" // ← Altere aqui para o IP real do seu PC!
#define PORT 5000

#define BUTTON_A 5
#define BUTTON_B 6
#define LED_BLUE 12
#define LED_RED 13

int main() {
    stdio_init_all();

    gpio_init(BUTTON_A); gpio_set_dir(BUTTON_A, GPIO_IN); gpio_pull_up(BUTTON_A);
    gpio_init(BUTTON_B); gpio_set_dir(BUTTON_B, GPIO_IN); gpio_pull_up(BUTTON_B);
    gpio_init(LED_BLUE); gpio_set_dir(LED_BLUE, GPIO_OUT); gpio_put(LED_BLUE, 0);
    gpio_init(LED_RED); gpio_set_dir(LED_RED, GPIO_OUT); gpio_put(LED_RED, 0);

    if (cyw43_arch_init()) {
        printf("Erro ao inicializar Wi-Fi\n");
        return 1;
    }
    cyw43_arch_enable_sta_mode();

    if (cyw43_arch_wifi_connect_timeout_ms(WIFI_SSID, WIFI_PASSWORD, CYW43_AUTH_WPA2_AES_PSK, 30000)) {
        printf("Erro na conexão Wi-Fi\n");
        return 1;
    }

    printf("Conectado ao Wi-Fi. IP: %s\n", ip4addr_ntoa(netif_ip4_addr(netif_list)));

    bool button_a_pressed = false;
    bool button_b_pressed = false;

    while (true) {
        const char* path = NULL;

        bool current_a = !gpio_get(BUTTON_A);
        bool current_b = !gpio_get(BUTTON_B);

        if (current_a && !button_a_pressed) {
            path = "/CLICK";
            gpio_put(LED_BLUE, 1);
            button_a_pressed = true;
        } else if (!current_a && button_a_pressed) {
            path = "/SOLTO";
            gpio_put(LED_BLUE, 0);
            button_a_pressed = false;
        }

        if (current_b && !button_b_pressed) {
            path = "/RED_ON";
            gpio_put(LED_RED, 1);
            button_b_pressed = true;
        } else if (!current_b && button_b_pressed) {
            path = "/RED_OFF";
            gpio_put(LED_RED, 0);
            button_b_pressed = false;
        }

        if (path != NULL) {
            printf("Enviando comando para %s\n", path);

            EXAMPLE_HTTP_REQUEST_T req = {0};
            req.hostname = HOST;
            req.url = path;
            req.port = PORT;
            req.headers_fn = http_client_header_print_fn;
            req.recv_fn = http_client_receive_print_fn;

            int result = http_client_request_sync(cyw43_arch_async_context(), &req);
            if (result != 0) {
                printf("Erro ao enviar comando: %d\n", result);
            } else {
                printf("Comando enviado com sucesso!\n");
            }
            sleep_ms(100);
        }

        sleep_ms(1000);
    }
}
