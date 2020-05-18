#include "htu21.h"

/** Default I2C address for the HTU21D. */
//#define HTU21D_ADDRESS         (0x40)

/** Read temperature register. */
#define HTU21DF_READTEMP        (0xE3)

/** Read humidity register. */
#define HTU21DF_READHUM         (0xE5)

/** Write register command. */
#define HTU21DF_WRITEREG        (0xE6)

/** Read register command. */
#define HTU21DF_READREG         (0xE7)

/** Reset command. */
#define HTU21DF_RESET           (0xFE)


int HTU21_init( ThpSensors* ps)
{
    ps->initlised=true;
    i2c_write8(ps->i2cnum,ps->addr,0xFF,HTU21DF_RESET);
    HAL_Delay(0xf);
    i2c_write8(ps->i2cnum,ps->addr,0xFF,HTU21DF_READREG);
    uint8_t ok =  i2c_read8(ps->i2cnum,ps->addr,0xFF);
    return ok == 0x2;
}

 struct Thp* HTU21_read( ThpSensors* pps)
{
    pps->sensdata.h = 0;
    pps->sensdata.p = 0;
    pps->sensdata.t = 0;
    union {
        uint8_t d[4];
        uint32_t u32;
    }udata;

    i2c_write8(pps->i2cnum,pps->addr,0xFF,HTU21DF_READTEMP);
    HAL_Delay(32);
    udata.u32 = i2c_read24(pps->i2cnum,pps->addr,NO_REGISTER);
    if(udata.d[0]==0)
        return 0;
    uint16_t temp        = (uint16_t)((udata.d[0] << 8) + udata.d[1]);
    float    temperature = temp;
    temperature  *= 175.72f;
    temperature  /= 65536.0f;
    temperature -= 46.85f;
    pps->sensdata.t = (int32_t)(temperature*100.0f);

    i2c_write8(pps->i2cnum,pps->addr,0xFF,HTU21DF_READHUM);
    HAL_Delay(32);
    udata.u32=0;
    udata.u32 = i2c_read24(pps->i2cnum,pps->addr,0xFF);
    if(udata.d[0]==0)
        return &pps->sensdata;

    uint16_t hum      = (udata.d[0] << 8) + udata.d[1];
    float    humidity = hum;
    humidity        *= 125;
    humidity        /= 65536;
    humidity        -= 6;
    pps->sensdata.h = humidity*100.0f;
    return &pps->sensdata;
}

struct Thp*  HTU21_Calibrate(ThpSensors* ps)
{
    return 0;
}
