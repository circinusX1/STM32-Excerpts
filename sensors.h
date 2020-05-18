#ifndef  _SENSORS_H_
#define  _SENSORS_H_


typedef struct Thp
{
    int32_t t;  // temp C * 100
    int32_t h;  // hum in % * 100
    int32_t p;  // presure in mmhg * 100
}Thp;

struct ThpSensors;

typedef struct Thp* (*pReadFoo)(struct ThpSensors*);
typedef int (*pInitFoo)(struct ThpSensors*);

typedef struct ThpSensors
{
/*
	  uint8_t  muxpin;     // mux pin on tca  if is on TCA MUX
    uint8_t  muxaddr;    // mux address
    uint8_t  i2cnum;     // i2c number 2 or 3
*/
    uint8_t  addr;         // device i2c address
    bool     initlised;    // was initialised
    pInitFoo pFnInit;      // ptr to init function
    pReadFoo pFnRead;      // ptr to read function
    pReadFoo pFnCalibrate; // ptr to calibrate function
    struct Thp sensdata;   // sensor last data
} ThpSensors;

#define  s_sleep Hal_Delay

#endif _SENSORS_H_
