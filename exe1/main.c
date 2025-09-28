#include "pico/stdlib.h"
#include "hardware/gpio.h"
#include <stdio.h>

const int LED_PINS[5] = {2, 3, 4, 5, 6};
const int BTN_PIN = 22;
const int SWITCH_PIN = 28;

volatile int val = 0;
volatile int btn_flag = 0;
volatile int mode = 0; // 0 = incrementar, 1 = decrementar

void bar_init() {
    for (int i = 0; i < 5; i++) {
        gpio_init(LED_PINS[i]);
        gpio_set_dir(LED_PINS[i], GPIO_OUT);
        gpio_put(LED_PINS[i], 0);
    }
}

void bar_display(int val) {
    if (val < 0) val = 0;
    if (val > 5) val = 5;

    for (int i = 0; i < 5; i++) {
        if (i < val)
            gpio_put(LED_PINS[i], 1);
        else
            gpio_put(LED_PINS[i], 0);
    }
}

// ISR do botão verde
static void btn_isr(uint gpio, uint32_t events) {
    if (gpio == BTN_PIN && (events & GPIO_IRQ_EDGE_FALL)) {
        btn_flag = 1;
    }
}

// ISR da chave seletora
static void sw_isr(uint gpio, uint32_t events) {
    if (gpio == SWITCH_PIN) {
        if (events & GPIO_IRQ_EDGE_RISE) {
            mode = 1; // chave foi para nível alto → decrementar
        } else if (events & GPIO_IRQ_EDGE_FALL) {
            mode = 0; // chave foi para nível baixo → incrementar
        }
    }
}

int main() {
    stdio_init_all();

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
    gpio_set_irq_enabled(SWITCH_PIN,
                         GPIO_IRQ_EDGE_RISE | GPIO_IRQ_EDGE_FALL,
                         true);

    while (true) {
        if (btn_flag) {
            btn_flag = 0;

            if (mode == 0)
                val++;
            else
                val--;

            bar_display(val);
        }
    }
}
