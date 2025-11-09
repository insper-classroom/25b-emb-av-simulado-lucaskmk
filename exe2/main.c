#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/gpio.h"
#include "hardware/timer.h"

const int LED_PIN_B = 9;
const int LED_PIN_Y = 5;
const int BTN_PIN   = 28;

struct repeating_timer timer_b; volatile int f_timer_b = 0;//flag do timer
struct repeating_timer timer_y; volatile int f_timer_y = 0;//flag do timer
volatile int flag_b = 0;//flag do botao

volatile int r_alarm_0 = 0;//alarm runinng
 volatile int f_alarm_0 = 0;//flag do timer

int64_t alarm_0_callback(alarm_id_t id, void *user_data) {
    f_alarm_0 = 1;r_alarm_0 = 0;
    return 0;}

bool timer_b_callback(struct repeating_timer *t) {
    f_timer_b = 1;
    return true;} // mantém o timer rodando
bool timer_y_callback(struct repeating_timer *t) {
    f_timer_y = 1;
    return true;} //

    void btn_callback(uint gpio, uint32_t events) {
    if (events == 0x4) { // fall edge
        if (gpio == BTN_PIN) {
            flag_b =1;}
    } 
}

int main() {
    stdio_init_all();
        gpio_init(LED_PIN_B);
    gpio_set_dir(LED_PIN_B, GPIO_OUT);
        gpio_init(LED_PIN_Y);
    gpio_set_dir(LED_PIN_Y, GPIO_OUT);
    gpio_init(BTN_PIN);
    gpio_set_dir(BTN_PIN, GPIO_IN);
    gpio_pull_up(BTN_PIN);

    gpio_set_irq_enabled_with_callback(BTN_PIN, GPIO_IRQ_EDGE_FALL, true,
                                       &btn_callback);


    int r_timer_b = 0;//timer runinng
    int r_timer_y = 0;//timer runinng
    alarm_id_t alarm_0;//alarme globaaalll
    int led_stateb = 0;
    int led_statey = 0 ;
    while (true) {
            if (flag_b) {flag_b = 0;
                if (!r_timer_b){r_timer_b = 1;//variavel para falar que esta rodando
                    add_repeating_timer_ms(500, timer_b_callback, NULL,&timer_b);//500ms
                }
                if (!r_timer_y){r_timer_y= 1;//variavel para falar que esta rodando
                    add_repeating_timer_ms(150, timer_y_callback, NULL,&timer_y);//500ms
                }
                if (!r_alarm_0){
                r_alarm_0 = 1;
                alarm_0 = add_alarm_in_ms(5000, alarm_0_callback, NULL, false);}
             }
        if (f_alarm_0){
            f_alarm_0 = 0;
            
            r_timer_b = 0;
            cancel_repeating_timer(&timer_b);
            gpio_put(LED_PIN_B, 0);
            r_timer_y = 0;
            cancel_repeating_timer(&timer_y);
            gpio_put(LED_PIN_Y, 0);
        }
        if(f_timer_b){// flag do timer ativado, desativa e faz oq tem q fazer
            f_timer_b = 0;

            led_stateb = !led_stateb;
            gpio_put(LED_PIN_B, led_stateb);
        }
        if(f_timer_y){// flag do timer ativado, desativa e faz oq tem q fazer
            f_timer_y = 0;

            led_statey = !led_statey;
            gpio_put(LED_PIN_Y, led_statey);
        }
    }
}