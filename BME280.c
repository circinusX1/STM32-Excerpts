#include <stdbool.h>
#include <math.h>
#include "bme280.h"

static int SpiNum=2;

static void _i2c_write(uint8_t addr, uint8_t *b, int sz)
{
    i2c_writebytes(2, addr, 0, b, sz);
}


static uint8_t _i2c_read(uint8_t addr, uint8_t *b, int sz)
{
    i2c_readbytes(2, addr, NO_REGISTER, b, sz);
    return (uint8_t)sz;
}

static bool _i2c_write8(uint8_t addr, uint16_t  reg, uint8_t  value)
{
	return i2c_write8(SpiNum, addr, reg, value);
}

int BME280_init(struct ThpSensors* ps)
{
	if(ps->initlised==false)
	{
		_i2c_write8(ps->addr, 0xE0, 0xB6); //reset
        HAL_Delay(32);
		ps->initlised=true;
        BME280_read(ps);
	}
	return 0;
}

struct Thp*  BME280_Calibrate(struct ThpSensors* ps)
{
    return 0;
}

/**
this functionw a taken from some GITHUB and adapted
*/
struct Thp* BME280_read(struct ThpSensors* s)
{
	uint8_t reg[1] = {0x88};
	_i2c_write(s->addr, reg, 1);
	uint8_t b1[24] = {0};

	if(_i2c_read(s->addr,  b1, 24) != 24)
	{
		return false;
	}

	// Convert the data
	// temp coefficents
	int dig_T1 = (b1[0] + b1[1] * 256);
	int dig_T2 = (b1[2] + b1[3] * 256);
	if(dig_T2 > 32767)
	{
		dig_T2 -= 65536;
	}
	int dig_T3 = (b1[4] + b1[5] * 256);
	if(dig_T3 > 32767)
	{
		dig_T3 -= 65536;
	}

	// pressure coefficents
	int dig_P1 = (b1[6] + b1[7] * 256);
	int dig_P2 = (b1[8] + b1[9] * 256);
	if(dig_P2 > 32767)
	{
		dig_P2 -= 65536;
	}
	int dig_P3 = (b1[10] + b1[11] * 256);
	if(dig_P3 > 32767)
	{
		dig_P3 -= 65536;
	}
	int dig_P4 = (b1[12] + b1[13] * 256);
	if(dig_P4 > 32767)
	{
		dig_P4 -= 65536;
	}
	int dig_P5 = (b1[14] + b1[15] * 256);
	if(dig_P5 > 32767)
	{
		dig_P5 -= 65536;
	}
	int dig_P6 = (b1[16] + b1[17] * 256);
	if(dig_P6 > 32767)
	{
		dig_P6 -= 65536;
	}
	int dig_P7 = (b1[18] + b1[19] * 256);
	if(dig_P7 > 32767)
	{
		dig_P7 -= 65536;
	}
	int dig_P8 = (b1[20] + b1[21] * 256);
	if(dig_P8 > 32767)
	{
		dig_P8 -= 65536;
	}
	int dig_P9 = (b1[22] + b1[23] * 256);
	if(dig_P9 > 32767)
	{
		dig_P9 -= 65536;
	}

	// _i2c_read 1 byte of data from register(0xA1)
	reg[0] = 0xA1;
	_i2c_write(s->addr,  reg, 1);
	uint8_t data[8] = {0};
	_i2c_read(s->addr,  data, 1);
	int dig_H1 = data[0];

	// _i2c_read 7 bytes of data from register(0xE1)
	reg[0] = 0xE1;
	_i2c_write(s->addr,  reg, 1);
	_i2c_read(s->addr,  b1, 7);

	// Convert the data
	// humidity coefficents
	int dig_H2 = (b1[0] + b1[1] * 256);
	if(dig_H2 > 32767)
	{
		dig_H2 -= 65536;
	}
	int dig_H3 = b1[2] & 0xFF ;
	int dig_H4 = (b1[3] * 16 + (b1[4] & 0xF));
	if(dig_H4 > 32767)
	{
		dig_H4 -= 65536;
	}
	int dig_H5 = (b1[4] / 16) + (b1[5] * 16);
	if(dig_H5 > 32767)
	{
		dig_H5 -= 65536;
	}
	int dig_H6 = b1[6];
	if(dig_H6 > 127)
	{
		dig_H6 -= 256;
	}

