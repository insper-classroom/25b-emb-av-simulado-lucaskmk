#include "pico/stdlib.h"
#include "hardware/gpio.h"
#include <stdio.h>

const int LED_PIN1 = 2;
const int LED_PIN2 = 3;
const int LED_PIN3 = 4;
const int LED_PIN4 = 5;
const int LED_PIN5 = 6;
const int BTN_PIN  = 22;
const int SWITCH_PIN = 28;

volatile int btn_flag = 0;   // única global, usada pelo botão

// inicializa a barra de LEDs
void bar_init() {
    gpio_init(LED_PIN1); gpio_set_dir(LED_PIN1, GPIO_OUT); gpio_put(LED_PIN1, 0);
    gpio_init(LED_PIN2); gpio_set_dir(LED_PIN2, GPIO_OUT); gpio_put(LED_PIN2, 0);
    gpio_init(LED_PIN3); gpio_set_dir(LED_PIN3, GPIO_OUT); gpio_put(LED_PIN3, 0);
    gpio_init(LED_PIN4); gpio_set_dir(LED_PIN4, GPIO_OUT); gpio_put(LED_PIN4, 0);
    gpio_init(LED_PIN5); gpio_set_dir(LED_PIN5, GPIO_OUT); gpio_put(LED_PIN5, 0);
}

// exibe valor na barra (sem usar for, apenas o necessário)
void bar_display(int val) {
    if (val < 0) val = 0;
    if (val > 5) val = 5;

    gpio_put(LED_PIN1, val >= 1 ? 1 : 0);
    gpio_put(LED_PIN2, val >= 2 ? 1 : 0);
    gpio_put(LED_PIN3, val >= 3 ? 1 : 0);
    gpio_put(LED_PIN4, val >= 4 ? 1 : 0);
    gpio_put(LED_PIN5, val >= 5 ? 1 : 0);
}

// ISR do botão verde
static void btn_isr(uint gpio, uint32_t events) {
    if (gpio == BTN_PIN && (events & GPIO_IRQ_EDGE_FALL)) {
        btn_flag = 1;
    }
}

// ISR da chave (atualiza o ponteiro mode sem gpio_get)
static void sw_isr(uint gpio, uint32_t events) {
    int *mode_ptr = (int *)gpio_get_irq_user_data(gpio);
    if (events & GPIO_IRQ_EDGE_RISE) {
        *mode_ptr = 1; // chave em nível alto → decrementar
    } else if (events & GPIO_IRQ_EDGE_FALL) {
        *mode_ptr = 0; // chave em nível baixo → incrementar
    }
}

int main() {
    stdio_init_all();

    bar_init();

    // configura botão
    gpio_init(BTN_PIN);
    gpio_set_dir(BTN_PIN, GPIO_IN);
    gpio_pull_up(BTN_PIN);
    gpio_set_irq_enabled_with_callback(BTN_PIN, GPIO_IRQ_EDGE_FALL, true, &btn_isr);

    // configura chave seletora
    gpio_init(SWITCH_PIN);
    gpio_set_dir(SWITCH_PIN, GPIO_IN);
    gpio_pull_up(SWITCH_PIN);

    int val = 0;   // contador da barra
    int mode = 0;  // 0 = incrementar, 1 = decrementar

    gpio_add_raw_irq_handler_with_user_data(SWITCH_PIN, sw_isr, &mode);
    gpio_set_irq_enabled(SWITCH_PIN, GPIO_IRQ_EDGE_RISE | GPIO_IRQ_EDGE_FALL, true);

    while (true) {
        if (btn_flag) {
            btn_flag = 0;

            if (mode == 0) val++;
            else val--;

            bar_display(val);
        }
    }
}
