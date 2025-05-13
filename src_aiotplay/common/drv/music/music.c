#include <string.h>
#include <stdbool.h>
#include "music.h"
#include "pwm.h"
#include "leds.h"
#include "fet.h"

//=========================== variables =======================================

typedef struct {
    uint32_t noteIdx;
    note_t*  notes;
    uint32_t numnotes;
    uint16_t speed;
    uint8_t  ledctr;
    bool     inhibit;
} music_vars_t;

music_vars_t music_vars;

typedef struct {
    bool     song_playing;
    note_t*  last_note;
    uint32_t num_music_init;
    uint32_t num_music_play;
} music_dbg_t;

music_dbg_t music_dbg;

//=========================== prototypes ======================================

static void _start_song(uint16_t speed);
static void _play_cur_note(void);
static void _end_song(void);
static void _leds_off(void);
static void _leds_shift(void);
static void _leds_applyctr(void);

//=========================== public ==========================================

void music_init(void) {

    // clear variables
    memset(&music_vars,0x00,sizeof(music_vars_t));
    memset(&music_dbg, 0x00,sizeof(music_dbg_t) );

    // by default, play the music
    music_vars.inhibit = false;

    // debug
    music_dbg.num_music_init++;

    // pwm
    pwm_init();

    // leds
    leds_init();

    // fet
    fet_init();

    //=== RTC2
    
    // 1098 7654 3210 9876 5432 1098 7654 3210
    // xxxx xxxx xxxx FEDC xxxx xxxx xxxx xxBA (C=compare 0)
    // 0000 0000 0000 0001 0000 0000 0000 0000 
    //    0    0    0    1    0    0    0    0 0x00010000
    NRF_RTC2->EVTENSET                 = 0x00010000;       // enable compare 0 event routing
    NRF_RTC2->INTENSET                 = 0x00010000;       // enable compare 0 interrupts

    // enable interrupts
    NVIC_SetPriority(RTC2_IRQn, 1);
    NVIC_ClearPendingIRQ(RTC2_IRQn);
    NVIC_EnableIRQ(RTC2_IRQn);
}

void music_inhibit(bool inhibit) {
    music_vars.inhibit = inhibit;
}

