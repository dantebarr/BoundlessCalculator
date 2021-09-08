#pragma once

#include "hardware/gpio.h"
#include "pico/stdlib.h"

typedef uint8_t u8;
typedef uint64_t u64;

#define LENGTH_PATTERNS 7
#define NUM_PATTERNS 8
#define END_LENGTH 6000 //How long to stay off before the repeat in ms
#define SHORT_FLASH 500, 200 //Off then on duration in ms
#define LONG_FLASH 500, 1000


struct Patterns {
    unsigned short duration[NUM_PATTERNS]; //In ms
    bool state[NUM_PATTERNS]; //On or off
};

static volatile u8 i;//Where we are in the pattern
const static struct Patterns* pattern;
static u8 pin;

int64_t callback(alarm_id_t id, void *data){
    (void) data;
    (void) id;

    if(i == LENGTH_PATTERNS-1)
        i = 0;
    else
        i++;

    gpio_put(pin, pattern->state[i]);

    return pattern->duration[i]*1000;//Convert us to ms
}

class LED {
    alarm_id_t alarm;

    const struct Patterns patterns[NUM_PATTERNS] = {
        {{SHORT_FLASH, SHORT_FLASH, SHORT_FLASH, END_LENGTH}, {0, 1, 0, 1, 0, 1, 0}},
        {{LONG_FLASH, LONG_FLASH, LONG_FLASH, END_LENGTH}, {0, 1, 0, 1, 0, 1, 0}},
        {{SHORT_FLASH, LONG_FLASH, SHORT_FLASH, END_LENGTH}, {0, 1, 0, 1, 0, 1, 0}},
        {{LONG_FLASH, SHORT_FLASH, LONG_FLASH, END_LENGTH}, {0, 1, 0, 1, 0, 1, 0}},
        {{LONG_FLASH, SHORT_FLASH, SHORT_FLASH, END_LENGTH}, {0, 1, 0, 1, 0, 1, 0}},
        {{SHORT_FLASH, SHORT_FLASH, LONG_FLASH, END_LENGTH}, {0, 1, 0, 1, 0, 1, 0}},
        {{LONG_FLASH, LONG_FLASH, SHORT_FLASH, END_LENGTH}, {0, 1, 0, 1, 0, 1, 0}},
        {{SHORT_FLASH, LONG_FLASH, LONG_FLASH, END_LENGTH}, {0, 1, 0, 1, 0, 1, 0}},
    };


public:
    LED(u8 Pin){
        pin = Pin;
        gpio_init(Pin);
        gpio_set_dir(Pin, GPIO_OUT);
    }

    //Seed can be any u8, we convert it to the range we want
    void enterTest(u8 Seed){
        i = -1;
        pattern = &patterns[Seed % NUM_PATTERNS];
        alarm = add_alarm_in_ms(500, callback, NULL, true);//Wait before starting the pattern
    }

    void exitTest(){
        cancel_alarm(alarm);
        gpio_put(pin, 0);
    }
};


