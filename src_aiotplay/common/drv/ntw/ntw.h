#ifndef __NTW_H
#define __NTW_H

#include <stdbool.h>
#include "nrf52833.h"
#include "dn_ipmt.h"

//=========================== define ==========================================

//=========================== typedef =========================================

typedef void (*ntw_joining_cbt)(void);
typedef void (*ntw_getMoteId_cbt)(dn_ipmt_getParameter_moteId_rpt* reply);
typedef void (*ntw_time_cbt)(dn_ipmt_getParameter_time_rpt* reply);
typedef void (*ntw_receive_cbt)(uint8_t* buf, uint8_t bufLen);

//=========================== variables =======================================

//=========================== prototypes ======================================

void ntw_init(
    ntw_joining_cbt   ntw_joining_cb,
    ntw_getMoteId_cbt ntw_getMoteId_cb,
    ntw_time_cbt      ntw_time_cb,
    ntw_receive_cbt   ntw_receive_cb
);
bool ntw_getMoteId(void);
bool ntw_getTime(void);
bool ntw_transmit(uint8_t* buf, uint8_t bufLen);

/**
\}
\}
*/

#endif