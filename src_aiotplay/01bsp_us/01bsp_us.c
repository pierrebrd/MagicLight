#include <string.h>
#include <stdbool.h>
#include "board.h"
#include "us.h"
#include "periodictimer.h"

//=========================== defines =========================================

#define MEASURING_PERIOD_S 1

//=========================== typedef =========================================

//=========================== variables =======================================

typedef struct {
    bool     doUsRead;
    uint8_t  us_num_reads;
    uint16_t us_val;
} app_vars_t;

app_vars_t app_vars;

//=========================== prototypes ======================================

void _periodtimer_cb(void);

//=========================== main ============================================

int main(void) {
   
    // reset variables
    memset(&app_vars,0x00,sizeof(app_vars_t));

    // bsp
    board_init();

    // US init
    us_init();

    // initialize the periodic timer
    periodictimer_init(
        MEASURING_PERIOD_S,  // period_s
        _periodtimer_cb      // periodtimer_cb
    );

    // main loop
    while(1) {
        
        // wait to be asked to do read
        while(app_vars.doUsRead==false) {
            board_sleep();
        }

        // do read
        app_vars.us_num_reads++;
        app_vars.us_val   = us_measure();
        app_vars.doUsRead = false;
    }
}

//=========================== private =========================================

void _periodtimer_cb(void) {
    app_vars.doUsRead = true;
}
