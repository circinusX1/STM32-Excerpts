/**
I think I took this from arduino samples and adapted to STM i2c
*/

#include "sht35.h"


static uint8_t _crc8(const uint8_t *data, int len);
static void _writeCommand(ThpSensors* ps, uint16_t c);


#define SHT31_MEAS_HIGHREP_STRETCH 0x2C06
#define SHT31_MEAS_MEDREP_STRETCH  0x2C0D
#define SHT31_MEAS_LOWREP_STRETCH  0x2C10
#define SHT31_MEAS_HIGHREP         0x2400
#define SHT31_MEAS_MEDREP          0x240B
#define SHT31_MEAS_LOWREP          0x2416
#define SHT31_READSTATUS           0xF32D
#define SHT31_CLEARSTATUS          0x3041
#define SHT31_SOFTRESET            0x30A2
#define SHT31_HEATEREN             0x306D
#define SHT31_HEATERDIS            0x3066

#define SHT31_ABORT               0x3093
#define SHT3X_CMD_LENGTH          2
#define PERSEC_ONE                0x3021
#define PERSEC_ONE2               0x2130
#define FETCH_DATA                0xE000
#define ATR_MODE                  0x2B32
#define SHIT_REGISTER             36

static void _reset(ThpSensors* ps);
static uint16_t _readStatus(ThpSensors* ps);



 int SHT35xx_init(struct ThpSensors* ps)
{
    ps->initlised=true;
    _reset(ps);
    return 0;
}

struct Thp*  SHT35xx_Calibrate(struct ThpSensors* ps)
{
    _reset(ps);
    return 0;
}

static uint16_t _readStatus(struct ThpSensors* ps)
{
    _writeCommand(ps, SHT31_CLEARSTATUS);
    _writeCommand(ps, SHT31_READSTATUS);
    return (uint16_t)i2c_read16(ps->i2cnum, ps->addr, NO_REGISTER);
}

void _reset(struct ThpSensors* ps)
{
    _writeCommand(ps, SHT31_SOFTRESET);
    HAL_Delay(64);
    _writeCommand(ps, SHT31_ABORT);
    HAL_Delay(64);
}

void SHT35xx_Heat(ThpSensors* ps, bool h)
{
    if (h)
        _writeCommand(ps, SHT31_HEATEREN);
    else
        _writeCommand(ps, SHT31_HEATERDIS);
}

struct Thp* SHT35xx_read(ThpSensors* ps)
{
    uint8_t readbuffer[6]={0};
    uint16_t ST, SRH;

    ps->sensdata.t=0;
    ps->sensdata.h=0;
    ps->sensdata.p=0;
    _writeCommand(ps, SHT31_ABORT);
    HAL_Delay(48);
    _writeCommand(ps, SHT31_MEAS_HIGHREP);
    HAL_Delay(48);
    i2c_readbytes(ps->i2cnum, ps->addr,SHIT_REGISTER,readbuffer,6);
    ST = readbuffer[0];
    ST <<= 8;
    ST |= readbuffer[1];

    if (readbuffer[2] != _crc8(readbuffer, 2))
        return 0;

    SRH = readbuffer[3];
    SRH <<= 8;
    SRH |= readbuffer[4];

    if (readbuffer[5] != _crc8(readbuffer+3, 2))
        return &ps->sensdata;

    double stemp = ST;
    stemp *= 175;
    stemp /= 0xFFFF;
    stemp = -45 + stemp;
    ps->sensdata.t = (int)(stemp * SCALE_UP);

    float shum = SRH;
    shum *= 100;
    shum /= 0xFFFF;
    ps->sensdata.h = (int)(shum * SCALE_UP);
    ps->sensdata.p=0;
    return &ps->sensdata;
}

static void _writeCommand(ThpSensors* ps, uint16_t cmd)
{
    i2c_write8(ps->i2cnum, ps->addr, NO_REGISTER, cmd>>8);
    HAL_Delay(12); /*data sheet 1 ms delay, make it 2*/
    i2c_write8(ps->i2cnum, ps->addr, NO_REGISTER, cmd&0x0F);
    HAL_Delay(12);
}

static uint8_t _crc8(const uint8_t *data, int len)
{
    /*
    *
    * CRC-8 formula from page 14 of SHT spec pdf
    *
    * Test data 0xBE, 0xEF should yield 0x92
    *
    * Initialization data 0xFF
    * Polynomial 0x31 (x8 + x5 +x4 +1)
    * Final XOR 0x00
    */

    const uint8_t POLYNOMIAL = (0x31);
    uint8_t crc = (0xFF);

    for ( int j = len; j; --j )
    {
        crc ^= *data++;
        for ( int i = 8; i; --i ) {
            crc = ( crc & 0x80 )
                    ? (crc << 1) ^ POLYNOMIAL
                    : (crc << 1);
        }
    }
    return crc;
}
