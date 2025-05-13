#include <string.h>
#include "fet.h"
#include "gpio.h"

//=========================== variables =======================================

//=========================== prototypes ======================================

//=========================== public ==========================================

void fet_init(void) {
    
    gpio_P020_output_init(); // leds_white
    fet_off();
}

void fet_on(void) {
    gpio_P020_output_low();
}

void fet_off(void) {
    gpio_P020_output_high();
}

//=========================== private =========================================

//=========================== interrupt handlers ==============================
