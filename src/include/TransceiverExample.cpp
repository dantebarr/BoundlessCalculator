#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/gpio.h"

#include "Sender.h"
#include "Receiver.h"


void gpioIRQ(uint gpio, uint32_t events) {
    printf("GPIO %d with event %u\n", gpio, events);
    //To check if the transmission was successful, it would be advisable to set a boolean flag here
    //Then wait to see if it changes in a time frame

    //As a result of this transmission, handleIRQ() MUST be eventually called on the appropriate object
}

int main() {
    stdio_init_all();
    char reserved[10];

    //This text won't be seen probably
    printf("Begin\nType anything to continue\n");
    scanf("%s", &reserved);

    Sender sender(0, 2, 0, 3, 1, 4);

    gpio_set_irq_enabled_with_callback(4, GPIO_IRQ_EDGE_FALL | GPIO_IRQ_EDGE_RISE, true, &gpioIRQ);
    //Currently the GPIO parameter is ignored, and this callback will be called for any enabled GPIO IRQ on any pin. (lol)

    sender.sendMessage(0x43, 0x12345);

    printf("End\n");
    while(1){}//Spin to maintain connection and wait for interrupt
    return 0;
}
