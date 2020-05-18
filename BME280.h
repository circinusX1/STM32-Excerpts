#ifndef __BME280_H__
#define __BME280_H__

#include "i2c.h"
#include "sensors.h"

#define BME280_ADDRESS                (0x76)

int BME280_init(struct ThpSensors* ps);
struct Thp* BME280_read(struct ThpSensors* ps);
struct Thp*  BME280_Calibrate(struct ThpSensors* ps);

#endif
