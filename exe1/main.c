#include "pico/stdlib.h"
#include "hardware/gpio.h"
#include <stdio.h>

const int LED_PINS[5] = {2, 3, 4, 5, 6};
const int BTN_PIN = 22;
const int SWITCH_PIN = 28;

volatile int val = 0;
volatile int btn_flag = 0;

// Inicializa barra de LEDs
void bar_init() {
    for (int i = 0; i < 5; i++) {
        gpio_init(LED_PINS[i]);
        gpio_set_dir(LED_PINS[i], GPIO_OUT);
        gpio_put(LED_PINS[i], 0);
    }
}

// Mostra valor (0..5) na barra
void bar_display(int val) {
    if (val < 0) val = 0;
    if (val > 5) val = 5;

    for (int i = 0; i < 5; i++) {
        gpio_put(LED_PINS[i], i < val ? 1 : 0);
    }
}

// ISR do botão
static void btn_isr(uint gpio, uint32_t events) {
    if (gpio == BTN_PIN && (events & GPIO_IRQ_EDGE_FALL)) {
        btn_flag = 1;
    }
}

int main() {
    stdio_init_all();

    // Inicializa barra
    bar_init();

    // Configura botão
    gpio_init(BTN_PIN);
    gpio_set_dir(BTN_PIN, GPIO_IN);
    gpio_pull_up(BTN_PIN);
    gpio_set_irq_enabled_with_callback(BTN_PIN, GPIO_IRQ_EDGE_FALL, true, &btn_isr);

    // Configura chave seletora
    gpio_init(SWITCH_PIN);
    gpio_set_dir(SWITCH_PIN, GPIO_IN);
    gpio_pull_up(SWITCH_PIN);

    while (true) {
        if (btn_flag) {
            btn_flag = 0;

            // Lê posição da chave no momento do clique
            int mode = gpio_get(SWITCH_PIN);

            if (mode == 0) val++;  // chave baixa → incrementar
            else val--;            // chave alta  → decrementar

            bar_display(val);
        }
    }
}
