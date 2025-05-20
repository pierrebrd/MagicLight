#ifndef __BUSYWAIT_H
#define __BUSYWAIT_H

#include "nrf52833.h"

//=========================== define ==========================================

//=========================== typedef =========================================

//=========================== variables =======================================

//=========================== prototypes ======================================

void busywait_approx_1s(void);
void busywait_approx_500ms(void);
void busywait_approx_250ms(void);
void busywait_approx_125ms(void);
void busywait_approx_62ms(void);
void busywait_approx_31ms(void);
void busywait_approx_15ms(void);
void busywait_approx_8ms(void);
void busywait_approx_4ms(void);

/**
\}
\}
*/

#endif