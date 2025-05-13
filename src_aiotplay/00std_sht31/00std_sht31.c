#include <stdio.h>
#include "nrf52833.h"
#include "busywait.h"

/*
This application establishes a self-contained I2C driver designed to fetch the raw 
temperature and humidity readings acquired from an SHT31 sensor in a periodic manner. 
It uses the single-shot data acquisition mode from the SHT31 for each reading. 
The supply voltage is set at 3.6V, whilst the SCL and SDA pins are connected to 
P0.03 and P0.02, respectively, on the nRF52833.

sources:
    https://github.com/DotBots/DotBot-firmware/blob/main/bsp/nrf/i2c.c
    https://sensirion.com/media/documents/213E6A3B/63A5A569/Datasheet_SHT3x_DIS.pdf

I2C:
    config:
        7-bit addressing plus 0 as the write bit
    pins:
        freq: 100000
        SCL==micro: B13 (header)==P0.03 (nRF)
        SDA==micro: A12 (header)==P0.02 (nRF)
*/

// Default SHT31 address
#define SHT31_ADDR 0X44 

// Measurement Commands for Single Shot Data Acquisition Mode
// 0x2C: Clock stretching enabled
// 0x0D: Medium repeatability (proportional to measurement duration)
uint8_t CMD_MEASURE[]      = {0x2C, 0x0D};

void i2c_init(void) {
   //  3           2            1           0
    // 1098 7654 3210 9876 5432 1098 7654 3210
    // .... .... .... .... .... .... .... ...A A: DIR:   0=Input
    // .... .... .... .... .... .... .... ..B. B: INPUT: 1=Disconnect
    // .... .... .... .... .... .... .... CC.. C: PULL:  0=Disabled
    // .... .... .... .... .... .DDD .... .... D: DRIVE: 6=S0D1
    // .... .... .... ..EE .... .... .... .... E: SENSE: 0=Disabled
    // xxxx xxxx xxxx xx00 xxxx x110 xxxx 0010 
    //    0    0    0    0    0    6    0    2 0x00000602
    NRF_P0->PIN_CNF[3]           = 0x00000602; // SCL (P0.03)
    NRF_P0->PIN_CNF[2]           = 0x00000602; // SDA (P0.02)

    //  3           2            1           0
    // 1098 7654 3210 9876 5432 1098 7654 3210
    // .... .... .... .... .... .... .... AAAA A: ENABLE: 5=Enabled
    // xxxx xxxx xxxx xxxx xxxx xxxx xxxx 0101 
    //    0    0    0    0    0    0    0    5 0x00000005
    NRF_TWI0->ENABLE              = 0x00000005;

    //  3           2            1           0
    // 1098 7654 3210 9876 5432 1098 7654 3210
    // .... .... .... .... .... .... ...A AAAA A: PIN:    03 (P0.03)
    // .... .... .... .... .... .... ..B. .... B: PORT:    0 (P0.03)
    // C... .... .... .... .... .... .... .... C: CONNECT: 0=Connected
    // 0xxx xxxx xxxx xxxx xxxx xxxx xx00 0011 
    //    0    0    0    0    0    0    0    3 0x00000003
    NRF_TWI0->PSEL.SCL            = 0x00000003;

    //  3           2            1           0
    // 1098 7654 3210 9876 5432 1098 7654 3210
    // .... .... .... .... .... .... ...A AAAA A: PIN:    02 (P0.02)
    // .... .... .... .... .... .... ..B. .... B: PORT:    0 (P0.02)
    // C... .... .... .... .... .... .... .... C: CONNECT: 0=Connected
    // 0xxx xxxx xxxx xxxx xxxx xxxx xx00 0010
    //    0    0    0    0    0    0    0    2 0x00000002
    NRF_TWI0->PSEL.SDA            = 0x00000002;

    //  3           2            1           0
    // 1098 7654 3210 9876 5432 1098 7654 3210
    // AAAA AAAA AAAA AAAA AAAA AAAA AAAA AAAA A: FREQUENCY: 0x01980000==K100==100 kbps
    NRF_TWI0->FREQUENCY           = 0x01980000;
}

void i2c_send(uint8_t addr, uint8_t* buf, uint8_t buflen) {
    uint8_t i = 0;

    NRF_TWI0->ADDRESS               = addr;
    NRF_TWI0->TXD                   = buf[i];
    NRF_TWI0->EVENTS_TXDSENT        = 0;
    NRF_TWI0->TASKS_STARTTX         = 1;
    i++;

    while(i<buflen) {
        while(NRF_TWI0->EVENTS_TXDSENT == 0);
        NRF_TWI0->EVENTS_TXDSENT    = 0;
        NRF_TWI0->TXD               = buf[i];
        i++;
    }

    while(NRF_TWI0->EVENTS_TXDSENT == 0);
    NRF_TWI0->TASKS_STOP            = 1;
}


void i2c_read(uint8_t addr, uint8_t* buf, uint8_t buflen) {
    uint8_t i;
    NRF_TWI0->ADDRESS               = addr;

    NRF_TWI0->TASKS_STARTRX         = 1;
    for (i = 0; i < buflen-1; i++) {
        while (NRF_TWI0->EVENTS_RXDREADY == 0);
        NRF_TWI0->EVENTS_RXDREADY   = 0;
        buf[i]                      = NRF_TWI0->RXD;
    }

    // trigger STOP task before extracting last byte
    NRF_TWI0->TASKS_STOP            = 1;
    while (NRF_TWI0->EVENTS_RXDREADY == 0);
    NRF_TWI0->EVENTS_RXDREADY       = 0;
    buf[i]                          = NRF_TWI0->RXD;

    NRF_TWI0->EVENTS_STOPPED        = 0;
    while (NRF_TWI0->EVENTS_STOPPED == 0);
}

void SHT31_readTempHumidity(uint16_t* temp_raw, uint16_t* humidity_raw) {
    uint8_t data[6];

    // send command to measure temperature
    i2c_send(SHT31_ADDR, &CMD_MEASURE, sizeof(CMD_MEASURE));

    // read temperature data
    i2c_read(SHT31_ADDR, data, sizeof(data));
    *temp_raw = data[0] << 8 | data[1];
    *humidity_raw = data[3] << 8 | data[4];
}

int main(void) {
    uint16_t temperature;
    uint16_t humidity;

    i2c_init();

    while (1) {
        SHT31_readTempHumidity(&temperature, &humidity);
        printf("Temperature: %X\n", temperature);
        printf("Humidity: %X\n", humidity);
        busywait_approx_1s();
    }
}
