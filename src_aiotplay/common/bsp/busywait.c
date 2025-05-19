#include "busywait.h"

//=========================== define ==========================================

#define DELAY_1s    0x003fffff
#define DELAY_500ms (DELAY_1s>>1)
#define DELAY_250ms (DELAY_1s>>2)
#define DELAY_125ms (DELAY_1s>>3)
#define DELAY_62_5ms (DELAY_1s>>4)
#define DELAY_31_25ms (DELAY_1s>>5)
#define DELAY_15_625ms (DELAY_1s>>6)
#define DELAY_7_8125ms (DELAY_1s>>7)
#define DELAY_3_90625ms (DELAY_1s>>8)

//=========================== variables =======================================

//=========================== prototypes ======================================

static void _busywait(uint32_t delay);

//=========================== public ==========================================

void busywait_approx_1s(void)     { _busywait(DELAY_1s);    }
void busywait_approx_500ms(void)  { _busywait(DELAY_500ms); }
void busywait_approx_250ms(void)  { _busywait(DELAY_250ms); }
void busywait_approx_125ms(void)  { _busywait(DELAY_125ms); }
void busywait_approx_62_5ms(void) { _busywait(DELAY_62_5ms); }
void busywait_approx_31_25ms(void) { _busywait(DELAY_31_25ms); }
void busywait_approx_15_625ms(void) { _busywait(DELAY_15_625ms); }
void busywait_approx_7_8125ms(void) { _busywait(DELAY_7_8125ms); }
void busywait_approx_3_90625ms(void) { _busywait(DELAY_3_90625ms); }

//=========================== private =========================================

static void _busywait(uint32_t delay) {
   volatile uint32_t counter;

   for (counter=0; counter<delay; counter++) {
       __NOP();
   }
}

//=========================== interrupt handlers ==============================
