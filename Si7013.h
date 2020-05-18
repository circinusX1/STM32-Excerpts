#ifndef SI7013_H
#define SI7013_H

#include "i2c.h"
#include "sensors.h"

#define SI7013_ADDRESS 0x41

int SI7013_init( ThpSensors* ps);
struct Thp* SI7013_read( ThpSensors* ps);
struct Thp*  SI7013_Calibrate( ThpSensors* ps);

#endif // SI7013_H
