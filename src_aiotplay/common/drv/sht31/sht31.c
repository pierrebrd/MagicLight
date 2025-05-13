#include <string.h>
#include "sht31.h"

//=========================== variables =======================================

// measurement command for single-shot data acquisition mode
//     0x2C: clock stretching enabled
//     0x06: high repeatability
const uint8_t SHT31_CMD_MEASURE[]  = {0x2C, 0x06};

//=========================== prototypes ======================================

//=========================== public ==========================================

void sht31_init(void) {
    i2c_init();
}

bool sht31_readTempHumidity(uint16_t* temperature_raw, uint16_t* humidity_raw) {
    uint8_t data[6];
    bool    rc;
    bool    returnVal;

    returnVal = false; // by default, read did NOT succeed
    do {

        // send command to measure temperature
        rc = i2c_send(SHT31_I2C_ADDR, (uint8_t*)&SHT31_CMD_MEASURE, sizeof(SHT31_CMD_MEASURE));
        if (rc==false) {
            break;
        }

        // read temperature data
        rc = i2c_read(SHT31_I2C_ADDR, data, sizeof(data));
        if (rc==false) {
            break;
        }
    
        // if I get here, accessing the SHT31 is a success

        returnVal            = true;
        *temperature_raw     = (data[0]<<8) | data[1];
        *humidity_raw        = (data[3]<<8) | data[4];
    } while(0);

    return returnVal;
}

//=========================== private =========================================

//=========================== interrupt handlers ==============================