	// Select control humidity register(0xF2)
	// Humidity over sampling rate = 1(0x01)
	uint8_t config[2] = {0};
	config[0] = 0xF2;
	config[1] = 0x01;
	_i2c_write(s->addr, config, 2);
	// Select control measurement register(0xF4)
	// Normal mode, temp and pressure over sampling rate = 1(0x27)
	config[0] = 0xF4;
	config[1] = 0x27;
	_i2c_write(s->addr,  config, 2);
	// Select config register(0xF5)
	// Stand_by time = 1000 ms(0xA0)
	config[0] = 0xF5;
	config[1] = 0xA0;
	_i2c_write(s->addr,  config, 2);

	// _i2c_read 8 bytes of data from register(0xF7)
	// pressure msb1, pressure msb, pressure lsb, temp msb1, temp msb, temp lsb, humidity lsb, humidity msb
	reg[0] = 0xF7;
	_i2c_write(s->addr, reg, 1);
	_i2c_read(s->addr,  data, 8);

	// Convert pressure and temperature data to 19-bits
	long adc_p = ((long)(data[0] * 65536 + ((long)(data[1] * 256) + (long)(data[2] & 0xF0)))) / 16;
	long adc_t = ((long)(data[3] * 65536 + ((long)(data[4] * 256) + (long)(data[5] & 0xF0)))) / 16;
	// Convert the humidity data
	long adc_h = (data[6] * 256 + data[7]);

	// Temperature offset calculations
    float var1 = (((float)adc_t) / 16384.0f - ((float)dig_T1) / 1024.0f) * ((float)dig_T2);
    float var2 = ((((float)adc_t) / 131072.0f - ((float)dig_T1) / 8192.0f) *
            (((float)adc_t)/131072.0f - ((float)dig_T1)/8192.0f)) * ((float)dig_T3);
	float t_fine = (long)(var1 + var2);
    ///float cTemp = (var1 + var2) / 5120.0f;
    float cTemp = (var1 + var2) / 512.0f;
	float fTemp = cTemp;// * 1.8 + 32;
	//_temp = fTemp;
    s->sensdata.t = (int32_t)fTemp*10;

	// Pressure offset calculations
    var1 = ((float)t_fine / 2.0f) - 64000.0f;
    var2 = var1 * var1 * ((float)dig_P6) / 32768.0f;
    var2 = var2 + var1 * ((float)dig_P5) * 2.0f;
    var2 = (var2 / 4.0f) + (((float)dig_P4) * 65536.0f);
    var1 = (((float) dig_P3) * var1 * var1 / 524288.0f +
                ((float) dig_P2) * var1) / 524288.0f;
    var1 = (1.0f + var1 / 32768.0f) * ((float)dig_P1);
    float p = 1048576.0f - (float)adc_p;
    p = (p - (var2 / 4096.0f)) * 6250.0f / var1;
    var1 = ((float) dig_P9) * p * p / 2147483648.0f;
    var2 = p * ((float) dig_P8) / 32768.0f;
	//float pressure = (p + (var1 + var2 + ((float)dig_P7)) / 16.0) / 100;
    float pressure = (p + (var1 + var2 + ((float)dig_P7)) / 16.0f) / 1.0f;
	//_pres = pressure;

    s->sensdata.p = (int32_t)pressure*100;

	// Humidity offset calculations
    float var_H = (((float)t_fine) - 76800.0f);
    var_H = (adc_h - (dig_H4 * 64.0f + dig_H5 / 16384.0f * var_H)) * (dig_H2 / 65536.0f * (1.0f + dig_H6 / 67108864.0f * var_H * (1.0f + dig_H3 / 67108864.0f * var_H)));
    float humidity = var_H * (1.0f -  dig_H1 * var_H / 524288.0f);
    if(humidity > 100.0f)
	{
        humidity = 100.0f;
	}
    else if(humidity < 0.0f)
	{
        humidity = 0.0f;
	}
    s->sensdata.h = (int32_t)humidity*100;
	return &s->sensdata;
}

