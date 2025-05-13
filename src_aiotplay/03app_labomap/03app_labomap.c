#include <string.h>
#include <stdbool.h>
#include "board.h"
#include "sht31.h"
#include "periodictimer.h"
#include "ntw.h"

//=========================== defines =========================================

#define SHT31_READPERIOD_S 600

//=========================== typedef =========================================

typedef struct __attribute__ ((__packed__)) {
    uint16_t       temperature_raw;
    uint16_t       humidity_raw;
} labomap_ht;

//=========================== variables =======================================

typedef struct {
    uint16_t       temperature_raw;
    uint16_t       humidity_raw;
} app_vars_t;

app_vars_t app_vars;

typedef struct {
    uint32_t       numcalls_periodtimer_cb;
    uint32_t       numcalls_periodtimer_cb_sht31_success;
    uint32_t       numcalls_periodtimer_cb_sht31_fail;
    uint32_t       numcalls_periodtimer_cb_transmit_success;
    uint32_t       numcalls_periodtimer_cb_transmit_fail;
    uint32_t       numcalls_ntw_joining_cb;
    uint32_t       numcalls_ntw_getMoteId_cb;
    uint32_t       numcalls_ntw_getTime_cb;
    uint32_t       numcalls_ntw_receive_cb;
} app_dbg_t;

app_dbg_t app_dbg;

//=========================== prototypes ======================================

void _periodtimer_cb(void);
void _ntw_joining_cb(void);
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
    
    // sht31
    sht31_init();

    // periodic timer
    periodictimer_init(
        SHT31_READPERIOD_S,  // period_s
        _periodtimer_cb      // periodtimer_cb
    );
    
    // ntw
    ntw_init(
        _ntw_joining_cb,     // ntw_joining_cb
        _ntw_getMoteId_cb,   // ntw_getMoteId_cb
        _ntw_getTime_cb,     // ntw_getTime_cb
        _ntw_receive_cb      // ntw_receive_cb
    );
    
    // main loop
    while(1) {

        // wait for event
        board_sleep();
    }
}

//=========================== private =========================================

void _periodtimer_cb(void) {
    labomap_ht labomap_h;
    bool       rc;

    // debug
    app_dbg.numcalls_periodtimer_cb++;

    // read
    rc = sht31_readTempHumidity(
        &app_vars.temperature_raw,// temperature_raw
        &app_vars.humidity_raw    // humidity_raw
    );

    // fill
    if (rc==true) {
        app_dbg.numcalls_periodtimer_cb_sht31_success++;
        labomap_h.temperature_raw     = app_vars.temperature_raw;
        labomap_h.humidity_raw        = app_vars.humidity_raw;
    } else {
        app_dbg.numcalls_periodtimer_cb_sht31_fail++;
        labomap_h.temperature_raw     = 0;
        labomap_h.humidity_raw        = 0;
    }
    
    // send
    rc = ntw_transmit((uint8_t*)&labomap_h,sizeof(labomap_ht));

    // debug
    if (rc==true) {
        app_dbg.numcalls_periodtimer_cb_transmit_success++;
    } else {
        app_dbg.numcalls_periodtimer_cb_transmit_fail++;
    }
}

void _ntw_joining_cb(void) {
    
    // debug
    app_dbg.numcalls_ntw_joining_cb++;

    // nothing else to do
}

void _ntw_getMoteId_cb(dn_ipmt_getParameter_moteId_rpt* reply) {

    // debug
    app_dbg.numcalls_ntw_getMoteId_cb++;

    // nothing else to do
}

void _ntw_getTime_cb(dn_ipmt_getParameter_time_rpt* reply) {
    
    // debug
    app_dbg.numcalls_ntw_getTime_cb++;

    // nothing else to do
}

void _ntw_receive_cb(uint8_t* buf, uint8_t bufLen) {
    
    // debug
    app_dbg.numcalls_ntw_receive_cb++;

    // nothing else to do
}

//=========================== interrupt handlers ==============================
