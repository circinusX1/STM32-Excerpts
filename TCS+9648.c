#include "tca9648.h"


static int SpiNum=2; 
static uint8_t Ouput=0;

int TCA9648_init(int spinum)
{
	SpiNum=spinum;
	return 0;
}

bool TCA9648_select(uint8_t pin)
{
	  Ouput=pin;
    return i2c_write8(SpiNum, TCA9648_ADDRESS, 0x04, 1 << (pin-1));
}

uint8_t TCA9648_selected()
{
	  return Ouput;
}
