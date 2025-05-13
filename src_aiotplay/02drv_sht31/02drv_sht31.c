#include <string.h>
#include <stdbool.h>
#include "board.h"
#include "periodictimer.h"
#include "sht31.h"

//=========================== defines =========================================

#define SHT31_READPERIOD_S 1

//=========================== typedef =========================================

//=========================== variables =======================================

typedef struct {
    uint16_t       temp_raw;
    uint16_t       humidity_raw;
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

    // sht31
    sht31_init();

    // initialize the periodic timer
    periodictimer_init(
        SHT31_READPERIOD_S,  // period_s
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
    
    // debug
    app_dbg.numcalls_periodtimer_cb++;

    // read SHT31
    sht31_readTempHumidity(
        &app_vars.temp_raw,       // temp_raw
        &app_vars.humidity_raw    // humidity_raw
    );
}
