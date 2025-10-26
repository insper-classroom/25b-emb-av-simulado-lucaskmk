#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/gpio.h"
#include "hardware/timer.h"

const int LED_PIN_B = 9;
const int LED_PIN_Y = 5;
const int BTN_PIN   = 28;

struct repeating_timer timer_b; volatile bool ledb_state = false;
struct repeating_timer timer_y; volatile bool ledy_state = false;

volatile alarm_id_t alarm_cb_ = 0; volatile bool is_blinking = false;

bool timer_callback_b(struct repeating_timer *t) {
    ledb_state = !ledb_state;          // inverte estado
    gpio_put(LED_PIN_B, ledb_state);    // atualiza LED
    return true; // mantém o timer rodando
}
bool timer_callback_y(struct repeating_timer *t) {
    ledy_state = !ledy_state;          // inverte estado
    gpio_put(LED_PIN_Y, ledy_state);    // atualiza LED
    return true; // mantém o timer rodando
}

// callback do alarme vermelho
static int64_t alarm_cb_b(alarm_id_t id, void *user_data) {
    is_blinking = false;
    cancel_repeating_timer(timer_b);gpio_put(LED_PIN_B, 0);
    cancel_repeating_timer(timer_y);gpio_put(LED_PIN_Y, 0);
    return 0; // one-shot
}

void btn_callback(uint gpio, uint32_t events) {
    // botão vermelho
    if (gpio == BTN_PIN) {
        if (events & GPIO_IRQ_EDGE_FALL) { // pressionado
            
            if (!is_blinking){ is_blinking = true;
            alarm_cb_ = add_alarm_in_ms(5000, alarm_cb_b, NULL, false);//5000ms 5segundos
            add_repeating_timer_ms(75, timer_callback_b, NULL, &timer_b); // T / 2? para 500ms 
            add_repeating_timer_ms(250,timer_callback_y, NULL, &timer_y);}
            
        }
        if (events & GPIO_IRQ_EDGE_RISE) { // solto
            //cancel_alarm(alarm_r); 
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
    gpio_set_irq_enabled_with_callback(
        BTN_PIN, GPIO_IRQ_EDGE_FALL | GPIO_IRQ_EDGE_RISE, true, &btn_callback);
        
    while (true) {

    }
    }

