/**
 * EXE3 - estilo do professor (non-blocking checks / small timeouts)
 *
 * Copyright (c) 2020 Raspberry Pi (Trading) Ltd.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include <FreeRTOS.h>
#include <queue.h>
#include <semphr.h>
#include <task.h>

#include "pico/stdlib.h"
#include "hardware/gpio.h"

const int LED_PIN_B = 8;   // azul
const int LED_PIN_Y = 13;  // amarelo

typedef struct input {
    int num_led1;
    int num_led2;
} input_t;

/* fila já fornecida */
QueueHandle_t xQueueInput;

/* filas e semáforo que vamos criar */
QueueHandle_t xQueueLed1;
QueueHandle_t xQueueLed2;
SemaphoreHandle_t xSemaphoreLed2;

/* Input task (NÃO MEXER) */
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

/* main_task: lê xQueueInput e repassa para xQueueLed1 / xQueueLed2
   Estilo: checagem não-bloqueante (if(xQueueReceive(...,0))) e loop com delay */
void main_task(void *p) {
    input_t rx;
    while (1) {
        if (xQueueReceive(xQueueInput, &rx, 0)) {
            int a = rx.num_led1;
            int b = rx.num_led2;
            /* envia para filas dos leds (não-bloqueante, estilo prof) */
            xQueueSend(xQueueLed1, &a, 0);
            xQueueSend(xQueueLed2, &b, 0);
        }
        /* evita busy-waiting agressivo */
        vTaskDelay(pdMS_TO_TICKS(10));
    }
}

/* led_1_task: verifica a sua fila; se houver contagem -> pisca e ao terminar dá semáforo */
void led_1_task(void *p) {
    int n = 0;
    /* inicializa GPIO (pode ser no main também) */
    gpio_init(LED_PIN_B);
    gpio_set_dir(LED_PIN_B, GPIO_OUT);
    gpio_put(LED_PIN_B, 0);

    while (1) {
        if (xQueueReceive(xQueueLed1, &n, 0)) {
            /* Se n==0, não pisca (comportamento do input) */
            for (int i = 0; i < n; ++i) {
                gpio_put(LED_PIN_B, 1);
                vTaskDelay(pdMS_TO_TICKS(500));
                gpio_put(LED_PIN_B, 0);
                vTaskDelay(pdMS_TO_TICKS(500));
            }
            /* libera led2 (estilo prof: sem timeout aqui) */
            xSemaphoreGive(xSemaphoreLed2);
        }
        vTaskDelay(pdMS_TO_TICKS(5)); /* polling leve */
    }
}

/* led_2_task: aguarda semáforo (com timeout curto no estilo do prof) e só então consome sua fila */
void led_2_task(void *p) {
    int n = 0;
    gpio_init(LED_PIN_Y);
    gpio_set_dir(LED_PIN_Y, GPIO_OUT);
    gpio_put(LED_PIN_Y, 0);

    while (1) {
        /* usa timeout curto igual ao exemplo do professor (10 ticks) */
        if (xSemaphoreTake(xSemaphoreLed2, 10)) {
            /* após receber semáforo, pega a contagem correspondente */
            if (xQueueReceive(xQueueLed2, &n, 0)) {
                for (int i = 0; i < n; ++i) {
                    gpio_put(LED_PIN_Y, 1);
                    vTaskDelay(pdMS_TO_TICKS(500));
                    gpio_put(LED_PIN_Y, 0);
                    vTaskDelay(pdMS_TO_TICKS(500));
                }
            }
        }
        vTaskDelay(pdMS_TO_TICKS(5));
    }
}

int main() {
    stdio_init_all();

    /* manter essas duas linhas */
    xQueueInput = xQueueCreate(32, sizeof(input_t));
    xTaskCreate(input_task, "Input", 256, NULL, 1, NULL);

    /* ----- nosso setup (estilo professor) ----- */
    xQueueLed1 = xQueueCreate(8, sizeof(int));
    xQueueLed2 = xQueueCreate(8, sizeof(int));

    xSemaphoreLed2 = xSemaphoreCreateBinary();
    /* Não damos o semáforo aqui: led_1 dará após terminar de piscar */

    /* cria tasks (prioridades no estilo do prof) */
    xTaskCreate(main_task, "Main", 512, NULL, 2, NULL);
    xTaskCreate(led_1_task, "Led1", 256, NULL, 2, NULL);
    xTaskCreate(led_2_task, "Led2", 256, NULL, 1, NULL);

    vTaskStartScheduler();

    while (1) { }
    return 0;
}
