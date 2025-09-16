/*
 * LED blink with FreeRTOS
 */
#include <FreeRTOS.h>
#include <task.h>
#include <semphr.h>
#include <queue.h>



#include "pico/stdlib.h"
#include <stdio.h>


int main() {
    stdio_init_all();


    while (true) {

        printf("exe4 \n");
        sleep_ms(500);
    }

}
