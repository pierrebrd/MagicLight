#include <string.h>
#include <stdbool.h>
#include "board.h"
#include "periodictimer.h"
#include "ntw.h"
#include "music.h"
#include "leds.h"
#include "us.h"

//=========================== defines =========================================

#define TICKS_PER_SLOT                   234            // 0.00725*32768
#define RTC0PERIOD_STEP_0_JOINING        (32768)      //    32768 = 1s
#define RTC0PERIOD_STEP_1_LOWPOWER       (5*32768)      //  5*32768 = 5s
#define RTC0PERIOD_STEP_2_US             (5*32768)      //  5*32768 = 5s
#define RTC0PERIOD_STEP_3_MUSIC_ASN3     (32768>>1)     // 32768>>1 = 500ms
#define US_THRESHOLD_SOMEONE             150            // in cm

#define MSGID_CMD_LOWPOWER                0x01
#define MSGID_CMD_ACTIVE                  0x02
#define MSGID_CMD_MUSIC_STAR_WARS         0x03
#define MSGID_CMD_MUSIC_HARRY_POTTER      0x04

//=========================== typedef =========================================

typedef enum {
    STEP_0_JOINING,
    STEP_1_LOWPOWER,
    STEP_2_US,
    STEP_3_MUSIC_ASN3,
    STEP_4_MUSIC_ASN4,
} step_t;

//=========================== variables =======================================

typedef struct {
    step_t    step;
    uint16_t  moteId;
    bool      doUsRead;
    bool      someoneDetected;
    uint16_t  us_val;
    uint8_t   asn[5];
    uint8_t   f_play_harry_potter;
    uint8_t   f_play_star_wars;
} app_vars_t;

app_vars_t app_vars;

typedef struct {
    uint32_t  numcalls_main;
    uint32_t  us_num_reads;
    uint32_t  numcalls_ntw_joining_cb;
    uint32_t  numcalls_ntw_getMoteId_cb;
    uint32_t  numerr_ntw_getMoteId_rc;
    uint32_t  numcalls_ntw_getTime_cb;
    uint32_t  numerr_ntw_getTime_rc;
    uint32_t  numerr_ntw_getTime_wrong_step;
    uint32_t  num_ntw_getTime_STEP_3_ASN3;
    uint32_t  num_ntw_getTime_STEP_4_ASN4;
    uint32_t  numcalls_ntw_receive_cb;
    uint32_t  numerr_ntw_receive_cb_wrong_msgid;
    uint32_t  numcalls_RTC0_IRQHandler;
    uint32_t  numcalls_RTC0_IRQHandler_EVENTS_COMPARE0;
    uint32_t  numerr_RTC0_IRQHandler_wrong_step;
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
    app_vars.step                      = STEP_0_JOINING;

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
    music_init();
    leds_init();
    us_init();

    //=== prepare RTC0

    // configure/start the RTC
    // 1098 7654 3210 9876 5432 1098 7654 3210
    // xxxx xxxx xxxx FEDC xxxx xxxx xxxx xxBA (C=compare 0)
    // 0000 0000 0000 0001 0000 0000 0000 0000 
    //    0    0    0    1    0    0    0    0 0x00010000
    NRF_RTC0->EVTENSET                 = 0x00010000;       // enable compare 0 event routing
    NRF_RTC0->INTENSET                 = 0x00010000;       // enable compare 0 interrupts

    // enable interrupts
    NVIC_SetPriority(RTC0_IRQn, 1);
    NVIC_ClearPendingIRQ(RTC0_IRQn);
    NVIC_EnableIRQ(RTC0_IRQn);

    // query getMoteId periodically
    NRF_RTC0->CC[0]                    = RTC0PERIOD_STEP_0_JOINING;
    NRF_RTC0->TASKS_START              = 0x00000001;

    // white led: no connection to network yet
    leds_white_on();

    // main loop
    while(1) {

        // wait to be asked to pull us
        while (app_vars.doUsRead==false) {
            board_sleep();
        }

        // poll us
        app_dbg.us_num_reads++;
        app_vars.us_val   = us_measure();
        if (app_vars.us_val<US_THRESHOLD_SOMEONE) {
            // someone detected

            if (app_vars.someoneDetected==false) {
                // state change

                // LEDs
                leds_green_on();
                leds_red_off();

                // remember
                app_vars.someoneDetected = true;

                // send
                txBuf[0] = 0x01;
                ntw_transmit(txBuf,sizeof(txBuf));
            } else {
                // same state

                // LEDs
                leds_green_off();
                leds_red_off();
            }
        } else {
            // nobody

            if (app_vars.someoneDetected==true) {
                // state change
                
                // LEDs
                leds_green_off();
                leds_red_on();

                // remember
                app_vars.someoneDetected = false;

                // send
                txBuf[0] = 0x00;
                ntw_transmit(txBuf,sizeof(txBuf));

            } else {
                // same state

                // LEDs
                leds_green_off();
                leds_red_off();
            }
        }
        app_vars.doUsRead = false;
    }
}

//=========================== private =========================================

void _ntw_joining_cb(void) {
    
    // debug
    app_dbg.numcalls_ntw_joining_cb++;

    // white led: no connection to mote yet
    leds_white_off();

    // blue led: joining
    leds_blue_on();
}

