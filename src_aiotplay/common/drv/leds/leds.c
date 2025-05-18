#include <string.h>
#include "leds.h"
#include "gpio.h"

//=========================== variables =======================================

//=========================== prototypes ======================================

//=========================== public ==========================================

void leds_init(void) { //from left to right: the first = blue; second = green ; third = GND ; last = red
    gpio_P002_output_init(); // leds_red
    leds_green_off();
    gpio_P003_output_init(); // leds_green
    leds_blue_off();
    gpio_P004_output_init(); // leds_blue
    leds_red_off();
}



void leds_red_on(void) {
    gpio_P002_output_high();
}

void leds_red_off(void) {
    gpio_P002_output_low();
}

void leds_blue_on(void) {
    gpio_P004_output_high();
}

void leds_blue_off(void) {
    gpio_P004_output_low();
}

void leds_green_on(void) {
    gpio_P003_output_high();
}

void leds_green_off(void) {
    gpio_P003_output_low();
}

//=========================== private =========================================

//=========================== interrupt handlers ==============================
