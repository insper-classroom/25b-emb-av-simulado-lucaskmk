#include <FreeRTOS.h>
#include <task.h>
#include <semphr.h>
#include <queue.h>

#include <stdio.h>
#include <string.h>
#include "pico/stdlib.h"
#include "hardware/gpio.h"

const int LED_PIN_B = 8;
const int LED_PIN_Y = 13;

typedef struct input {
    int num_led1;
    int num_led2;
} input_t;

QueueHandle_t xQueueInput;
QueueHandle_t xQueueLed1;
QueueHandle_t xQueueLed2;
SemaphoreHandle_t xSemaphoreLed2;

/**
 * NÃO MEXER!
 */
void input_task(void* p) {
    input_t test_case;

    test_case.num_led1 = 3;
    test_case.num_led2 = 4;
    xQueueSend(xQueueInput, &test_case, 0);

    test_case.num_led1 = 0;
    test_case.num_led2 = 2;
    xQueueSend(xQueueInput, &test_case, 0);

    while (true) {
        vTaskDelay(pdMS_TO_TICKS(1000));
    }
}

/**
 * MAIN TASK: lê xQueueInput e distribui
 */
void main_task(void* p) {
    input_t data;
    while (true) {
        if (xQueueReceive(xQueueInput, &data, portMAX_DELAY)) {
            xQueueSend(xQueueLed1, &data.num_led1, portMAX_DELAY);
            xQueueSend(xQueueLed2, &data.num_led2, portMAX_DELAY);
        }
    }
}

/**
 * LED 1 TASK: pisca LED Azul
 */
void led_1_task(void* p) {
    int count;
    while (true) {
        if (xQueueReceive(xQueueLed1, &count, portMAX_DELAY)) {
            for (int i = 0; i < count; i++) {
                gpio_put(LED_PIN_B, 1);
                vTaskDelay(pdMS_TO_TICKS(500));
                gpio_put(LED_PIN_B, 0);
                vTaskDelay(pdMS_TO_TICKS(500));
            }
            // sinaliza que LED amarelo pode começar
            xSemaphoreGive(xSemaphoreLed2);
        }
    }
}

/**
 * LED 2 TASK: pisca LED Amarelo
 */
void led_2_task(void* p) {
    int count;
    while (true) {
        if (xQueueReceive(xQueueLed2, &count, portMAX_DELAY)) {
            // espera LED azul terminar
            xSemaphoreTake(xSemaphoreLed2, portMAX_DELAY);
            for (int i = 0; i < count; i++) {
                gpio_put(LED_PIN_Y, 1);
                vTaskDelay(pdMS_TO_TICKS(500));
                gpio_put(LED_PIN_Y, 0);
                vTaskDelay(pdMS_TO_TICKS(500));
            }
        }
    }
}

int main() {
    stdio_init_all();

    // Inicialização GPIO
    gpio_init(LED_PIN_B);
    gpio_set_dir(LED_PIN_B, GPIO_OUT);
    gpio_put(LED_PIN_B, 0);

    gpio_init(LED_PIN_Y);
    gpio_set_dir(LED_PIN_Y, GPIO_OUT);
    gpio_put(LED_PIN_Y, 0);

    /**
     * manter essas duas linhas!
     */
    xQueueInput = xQueueCreate(32, sizeof(input_t));
    xTaskCreate(input_task, "Input", 256, NULL, 1, NULL);

    /**
     * Seu código vem aqui!
     */
    xQueueLed1 = xQueueCreate(32, sizeof(int));
    xQueueLed2 = xQueueCreate(32, sizeof(int));
    xSemaphoreLed2 = xSemaphoreCreateBinary();

    xTaskCreate(main_task, "Main", 256, NULL, 1, NULL);
    xTaskCreate(led_1_task, "LED1", 256, NULL, 1, NULL);
    xTaskCreate(led_2_task, "LED2", 256, NULL, 1, NULL);

    vTaskStartScheduler();

    while (1) {}
    return 0;
}
