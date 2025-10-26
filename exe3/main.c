/**
 * Copyright (c) 2020 Raspberry Pi (Trading) Ltd.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include <FreeRTOS.h>
#include <task.h>
#include <semphr.h>
#include <queue.h>

#include <stdio.h>
#include <string.h> 
#include "pico/stdlib.h"
#include "hardware/gpio.h"
#include "hardware/timer.h"

const int LED_PIN_B = 8;
const int LED_PIN_Y = 13;

typedef struct input {
    int num_led1;
    int num_led2;
} input_t;

QueueHandle_t xQueueInput;

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

    }
}

/**
 * Seu código vem aqui!
 */
QueueHandle_t xQueueLed1;    
QueueHandle_t xQueueLed2;        // Fila de comunicação
SemaphoreHandle_t xSemaphoreLed2; 

void main_task(void *p){
    input_t recived;
    while (1)
    {
    if(xQueueReceive(xQueueInput,&recived, portMAX_DELAY)){
        int n_piscadas1 = recived.num_led1;
        xQueueSend(xQueueLed1,&n_piscadas1,0);
        int n_piscadas2 = recived.num_led2;
        xQueueSend(xQueueLed2,&n_piscadas2,0);}
    }  
}

void led_1_task(void *p) {
    gpio_init(LED_PIN_B);
    gpio_set_dir(LED_PIN_B, GPIO_OUT);
    gpio_put(LED_PIN_B, 0);

    int n_piscadas_recived;
    while (1) {
        if (xQueueReceive(xQueueLed1, &n_piscadas_recived, portMAX_DELAY)) {
            for (int i = 0; i < n_piscadas_recived; ++i) {
                gpio_put(LED_PIN_B, 1);
                vTaskDelay(pdMS_TO_TICKS(500));
                gpio_put(LED_PIN_B, 0);
                vTaskDelay(pdMS_TO_TICKS(500));
            }
            xSemaphoreGive(xSemaphoreLed2);
        }  
    }
}
void led_2_task(void *p) {
    gpio_init(LED_PIN_Y);
    gpio_set_dir(LED_PIN_Y, GPIO_OUT);
    gpio_put(LED_PIN_Y, 0);

    int n_piscadas_recived2;
    while (1) {
        if (xSemaphoreTake(xSemaphoreLed2,portMAX_DELAY)){
              if (xQueueReceive(xQueueLed2, &n_piscadas_recived2, 0)) {
            for (int i = 0; i < n_piscadas_recived2; ++i) {
                gpio_put(LED_PIN_Y, 1);
                vTaskDelay(pdMS_TO_TICKS(500));
                gpio_put(LED_PIN_Y, 0);
                vTaskDelay(pdMS_TO_TICKS(500));
            }
        }
        }
    }
}

int main() {
    stdio_init_all();

    /**
     * manter essas duas linhas!
     */
    xQueueInput = xQueueCreate(32, sizeof(input_t));
    xTaskCreate(input_task, "Input", 256, NULL, 1, NULL);

    /**
     * Seu código vem aqui!
     */
    xQueueLed1 = xQueueCreate(8, sizeof(int));
    xQueueLed2 = xQueueCreate(8, sizeof(int));

    xSemaphoreLed2 = xSemaphoreCreateBinary();
    xTaskCreate(main_task, "Main", 512, NULL, 2, NULL);
    xTaskCreate(led_1_task, "Led1", 256, NULL, 2, NULL);
    xTaskCreate(led_2_task, "Led2", 256, NULL, 1, NULL);

    vTaskStartScheduler();

    while (1) {}

    return 0;
}