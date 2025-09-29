#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/gpio.h"
#include "hardware/timer.h"

const int LED_PIN_B = 9;
const int LED_PIN_Y = 5;
const int BTN_PIN   = 28;

struct repeating_timer timerY;
struct repeating_timer timerB;
volatile alarm_id_t alarm_id = 0;

volatile int ledstateB = 0;
volatile int ledstateY = 0;
volatile int timer_runningB = 0;
volatile int timer_runningY = 0;

bool timer_callbackB(struct repeating_timer *t) {
    ledstateB = !ledstateB;
    gpio_put(LED_PIN_B, ledstateB);
    return true;
}

bool timer_callbackY(struct repeating_timer *t) {
    ledstateY = !ledstateY;
    gpio_put(LED_PIN_Y, ledstateY);
    return true;
}

// Alarme: após 5 segundos, parar os LEDs
static int64_t alarm_cb_r(alarm_id_t id, void *user_data) {
    cancel_repeating_timer(&timerB);
    cancel_repeating_timer(&timerY);

    ledstateY = 0;
    gpio_put(LED_PIN_Y, 0);
    ledstateB = 0;
    gpio_put(LED_PIN_B, 0);

    timer_runningY = 0;
    timer_runningB = 0;
    alarm_id = 0; // libera para próxima vez
    return 0; // one-shot
}

void btn_callback(uint gpio, uint32_t events) {
    if ((gpio == BTN_PIN) && (events & GPIO_IRQ_EDGE_FALL)) {
        if (!timer_runningB) {
            add_repeating_timer_ms(150, timer_callbackB, NULL, &timerB);
            timer_runningB = 1;
        }
        if (!timer_runningY) {
            add_repeating_timer_ms(500, timer_callbackY, NULL, &timerY);
            timer_runningY = 1;
        }
        if (alarm_id == 0) {
            alarm_id = add_alarm_in_ms(5000, alarm_cb_r, NULL, false);
        }
    }
}

int main() {
    stdio_init_all();

    gpio_init(LED_PIN_Y);
    gpio_set_dir(LED_PIN_Y, GPIO_OUT);
    gpio_put(LED_PIN_Y, 0);

    gpio_init(LED_PIN_B);
    gpio_set_dir(LED_PIN_B, GPIO_OUT);
    gpio_put(LED_PIN_B, 0);

    gpio_init(BTN_PIN);
    gpio_set_dir(BTN_PIN, GPIO_IN);
    gpio_pull_up(BTN_PIN);

    gpio_set_irq_enabled_with_callback(BTN_PIN, GPIO_IRQ_EDGE_FALL, true, &btn_callback);

    while (true) {
    }
}