void music_play(songtitle_t songtitle, uint8_t trackIdx) {

    // debug
    music_dbg.num_music_play++;

    switch (songtitle) {
        case SONGTITLE_STAR_WARS:
            trackIdx = trackIdx%22; // roll over if more motes
            switch (trackIdx) {
                case 0:
                    music_vars.notes      = (note_t*)SONGNOTES_Star_Wars_Medley_TRACK_0;
                    music_vars.numnotes   = sizeof(  SONGNOTES_Star_Wars_Medley_TRACK_0)/sizeof(note_t);
                    break;
                case 1:
                    music_vars.notes      = (note_t*)SONGNOTES_Star_Wars_Medley_TRACK_1;
                    music_vars.numnotes   = sizeof(  SONGNOTES_Star_Wars_Medley_TRACK_1)/sizeof(note_t);
                    break;
                case 2:
                    music_vars.notes      = (note_t*)SONGNOTES_Star_Wars_Medley_TRACK_2;
                    music_vars.numnotes   = sizeof(  SONGNOTES_Star_Wars_Medley_TRACK_2)/sizeof(note_t);
                    break;
                case 3:
                    music_vars.notes      = (note_t*)SONGNOTES_Star_Wars_Medley_TRACK_3;
                    music_vars.numnotes   = sizeof(  SONGNOTES_Star_Wars_Medley_TRACK_3)/sizeof(note_t);
                    break;
                case 4:
                    music_vars.notes      = (note_t*)SONGNOTES_Star_Wars_Medley_TRACK_4;
                    music_vars.numnotes   = sizeof(  SONGNOTES_Star_Wars_Medley_TRACK_4)/sizeof(note_t);
                    break;
                case 5:
                    music_vars.notes      = (note_t*)SONGNOTES_Star_Wars_Medley_TRACK_5;
                    music_vars.numnotes   = sizeof(  SONGNOTES_Star_Wars_Medley_TRACK_5)/sizeof(note_t);
                    break;
                case 6:
                    music_vars.notes      = (note_t*)SONGNOTES_Star_Wars_Medley_TRACK_6;
                    music_vars.numnotes   = sizeof(  SONGNOTES_Star_Wars_Medley_TRACK_6)/sizeof(note_t);
                    break;
                case 7:
                    music_vars.notes      = (note_t*)SONGNOTES_Star_Wars_Medley_TRACK_7;
                    music_vars.numnotes   = sizeof(  SONGNOTES_Star_Wars_Medley_TRACK_7)/sizeof(note_t);
                    break;
                case 8:
                    music_vars.notes      = (note_t*)SONGNOTES_Star_Wars_Medley_TRACK_8;
                    music_vars.numnotes   = sizeof(  SONGNOTES_Star_Wars_Medley_TRACK_8)/sizeof(note_t);
                    break;
                case 9:
                    music_vars.notes      = (note_t*)SONGNOTES_Star_Wars_Medley_TRACK_9;
                    music_vars.numnotes   = sizeof(  SONGNOTES_Star_Wars_Medley_TRACK_9)/sizeof(note_t);
                    break;
                case 10:
                    music_vars.notes      = (note_t*)SONGNOTES_Star_Wars_Medley_TRACK_10;
                    music_vars.numnotes   = sizeof(  SONGNOTES_Star_Wars_Medley_TRACK_10)/sizeof(note_t);
                    break;
                case 11:
                    music_vars.notes      = (note_t*)SONGNOTES_Star_Wars_Medley_TRACK_11;
                    music_vars.numnotes   = sizeof(  SONGNOTES_Star_Wars_Medley_TRACK_11)/sizeof(note_t);
                    break;
                case 12:
                    music_vars.notes      = (note_t*)SONGNOTES_Star_Wars_Medley_TRACK_12;
                    music_vars.numnotes   = sizeof(  SONGNOTES_Star_Wars_Medley_TRACK_12)/sizeof(note_t);
                    break;
                case 13:
                    music_vars.notes      = (note_t*)SONGNOTES_Star_Wars_Medley_TRACK_13;
                    music_vars.numnotes   = sizeof(  SONGNOTES_Star_Wars_Medley_TRACK_13)/sizeof(note_t);
                    break;
                case 14:
                    music_vars.notes      = (note_t*)SONGNOTES_Star_Wars_Medley_TRACK_14;
                    music_vars.numnotes   = sizeof(  SONGNOTES_Star_Wars_Medley_TRACK_14)/sizeof(note_t);
                    break;
                case 15:
                    music_vars.notes      = (note_t*)SONGNOTES_Star_Wars_Medley_TRACK_15;
                    music_vars.numnotes   = sizeof(  SONGNOTES_Star_Wars_Medley_TRACK_15)/sizeof(note_t);
                    break;
                case 16:
                    music_vars.notes      = (note_t*)SONGNOTES_Star_Wars_Medley_TRACK_16;
                    music_vars.numnotes   = sizeof(  SONGNOTES_Star_Wars_Medley_TRACK_16)/sizeof(note_t);
                    break;
                case 17:
                    music_vars.notes      = (note_t*)SONGNOTES_Star_Wars_Medley_TRACK_17;
                    music_vars.numnotes   = sizeof(  SONGNOTES_Star_Wars_Medley_TRACK_17)/sizeof(note_t);
                    break;
                case 18:
                    music_vars.notes      = (note_t*)SONGNOTES_Star_Wars_Medley_TRACK_18;
                    music_vars.numnotes   = sizeof(  SONGNOTES_Star_Wars_Medley_TRACK_18)/sizeof(note_t);
                    break;
                case 19:
                    music_vars.notes      = (note_t*)SONGNOTES_Star_Wars_Medley_TRACK_19;
                    music_vars.numnotes   = sizeof(  SONGNOTES_Star_Wars_Medley_TRACK_19)/sizeof(note_t);
                    break;
                case 20:
                    music_vars.notes      = (note_t*)SONGNOTES_Star_Wars_Medley_TRACK_20;
                    music_vars.numnotes   = sizeof(  SONGNOTES_Star_Wars_Medley_TRACK_20)/sizeof(note_t);
                    break;
                case 21:
                default:
                    music_vars.notes      = (note_t*)SONGNOTES_Star_Wars_Medley_TRACK_21;
                    music_vars.numnotes   = sizeof(  SONGNOTES_Star_Wars_Medley_TRACK_21)/sizeof(note_t);
                    break;
            }
            music_vars.speed              = SONGSPEED[SONGTITLE_STAR_WARS];
            break;
        case SONGTITLE_HARRY_POTTER:
            trackIdx = trackIdx%9; // roll over if more motes
            switch (trackIdx) {
                case 0:
                    music_vars.notes      = (note_t*)SONGNOTES_harry_potter_TRACK_0;
                    music_vars.numnotes   = sizeof(  SONGNOTES_harry_potter_TRACK_0)/sizeof(note_t);
                    break;
                case 1:
                    music_vars.notes      = (note_t*)SONGNOTES_harry_potter_TRACK_1;
                    music_vars.numnotes   = sizeof(  SONGNOTES_harry_potter_TRACK_1)/sizeof(note_t);
                    break;
                case 2:
                    music_vars.notes      = (note_t*)SONGNOTES_harry_potter_TRACK_2;
                    music_vars.numnotes   = sizeof(  SONGNOTES_harry_potter_TRACK_2)/sizeof(note_t);
                    break;
                case 3:
                    music_vars.notes      = (note_t*)SONGNOTES_harry_potter_TRACK_3;
                    music_vars.numnotes   = sizeof(  SONGNOTES_harry_potter_TRACK_3)/sizeof(note_t);
                    break;
                case 4:
                    music_vars.notes      = (note_t*)SONGNOTES_harry_potter_TRACK_4;
                    music_vars.numnotes   = sizeof(  SONGNOTES_harry_potter_TRACK_4)/sizeof(note_t);
                    break;
                case 5:
                    music_vars.notes      = (note_t*)SONGNOTES_harry_potter_TRACK_5;
                    music_vars.numnotes   = sizeof(  SONGNOTES_harry_potter_TRACK_5)/sizeof(note_t);
                    break;
                case 6:
                    music_vars.notes      = (note_t*)SONGNOTES_harry_potter_TRACK_6;
                    music_vars.numnotes   = sizeof(  SONGNOTES_harry_potter_TRACK_6)/sizeof(note_t);
                    break;
                case 7:
                    music_vars.notes      = (note_t*)SONGNOTES_harry_potter_TRACK_7;
                    music_vars.numnotes   = sizeof(  SONGNOTES_harry_potter_TRACK_7)/sizeof(note_t);
                    break;
                case 8:
                default:
                    music_vars.notes      = (note_t*)SONGNOTES_harry_potter_TRACK_8;
                    music_vars.numnotes   = sizeof(  SONGNOTES_harry_potter_TRACK_8)/sizeof(note_t);
                    break;
            }
            music_vars.speed              = SONGSPEED[SONGTITLE_HARRY_POTTER];
            break;
      case SONGTITLE_SYNCTEST:
            trackIdx = trackIdx%2; // roll over if more motes
            switch (trackIdx) {
                case 0:
                    music_vars.notes      = (note_t*)SONGNOTES_synctest_TRACK_0;
                    music_vars.numnotes   = sizeof(  SONGNOTES_synctest_TRACK_0)/sizeof(note_t);
                    break;
                case 1:
                    music_vars.notes      = (note_t*)SONGNOTES_synctest_TRACK_1;
                    music_vars.numnotes   = sizeof(  SONGNOTES_synctest_TRACK_1)/sizeof(note_t);
                    break;
            }
            music_vars.speed              = SONGSPEED[SONGTITLE_SYNCTEST];
            break;
            break;
    }

    // play first note
    _start_song(music_vars.speed);
    _play_cur_note();
}

