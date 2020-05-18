#ifndef TCA9648_H_
#define TCA9648_H_

#include "i2c.h"


#define TCA9648_ADDRESS                (0x70)


int TCA9648_init(int spinum);
bool TCA9648_select(uint8_t pin);
uint8_t TCA9648_selected();

#endif /* TCA9648_H_ */
