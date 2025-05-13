#include <string.h>
#include <stdbool.h>
#include "us.h"
#include "gpio.h"
#include "busywait.h"

//=========================== variables =======================================

typedef struct {
    bool     measurementOngoing;
    uint32_t distance;
} us_vars_t;

us_vars_t us_vars;

typedef struct {
    uint32_t numcalls_us_init;
    uint32_t numcalls_us_measure;
    uint32_t numcalls_echo_pin_toggle_cb;
    uint32_t numcalls_echo_pin_toggle_cb_high;
    uint32_t numcalls_echo_pin_toggle_cb_low;
} us_dbg_t;

us_dbg_t us_dbg;

//=========================== prototypes ======================================

static void _wait_10us(void);
static void _echo_pin_toggle_cb(uint8_t pin_state);

//=========================== public ==========================================

void us_init(void) {

    // clear module variables
    memset(&us_vars,0x00,sizeof(us_vars_t));
    memset(&us_dbg, 0x00,sizeof(us_dbg_t) );
    us_vars.measurementOngoing = false;

    // debug
    us_dbg.numcalls_us_init++;

    // PIN_TRIGGER
    gpio_P017_output_init();
    gpio_P017_output_low();

    // PIN_ECHO
    gpio_P015_input_init(_echo_pin_toggle_cb);
}

/**
Returns the distance in cm, to the target.


The ECHO pin measures the time it takes for the pulse to go back and forth between
sensor and target.
Let's assume the speed of sound in air at 20C is 343 m/s.
So it's roughtly 1cm per 34ticks.
Of course, this is the RTT, to get a distance, we just divided the value by 2.
*/
uint16_t us_measure(void) {
    
    // debug
    us_dbg.numcalls_us_measure++;

    // arm
    us_vars.measurementOngoing         = true;

    // configure/start the RTC
    NRF_TIMER0->MODE                   = 0x00000000;       // 0x00000000==Timer
    NRF_TIMER0->BITMODE                = 3;                // 3==32Bit
    NRF_TIMER0->PRESCALER              = 4;                // fTIMER = 16 MHz / (2^PRESCALER) -> 4==(fTIMER==16/(2^4)==1MHz)
    // 1098 7654 3210 9876 5432 1098 7654 3210
    // xxxx xxxx xxFE DCBA xxxx xxxx xxxx xxBA (A=compare 0)
    // 0000 0000 0000 0001 0000 0000 0000 0000 
    //    0    0    0    1    0    0    0    0 0x00010000
    NRF_TIMER0->INTENSET               = 0x00010000;       // enable compare 0 interrupts

    // enable interrupts
    NVIC_SetPriority(TIMER0_IRQn, 2);
    NVIC_ClearPendingIRQ(TIMER0_IRQn);
    NVIC_EnableIRQ(TIMER0_IRQn);

    // have TIMER0 timeout; start TIMER0
    NRF_TIMER0->CC[0]                  = 500000;           // in us
    NRF_TIMER0->CC[1]                  = 0;
    NRF_TIMER0->CC[2]                  = 0;
    NRF_TIMER0->TASKS_CLEAR            = 0x00000001;       // clear
    NRF_TIMER0->TASKS_START            = 0x00000001;       // start

    // trigger
    gpio_P017_output_high();
    _wait_10us();
    gpio_P017_output_low();
    
    // block until finished
    while (us_vars.measurementOngoing==true);
    
    if (NRF_TIMER0->CC[1]==0x0000 && NRF_TIMER0->CC[2]==0x0000) {
        us_vars.distance               = US_DISTANCE_INVALID;
    } else {
        us_vars.distance               = NRF_TIMER0->CC[2];
        us_vars.distance              -= NRF_TIMER0->CC[1];
        us_vars.distance              /= 34; // 1cm per 34us
        us_vars.distance              /= 2;  // distance is half the round-trip distance
    }
        
    return (uint16_t)us_vars.distance;
}

//=========================== private =========================================

static void _wait_10us(void) {
   volatile uint32_t counter;

   for (counter=0; counter<0xfff; counter++) {
       __NOP();
   }
}

static void _echo_pin_toggle_cb(uint8_t pin_state) {
    
    // debug
    us_dbg.numcalls_echo_pin_toggle_cb++;
    if (pin_state==1) {
        us_dbg.numcalls_echo_pin_toggle_cb_high++;
        NRF_TIMER0->TASKS_CAPTURE[1] = 0x00000001;
    } else {
        us_dbg.numcalls_echo_pin_toggle_cb_low++;
        NRF_TIMER0->TASKS_CAPTURE[2] = 0x00000001;
        // measurement now done
        us_vars.measurementOngoing     = false;
        NRF_TIMER0->TASKS_STOP         = 0x00000001;
    }
}

//=========================== interrupt handlers ==============================

void TIMER0_IRQHandler(void) {

    // handle compare[0]
    if (NRF_TIMER0->EVENTS_COMPARE[0] == 0x00000001 ) {

        // clear flag
        NRF_TIMER0->EVENTS_COMPARE[0]  = 0x00000000;

        // stop NRF_TIMER0
        NRF_TIMER0->TASKS_STOP         = 0x00000001;

        // measurement now done
        us_vars.measurementOngoing     = false;
    }
}