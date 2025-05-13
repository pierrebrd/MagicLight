#include <string.h>
#include <stdbool.h>
#include "board.h"
#include "periodictimer.h"
#include "leds.h"

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

//=========================== main ============================================

int main(void) {
    
    // clear module variables
    memset(&app_vars,0x00,sizeof(app_vars_t));
    memset(&app_dbg, 0x00,sizeof(app_dbg_t));

    // bsp
    board_init();

    // leds
    leds_init();

    // initialize the periodic timer
    periodictimer_init(
        BLINK_PERIOD_S,      // period_s
        _periodtimer_cb      // periodtimer_cb
    );

    // main loop
    while(1) {

        // wait for event
        board_sleep();
    }
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
