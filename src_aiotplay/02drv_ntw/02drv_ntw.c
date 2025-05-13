#include <string.h>
#include <stdbool.h>
#include "board.h"
#include "periodictimer.h"
#include "ntw.h"

//=========================== defines =========================================

#define DATA_PERIOD_S 10

//=========================== typedef =========================================

//=========================== variables =======================================

typedef struct {
    uint8_t        txCounter;
} app_vars_t;

app_vars_t app_vars;

typedef struct {
    uint32_t       numcalls_periodtimer_cb;
    uint32_t       numcalls_periodtimer_cb_success;
    uint32_t       numcalls_periodtimer_cb_fail;
    uint32_t       numcalls_ntw_getMoteId_cb;
    uint32_t       numcalls_ntw_getTime_cb;
    uint32_t       numcalls_ntw_receive_cb;
} app_dbg_t;

app_dbg_t app_dbg;

//=========================== prototypes ======================================

void _periodtimer_cb(void);
void _ntw_getMoteId_cb(dn_ipmt_getParameter_moteId_rpt* reply);
void _ntw_getTime_cb(dn_ipmt_getParameter_time_rpt* reply);
void _ntw_receive_cb(uint8_t* buf, uint8_t bufLen);

//=========================== main ============================================

int main(void) {

    // initialize variables
    memset(&app_vars,0x00,sizeof(app_vars));
    memset(&app_dbg, 0x00,sizeof(app_dbg));
    
    // bsp
    board_init();

    // ntw
    ntw_init(
        NULL,                // ntw_joining_cb
        _ntw_getMoteId_cb,   // ntw_getMoteId_cb
        _ntw_getTime_cb,     // ntw_getTime_cb
        _ntw_receive_cb      // ntw_receive_cb
    );

    // initialize the periodic timer
    periodictimer_init(
        DATA_PERIOD_S,       // period_s
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
    uint8_t txBuf[4];
    bool    success;

    // debug
    app_dbg.numcalls_periodtimer_cb++;

    // increment
    app_vars.txCounter++;

    // fill txBuf
    txBuf[0] = app_vars.txCounter;
    txBuf[1] = 0x01;
    txBuf[2] = 0x02;
    txBuf[3] = 0x03;

    // send txBuf
    success = ntw_transmit(txBuf,sizeof(txBuf));

    // debug
    if (success==true) {
        app_dbg.numcalls_periodtimer_cb_success++;
    } else {
        app_dbg.numcalls_periodtimer_cb_fail++;
    }
}

void _ntw_getMoteId_cb(dn_ipmt_getParameter_moteId_rpt* reply) {

    // debug
    app_dbg.numcalls_ntw_getMoteId_cb++;
}

void _ntw_getTime_cb(dn_ipmt_getParameter_time_rpt* reply) {

    // debug
    app_dbg.numcalls_ntw_getTime_cb++;
}

void _ntw_receive_cb(uint8_t* buf, uint8_t bufLen) {
    
    // debug
    app_dbg.numcalls_ntw_receive_cb++;
}
