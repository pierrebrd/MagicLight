#include "busywait.h"

//=========================== define ==========================================

#define DELAY_1s    0x003fffff
#define DELAY_500ms (DELAY_1s>>1)
#define DELAY_250ms (DELAY_1s>>2)
#define DELAY_125ms (DELAY_1s>>3)
#define DELAY_62ms (DELAY_1s>>4)
#define DELAY_31ms (DELAY_1s>>5)
#define DELAY_15ms (DELAY_1s>>6)
#define DELAY_8ms (DELAY_1s>>7)
#define DELAY_4ms (DELAY_1s>>8)

//=========================== variables =======================================

//=========================== prototypes ======================================

static void _busywait(uint32_t delay);

//=========================== public ==========================================

void busywait_approx_1s(void) { _busywait(DELAY_1s); }
void busywait_approx_500ms(void) { _busywait(DELAY_500ms); }
void busywait_approx_250ms(void) { _busywait(DELAY_250ms); }
void busywait_approx_125ms(void) { _busywait(DELAY_125ms); }
void busywait_approx_62ms(void) { _busywait(DELAY_62ms); }
void busywait_approx_31ms(void) { _busywait(DELAY_31ms); }
void busywait_approx_15ms(void) { _busywait(DELAY_15ms); }
void busywait_approx_8ms(void) { _busywait(DELAY_8ms); }
void busywait_approx_4ms(void) { _busywait(DELAY_4ms); }

//=========================== private =========================================

static void _busywait(uint32_t delay) {
    volatile uint32_t counter;

    for (counter = 0; counter < delay; counter++) {
        __NOP();
    }
}

//=========================== interrupt handlers ==============================
