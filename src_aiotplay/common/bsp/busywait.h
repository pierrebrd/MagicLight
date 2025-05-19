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
void busywait_approx_62_5ms(void);
void busywait_approx_31_25ms(void);
void busywait_approx_15_625ms(void);
void busywait_approx_7_8125ms(void);
void busywait_approx_3_90625ms(void);

/**
\}
\}
*/

#endif