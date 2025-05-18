#include <string.h>
#include <stdbool.h>
#include "board.h"
#include "periodictimer.h"
#include "leds.h"
#include "gpio.h"

//=========================== defines =========================================

#define BLINK_PERIOD_S 1

//=========================== typedef =========================================

//=========================== variables =======================================

typedef struct {
    uint8_t        counter;
} app_vars_t;

app_vars_t app_vars;

typedef struct {
    uint32_t       numcalls_periodtimer_cb;
} app_dbg_t;

app_dbg_t app_dbg;


//=========================== prototypes ======================================

void _periodtimer_cb(void);

void nrf_delay_ms(uint32_t ms) {
    for (uint32_t i = 0; i < ms; i++) {
        for (volatile uint32_t j = 0; j < 64000; j++) {
        // vide loop -- just for the delay of time
        }
    }
}

//=========================== main ============================================

int main(void) {
    
    // clear module variables
    memset(&app_vars,0x00,sizeof(app_vars_t));
    memset(&app_dbg, 0x00,sizeof(app_dbg_t));

    // bsp
    board_init();

    gpio_init();

    // leds
    leds_init();

    // Initialize P17 as the PIR pin
    gpio_P017_input_init(NULL);

    while (1) {
        if (NRF_P0->IN & (1 << 17)) {
            // DP2 high -- movement detected -- light on
            leds_blue_on();
        } else {
            // no movement -- DP2 low -- light off
            leds_blue_off();
        }

        nrf_delay_ms(50);
    }


    //// initialize the periodic timer
    //periodictimer_init(
    //    BLINK_PERIOD_S,      // period_s
    //    _periodtimer_cb      // periodtimer_cb
    //);

    //// main loop
    //while(1) {

    //    // wait for event
    //    board_sleep();
    //}
}

//=========================== private =========================================

void _periodtimer_cb(void) {
    
    // increment
    app_vars.counter++;

    // set LEDs
    if (app_vars.counter&0x01) {
        leds_red_on();
    } else {
        leds_red_off();
    }
    if (app_vars.counter&0x02) {
        leds_blue_on();
    } else {
        leds_blue_off();
    }
    if (app_vars.counter&0x04) {
        leds_green_on();
    } else {
        leds_green_off();
    }
    if (app_vars.counter&0x08) {
        leds_white_on();
    } else {
        leds_white_off();
    }
}
