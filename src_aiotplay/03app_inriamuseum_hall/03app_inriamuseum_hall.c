#include <string.h>
#include <stdbool.h>
#include "board.h"
#include "periodictimer.h"
#include "ntw.h"

//=========================== defines =========================================

//=========================== typedef =========================================

//=========================== variables =======================================

typedef struct {
    uint32_t dummy;
} app_vars_t;

app_vars_t app_vars;

typedef struct {
    uint32_t  numcalls_main;
    uint32_t  numcalls_ntw_joining_cb;
    uint32_t  numcalls_ntw_getMoteId_cb;
    uint32_t  numcalls_ntw_getTime_cb;
    uint32_t  numcalls_ntw_receive_cb;
} app_dbg_t;

app_dbg_t app_dbg;

//=========================== prototypes ======================================

void _ntw_joining_cb(void);
void _ntw_getMoteId_cb(dn_ipmt_getParameter_moteId_rpt* reply);
void _ntw_getTime_cb(dn_ipmt_getParameter_time_rpt* reply);
void _ntw_receive_cb(uint8_t* buf, uint8_t bufLen);

//=========================== main ============================================

int main(void) {
    uint8_t txBuf[1];

    //=== initialize variables
    memset(&app_vars,0x00,sizeof(app_vars));
    memset(&app_dbg, 0x00,sizeof(app_dbg));

    // debug
    app_dbg.numcalls_main++;

    //=== initialize bsp/ntw
    board_init();
    ntw_init(
        _ntw_joining_cb,     // ntw_joining_cb
        _ntw_getMoteId_cb,   // ntw_getMoteId_cb
        _ntw_getTime_cb,     // ntw_getTime_cb
        _ntw_receive_cb      // ntw_receive_cb
    );

    // main loop
    while(1) {

        board_sleep();
    }
}

//=========================== private =========================================

void _ntw_joining_cb(void) {
    
    // debug
    app_dbg.numcalls_ntw_joining_cb++;
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

//=========================== interrupt handlers ==============================
