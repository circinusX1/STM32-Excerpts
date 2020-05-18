#ifndef HTU21_H
#define HTU21_H

#include "i2c.h"
#include "sensors.h"

#define HTU21D_ADDRESS 0x40

int HTU21_init( ThpSensors* ps);
struct Thp* HTU21_read( ThpSensors* ps);
struct Thp*  HTU21_Calibrate( ThpSensors* ps);

#endif
