#include "si7013.h"

#define MEASURE_RH_HOLD  0xE5
#define MEASURE_RH_NOHOLD  0xF5
#define MEASURE_TEMP_HOLD  0xE3
#define MEASURE_TEMP_NOHOLD  0xF3
#define MEASURE_ANALOG  0xEE
#define READ_TEMP_FROM_PREV_RH  0xE0
#define RESET  0xFE
#define WRITE_USER_REG1  0xE6
#define READ_USER_REG1  0xE7
#define WRITE_USER_REG2  0x50
#define READ_USER_REG2  0x10
#define WRITE_USER_REG3  0x51
#define READ_USER_REG3  0x11
#define WRITE_THERMISTOR_COEFF  0xC5
#define READ_THERMISTOR_COEFF  0x84
#define READ_ID0  0xFA
#define READ_ID1  0x0F
#define READ_ID2  0xFC
#define READ_ID3  0xC9
//#define READ_FIRMWARE  0x84
#define READ_FIRMWARE  0xB8

static uint8_t _check_crc(uint16_t byval, uint8_t bychk);


int SI7013_init( ThpSensors* ps)
{
    if(ps->initlised==false)
    {
        ps->initlised=true;
        i2c_write8(ps->i2cnum, ps->addr,NO_REGISTER,RESET);
        HAL_Delay(32);
    }
    return 0;
}

struct Thp* SI7013_read( ThpSensors* ps)
{
    union{uint8_t b[4]; uint32_t u32;}   data;

    //i2c_write8(ps->i2cnum, ps->addr,NO_REGISTER,RESET);
    //HAL_Delay(512);
    i2c_write8(ps->i2cnum,ps->addr,NO_REGISTER,MEASURE_TEMP_NOHOLD);
    HAL_Delay(20);
    data.u32=0;
    data.u32 = i2c_read24(ps->i2cnum,ps->addr,NO_REGISTER);

    uint16_t readword = ((uint16_t) data.b[0] << 8) | (uint16_t) data.b[1];

    //if(_check_crc(readword, data.b[2]) != 0)
    //    return 0;
    readword &= 0xFFFC; //Zero out the status bits but keep them in place

    double fvalue = readword / (double)65536; //2^16 = 65536
    double realVal = -46.85 + (175.72 * fvalue); //From page 14
    ps->sensdata.t = (int)(realVal * SCALE_UP);

    i2c_write8(ps->i2cnum,ps->addr,NO_REGISTER,MEASURE_RH_NOHOLD);
    HAL_Delay(20);
    data.u32=0;
    data.u32 = i2c_read24(ps->i2cnum,ps->addr,NO_REGISTER);
    readword = ((uint16_t) data.b[0] << 8) | (uint16_t) data.b[1];

    //if(_check_crc(readword, data.b[2]) != 0)
    //    return 0;
    readword &= 0xFFFC; //Zero out the status bits but keep them in place

    fvalue = readword / (double)65536; //2^16 = 65536
    realVal = -6 + (125 * fvalue); //From page 14
    ps->sensdata.h = (int)(realVal * SCALE_UP);
    return &ps->sensdata;
}

struct Thp*  SI7013_Calibrate( ThpSensors* ps)
{
    (void)ps;
    return 0;
}


#define SHIFTED_DIVISOR 0x988000
uint8_t _check_crc(uint16_t byval,
                          uint8_t bychk)
{
    uint32_t remainder = (uint32_t)byval << 8;
    remainder |= bychk;
    uint32_t divsor = (uint32_t)SHIFTED_DIVISOR;

    for (int i = 0 ; i < 16 ; i++)
    {
        if( remainder & (uint32_t)1<<(23 - i) )
            remainder ^= divsor;
        divsor >>= 1;
    }
    return (uint8_t)remainder;
}