//=========================== private =========================================

static void _start_song(uint16_t speed) {
    _leds_off();
    if (music_vars.inhibit==false) {
        fet_on();
    }
    music_dbg.song_playing   = true;
    music_vars.noteIdx       = 0;
    NRF_RTC2->PRESCALER      = speed;
    NRF_RTC2->TASKS_START    = 0x00000001;
}

static void _play_cur_note(void) {
    _leds_shift();
    music_dbg.last_note      = &music_vars.notes[music_vars.noteIdx];
    pwm_setperiod(music_vars.notes[music_vars.noteIdx].val);
    NRF_RTC2->CC[0]          = music_vars.notes[music_vars.noteIdx].duration;
}

static void _end_song(void) {
    _leds_off();
    fet_off();
    NRF_RTC2->TASKS_STOP     = 0x00000001;
    pwm_stop();
    music_dbg.song_playing   = false;
}

static void _leds_off(void) {
    music_vars.ledctr = 0;
    _leds_applyctr();
}

static void _leds_shift(void) {
    music_vars.ledctr++;
    _leds_applyctr();
}

static void _leds_applyctr(void) {
    
    // green
    if (music_vars.ledctr & (1<<0)) {
        leds_green_on();
    } else {
        leds_green_off();
    }

    // red
    if (music_vars.ledctr & (1<<1)) {
        leds_red_on();
    } else {
        leds_red_off();
    }

    // blue
    if (music_vars.ledctr & (1<<2)) {
        leds_blue_on();
    } else {
        leds_blue_off();
    }

    // white
    if (music_vars.ledctr & (1<<3)) {
        leds_white_on();
    } else {
        leds_white_off();
    }
}

//=========================== interrupt handlers ==============================

void RTC2_IRQHandler(void) {

    // handle compare[0]
    if (NRF_RTC2->EVENTS_COMPARE[0] == 0x00000001 ) {

        // clear flag
        NRF_RTC2->EVENTS_COMPARE[0]    = 0x00000000;

        // clear COUNTER
        NRF_RTC2->TASKS_CLEAR          = 0x00000001;

        // bump
        music_vars.noteIdx++;

        if (music_vars.noteIdx<music_vars.numnotes) {
            // play
            _play_cur_note();
        } else {
            // end
            _end_song();
        }
    }
}