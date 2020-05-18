#ifndef SHT35_H
#define SHT35_H

#include <stdbool.h>
#include <stdint.h>
#include "i2c.h"
#include "sensors.h"

#define SHT35X_ADDRESS                (0x44)

int SHT35xx_init(struct ThpSensors* ps);
struct Thp* SHT35xx_read(struct ThpSensors* ps);
struct Thp*  SHT35xx_Calibrate(struct ThpSensors* ps);
void SHT35xx_Heat(struct ThpSensors* ps, bool h);


#define ARRAY_SIZE(a) sizeof(a)/sizeof(a[0])
#endif // SHT35_H