void _ntw_getMoteId_cb(dn_ipmt_getParameter_moteId_rpt* reply) {

    // debug
    app_dbg.numcalls_ntw_getMoteId_cb++;

    // white led: no connection to network yet
    leds_white_off();

    do {
        if (reply->RC!=DN_ERR_NONE) {
            app_dbg.numerr_ntw_getMoteId_rc++;
            break;
        }
        
        // store
        app_vars.moteId = reply->moteId;

        // blue led: we have joined when getting a moteId
        leds_blue_off();

        // change step
        app_vars.step = STEP_1_LOWPOWER;

    } while(0);
}

void _ntw_getTime_cb(dn_ipmt_getParameter_time_rpt* reply) {
    uint32_t num_asns_to_wait;
    uint32_t num_ticks_to_wait;
    uint8_t  trackIdx;

    // debug
    app_dbg.numcalls_ntw_getTime_cb++;

    do {
        if (reply->RC!=DN_ERR_NONE) {
            app_dbg.numerr_ntw_getTime_rc++;
            break;
        }
        if (reply->upTime==0) {
            break;
        }

        // copy over to local copy for easier debug
        memcpy(app_vars.asn,reply->asn,sizeof(app_vars.asn));

        switch (app_vars.step) {
            case STEP_0_JOINING:
            case STEP_1_LOWPOWER:
            case STEP_2_US:
                // cannot happen
                app_dbg.numerr_ntw_getTime_wrong_step++;
                break;
            case STEP_3_MUSIC_ASN3:
                app_dbg.num_ntw_getTime_STEP_3_ASN3++;
                if ( (app_vars.asn[3]&0x03)==0) {
                    // step 2: I'm at the right ASN[3]
                    // wait for ASN[4] to roll over

                    num_asns_to_wait  = 0xff-app_vars.asn[4];
                    num_ticks_to_wait = num_asns_to_wait*TICKS_PER_SLOT;
                    app_vars.step     = STEP_4_MUSIC_ASN4;
                    NRF_RTC0->CC[0]   = num_ticks_to_wait;
                }
                break;
            case STEP_4_MUSIC_ASN4:
                app_dbg.num_ntw_getTime_STEP_4_ASN4++;
                app_vars.step         = STEP_2_US;
                NRF_RTC0->CC[0]       = RTC0PERIOD_STEP_2_US;
                trackIdx              = app_vars.moteId-2; // the first mote has moteId 2, yet we want trackIdx 0 for it
                if(app_vars.f_play_star_wars){
                    music_play(SONGTITLE_STAR_WARS,trackIdx);
                }
                else if(app_vars.f_play_harry_potter){
                    music_play(SONGTITLE_HARRY_POTTER,trackIdx);
                }
                break;
        }

    } while(0);
}

void _ntw_receive_cb(uint8_t* buf, uint8_t bufLen) {
    
    // debug
    app_dbg.numcalls_ntw_receive_cb++;

    // handle
    switch (buf[0]) {
        case MSGID_CMD_LOWPOWER:
            app_vars.step = STEP_1_LOWPOWER;
            break;
        case MSGID_CMD_ACTIVE:
            app_vars.step = STEP_2_US;
            break;
        case MSGID_CMD_MUSIC_HARRY_POTTER :
            app_vars.f_play_harry_potter = 1;
            app_vars.f_play_star_wars = 0;
            app_vars.step = STEP_3_MUSIC_ASN3;
            break;
        case MSGID_CMD_MUSIC_STAR_WARS :
            app_vars.f_play_star_wars = 1;
            app_vars.f_play_harry_potter = 0;
            app_vars.step = STEP_3_MUSIC_ASN3;
            break;
        default:
            // cannot happen
            app_dbg.numerr_ntw_receive_cb_wrong_msgid++;
            break;
    }
}

//=========================== interrupt handlers ==============================

void RTC0_IRQHandler(void) {
    
    // debug
    app_dbg.numcalls_RTC0_IRQHandler++;
    
    // handle compare[0]
    if (NRF_RTC0->EVENTS_COMPARE[0] == 0x00000001 ) {

        // clear flag
        NRF_RTC0->EVENTS_COMPARE[0]    = 0x00000000;

        // clear COUNTER
        NRF_RTC0->TASKS_CLEAR          = 0x00000001;

        // debug
        app_dbg.numcalls_RTC0_IRQHandler_EVENTS_COMPARE0++;

        // handle
        switch (app_vars.step) {
            case STEP_0_JOINING:
                NRF_RTC0->CC[0]        = RTC0PERIOD_STEP_0_JOINING;
                ntw_getMoteId();
                break;
            case STEP_1_LOWPOWER:
                NRF_RTC0->CC[0]        = RTC0PERIOD_STEP_1_LOWPOWER;
                break;
            case STEP_2_US:
                NRF_RTC0->CC[0]        = RTC0PERIOD_STEP_2_US;
                app_vars.doUsRead      = true;
                break;
            case STEP_3_MUSIC_ASN3:
                NRF_RTC0->CC[0]        = RTC0PERIOD_STEP_3_MUSIC_ASN3;
                ntw_getTime();
                break;
            case STEP_4_MUSIC_ASN4:
                ntw_getTime();
                break;
            default:
                // cannot happen
                app_dbg.numerr_RTC0_IRQHandler_wrong_step++;
                break;
        }
    }
}
