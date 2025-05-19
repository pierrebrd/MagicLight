#include <string.h>
#include <stdbool.h>
#include "board.h"
#include "periodictimer.h"
#include "ntw.h"
#include "leds.h"
#include "gpio.h"
#include "busywait.h"
#include <stdio.h>

//=========================== defines =========================================


#define BLINK_PERIOD_S 1

//=========================== typedef =========================================

typedef enum
{
    Blue,
    Red,
    Green,
    Magenta,
    Cyan,
    Yellow,
    White,
} light_color;

//=========================== variables =======================================

typedef struct
{
    bool      isAlarmActivated;
    bool      someoneDetected;
    uint8_t   lightColor;
    bool      isAlarmRunning;
} app_vars_t;

app_vars_t app_vars;

typedef struct
{
    uint32_t       numcalls_periodtimer_cb;
    uint32_t       numcalls_periodtimer_cb_success;
    uint32_t       numcalls_periodtimer_cb_fail;
    uint32_t       numcalls_ntw_getMoteId_cb;
    uint32_t       numcalls_ntw_getTime_cb;
    uint32_t       numcalls_ntw_receive_cb;
} app_dbg_t;

app_dbg_t app_dbg;





//=========================== prototypes ======================================


void _ntw_getMoteId_cb(dn_ipmt_getParameter_moteId_rpt* reply);
void _ntw_getTime_cb(dn_ipmt_getParameter_time_rpt* reply);
void _ntw_receive_cb(uint8_t* buf, uint8_t bufLen);


//=========================== functions =======================================

void normal_behavior(void) {
    if (app_vars.someoneDetected == FALSE) {
        leds_red_off();
        leds_green_off();
        leds_blue_off();
    }
    else {
        volatile uint32_t counter;
        switch (app_vars.lightColor) {
        case Blue:
            leds_red_off();
            leds_green_off();
            leds_blue_on();
            break;
        case Red:
            leds_red_on();
            leds_green_off();
            leds_blue_off();
            break;
        case Green:
            leds_red_off();
            leds_green_on();
            leds_blue_off();
            break;
        case Magenta:
            leds_green_off();
            leds_blue_off();
            leds_red_off();
            for (counter = 0; counter < 125; counter++) {
                leds_blue_on();
                busywait_approx_4ms();
                leds_blue_off();
                leds_red_on();
                busywait_approx_4ms();
                leds_red_off();
            }
            break;
        case Cyan:
            leds_green_off();
            leds_blue_off();
            leds_red_off();
            for (counter = 0; counter < 125; counter++) {
                leds_blue_on();
                busywait_approx_4ms();
                leds_blue_off();
                leds_green_on();
                busywait_approx_4ms();
                leds_green_off();
            }
            break;
        case Yellow:
            leds_green_off();
            leds_blue_off();
            leds_red_off();
            for (counter = 0; counter < 125; counter++) {
                leds_green_on();
                busywait_approx_4ms();
                leds_green_off();
                leds_red_on();
                busywait_approx_4ms();
                leds_red_off();
            }
            break;
        case White:
            leds_green_off();
            leds_blue_off();
            leds_red_off();
            for (counter = 0; counter < 83; counter++) {
                leds_green_on();
                busywait_approx_4ms();
                leds_green_off();
                leds_red_on();
                busywait_approx_4ms();
                leds_red_off();
                leds_blue_on();
                busywait_approx_4ms();
                leds_blue_off();
            }
            break;
        }
    }
}

//=========================== main ============================================

int main(void) {

    // initialize variables
    memset(&app_vars, 0x00, sizeof(app_vars));
    memset(&app_dbg, 0x00, sizeof(app_dbg));

    // initialize bsp
    board_init();

    // initialize GPIO (PIR)
    gpio_init();

    // initialize P17 as the PIR pin
    gpio_P017_input_init(NULL);

    // initialize leds
    leds_init();

    // initialize ntw
    ntw_init(
        NULL,                // ntw_joining_cb
        _ntw_getMoteId_cb,   // ntw_getMoteId_cb
        _ntw_getTime_cb,     // ntw_getTime_cb
        _ntw_receive_cb      // ntw_receive_cb
    );


    while (1) {

        // We start by looking at the detector input
        if (NRF_P0->IN & (1 << 17)) {
            // DP2 high -- movement detected
            app_vars.someoneDetected = TRUE;
            printf("HIGH\n");
        }
        else {
            // no movement -- DP2 low
            app_vars.someoneDetected = FALSE;
            printf("LOW\n");
        }


        // We change the color accordingly
        if (app_vars.isAlarmActivated == FALSE) {
            normal_behavior();
        }
        else {
            // Alarm activated
            if (app_vars.someoneDetected == TRUE) {
                // Alarm triggered!
                app_vars.isAlarmRunning = TRUE;
                // Send the alarm notification
                uint8_t txBuf[1];
                txBuf[0] = 'T';
                bool    success;
                success = ntw_transmit(txBuf, sizeof(txBuf));
                // debug
                if (success == true) {
                    app_dbg.numcalls_periodtimer_cb_success++;
                }
                else {
                    app_dbg.numcalls_periodtimer_cb_fail++;
                }
                leds_red_off();
                leds_green_off();
                leds_blue_off();
                while (app_vars.isAlarmRunning == TRUE) {
                    // Blink red
                    leds_red_on();
                    busywait_approx_500ms();
                    leds_red_off();
                    busywait_approx_500ms();
                }
            }


        }
        // Wait a bit
        //busywait_approx_125ms();

    }
}

//=========================== private =========================================


void _ntw_getMoteId_cb(dn_ipmt_getParameter_moteId_rpt* reply) {

    // debug
    app_dbg.numcalls_ntw_getMoteId_cb++;
}

void _ntw_getTime_cb(dn_ipmt_getParameter_time_rpt* reply) {

    // debug
    app_dbg.numcalls_ntw_getTime_cb++;
}

void _ntw_receive_cb(uint8_t* buf, uint8_t bufLen) {
    //printf("%d\n",bufLen);
    //printf("%s",buf);
    //printf("\n");

    app_dbg.numcalls_ntw_receive_cb++;
    switch (buf[0]) {
    case 'A':
        //printf("%s \n", buf);
        app_vars.isAlarmActivated = true;
        break;
    case 'D':
        app_vars.isAlarmActivated = false;
        app_vars.isAlarmRunning = false;
        break;
    case 'B':
        app_vars.lightColor = Blue;
        break;
    case 'R':
        app_vars.lightColor = Red;
        break;
    case 'G':
        app_vars.lightColor = Green;
        break;
    case 'M':
        app_vars.lightColor = Magenta;
        break;
    case 'C':
        app_vars.lightColor = Cyan;
        break;
    case 'Y':
        app_vars.lightColor = Yellow;
        break;
    case 'W':
        app_vars.lightColor = White;
        break;
    }

    printf("DETECTOR_MODE: %s\n", app_vars.isAlarmActivated ? "true" : "false");
    printf("Light Color %d\n", app_vars.lightColor);
}
