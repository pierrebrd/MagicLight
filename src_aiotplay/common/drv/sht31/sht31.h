#ifndef __SHT31_H
#define __SHT31_H

#include "i2c.h"

//=========================== define ==========================================

// default SHT31 address
#define SHT31_I2C_ADDR       0X44 

//=========================== typedef =========================================

//=========================== variables =======================================

//=========================== prototypes ======================================

void sht31_init(void);
bool sht31_readTempHumidity(uint16_t* temp_raw, uint16_t* humidity_raw);

#endif
