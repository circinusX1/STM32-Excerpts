/**
  ******************************************************************************
  * File Name          : I2C.c
  * Description        : This file provides code for the configuration
  *                      of the I2C instances.
  ******************************************************************************
  ** This notice applies to any and all portions of this file
  * that are not between comment pairs USER CODE BEGIN and
  * USER CODE END. Other portions of this file, whether
  * inserted by the user or by software development tools
  * are owned by their respective copyright owners.
  *
  * COPYRIGHT(c) 2018 STMicroelectronics
  *
  * Redistribution and use in source and binary forms, with or without modification,
  * are permitted provided that the following conditions are met:
  *   1. Redistributions of source code must retain the above copyright notice,
  *      this list of conditions and the following disclaimer.
  *   2. Redistributions in binary form must reproduce the above copyright notice,
  *      this list of conditions and the following disclaimer in the documentation
  *      and/or other materials provided with the distribution.
  *   3. Neither the name of STMicroelectronics nor the names of its contributors
  *      may be used to endorse or promote products derived from this software
  *      without specific prior written permission.
  *
  * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
  * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
  * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
  * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
  * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
  * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
  * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
  * CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
  * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
  * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
  *
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/
#include "i2c.h"

#include "gpio.h"

/* USER CODE BEGIN 0 */
bool ERRREPO = true;
typedef union I2cReg
{
    uint8_t     u8;
    uint16_t    u16;
    uint32_t    u32;
    uint8_t     b[4];
}I2cReg;


/* USER CODE END 0 */

I2C_HandleTypeDef hi2c2;

/* I2C2 init function */
void MX_I2C2_Init(void)
{

    hi2c2.Instance = I2C2;
    hi2c2.Init.Timing = 0x2000090E;
    hi2c2.Init.OwnAddress1 = 0;
    hi2c2.Init.AddressingMode = I2C_ADDRESSINGMODE_7BIT;
    hi2c2.Init.DualAddressMode = I2C_DUALADDRESS_DISABLE;
    hi2c2.Init.OwnAddress2 = 0;
    hi2c2.Init.OwnAddress2Masks = I2C_OA2_NOMASK;
    hi2c2.Init.GeneralCallMode = I2C_GENERALCALL_DISABLE;
    hi2c2.Init.NoStretchMode = I2C_NOSTRETCH_DISABLE;
    if (HAL_I2C_Init(&hi2c2) != HAL_OK)
    {
        _Error_Handler(__FILE__, __LINE__);
    }

    /**Configure Analogue filter
    */
    if (HAL_I2CEx_ConfigAnalogFilter(&hi2c2, I2C_ANALOGFILTER_ENABLE) != HAL_OK)
    {
        _Error_Handler(__FILE__, __LINE__);
    }

    /**Configure Digital filter
    */
    if (HAL_I2CEx_ConfigDigitalFilter(&hi2c2, 0) != HAL_OK)
    {
        _Error_Handler(__FILE__, __LINE__);
    }
}

void HAL_I2C_MspInit(I2C_HandleTypeDef* i2cHandle)
{

    GPIO_InitTypeDef GPIO_InitStruct;
    if(i2cHandle->Instance==I2C2)
    {
        /* USER CODE BEGIN I2C2_MspInit 0 */

        /* USER CODE END I2C2_MspInit 0 */

        /**I2C2 GPIO Configuration
    PA9     ------> I2C2_SCL
    PA10     ------> I2C2_SDA
    */
        GPIO_InitStruct.Pin = GPIO_PIN_9|GPIO_PIN_10;
        GPIO_InitStruct.Mode = GPIO_MODE_AF_OD;
        GPIO_InitStruct.Pull = GPIO_PULLUP;
        GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
        GPIO_InitStruct.Alternate = GPIO_AF4_I2C2;
        HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

        /* I2C2 clock enable */
        __HAL_RCC_I2C2_CLK_ENABLE();
        /* USER CODE BEGIN I2C2_MspInit 1 */

        /* USER CODE END I2C2_MspInit 1 */
    }
}

void HAL_I2C_MspDeInit(I2C_HandleTypeDef* i2cHandle)
{

    if(i2cHandle->Instance==I2C2)
    {
        /* USER CODE BEGIN I2C2_MspDeInit 0 */

        /* USER CODE END I2C2_MspDeInit 0 */
        /* Peripheral clock disable */
        __HAL_RCC_I2C2_CLK_DISABLE();

        /**I2C2 GPIO Configuration
    PA9     ------> I2C2_SCL
    PA10     ------> I2C2_SDA
    */
        HAL_GPIO_DeInit(GPIOA, GPIO_PIN_9|GPIO_PIN_10);

        /* USER CODE BEGIN I2C2_MspDeInit 1 */

        /* USER CODE END I2C2_MspDeInit 1 */
    }
} 

/* USER CODE BEGIN 1 */

/**
 * @brief _i2c_read
 * @param i2cnum
 * @param address
 * @param reg register to write into
 * @param size size op reading bytes, max 4
 * @return 0 or value;
 */
static I2cReg _i2c_read(int i2cnum, uint16_t address, uint16_t reg, uint16_t size)
{
    I2C_HandleTypeDef* ph = &hi2c2 ;
    HAL_StatusTypeDef sd = HAL_OK;
    I2cReg ival;

    ival.u32 = 0x00;
    while (HAL_I2C_GetState(ph) != HAL_I2C_STATE_READY);
    if(reg!=NO_REGISTER)
        sd = HAL_I2C_Master_Transmit(ph, address<<1, (uint8_t*)&reg, 1, RW_TOUT);
    if(sd==HAL_OK)
        sd = HAL_I2C_Master_Receive(ph, address<<1, ival.b, size, RW_TOUT);
    if(ERRREPO){
        if(sd != HAL_OK)
            printf("#E i2c read addr:%x[%d] %d bytes\r\n",address, reg, size);
    }
    while (HAL_I2C_GetState(&hi2c2) != HAL_I2C_STATE_READY);
    return ival;
}

static bool _i2c_write(int i2cnum, uint16_t address, uint16_t reg,
                       union I2cReg* val, uint16_t size)
{
    I2C_HandleTypeDef* ph = &hi2c2; //i2cnum==2 ? &hi2c2 : &hi2c3;
    HAL_StatusTypeDef sd;

    //printf("Writing:%d %x[%x] %02x %02x %02x %02x \r\n",size, address,reg, val->b[0], val->b[1], val->b[2], val->b[3] );

    while (HAL_I2C_GetState(ph) != HAL_I2C_STATE_READY);
    //I2C_MEMADD_SIZE_8BIT
    if(reg!=NO_REGISTER)
        sd = HAL_I2C_Mem_Write(ph, ((address<<1)), reg, I2C_MEMADD_SIZE_8BIT, val->b, size, RW_TOUT);
    else
        sd = HAL_I2C_Master_Transmit(ph, address<<1, (uint8_t*)&val->u32, size, RW_TOUT);
    if(ERRREPO){
        if(sd != HAL_OK)
            printf("#E error i2c write addr:%x[%d] %d bytes\r\n", address, reg, size);
    }
    while (HAL_I2C_GetState(ph) != HAL_I2C_STATE_READY);
    return sd==HAL_OK;
}

bool i2c_writebytes(int i2cnum, uint16_t address, uint16_t reg, uint8_t* buff, int sz)
{
    I2C_HandleTypeDef* ph = &hi2c2; //i2cnum==2 ? &hi2c2 : &hi2c3;
    HAL_StatusTypeDef sd;

    //printf("Writing:%d %x[%x] %02x %02x %02x %02x \r\n",size, address,reg, val->b[0], val->b[1], val->b[2], val->b[3] );
    while (HAL_I2C_GetState(ph) != HAL_I2C_STATE_READY);
    sd = HAL_I2C_Master_Transmit(ph, address<<1, buff, sz, RW_TOUT);
    if(ERRREPO){
        if(sd != HAL_OK)
            printf("#E error i2c write addr:%x[%d] %d bytes\r\n", address, reg,sz);
    }
    while (HAL_I2C_GetState(ph) != HAL_I2C_STATE_READY);
    return sd==HAL_OK;
}


bool i2c_readbytes(int i2cnum, uint16_t address, uint16_t reg, uint8_t* buff, int sz)
{
    I2C_HandleTypeDef* ph = &hi2c2; //i2cnum==2 ? &hi2c2 : &hi2c3;
    HAL_StatusTypeDef sd = HAL_OK;

    while (HAL_I2C_GetState(ph) != HAL_I2C_STATE_READY);
    if(reg!=NO_REGISTER){
        sd = HAL_I2C_Mem_Write(ph, ((address<<1)), (uint16_t)reg,
                               I2C_MEMADD_SIZE_8BIT, (uint8_t*)reg, 1, RW_TOUT);
    }
    if(sd==HAL_OK)
        sd = HAL_I2C_Master_Receive(ph, address<<1, buff, sz, RW_TOUT);
    if(ERRREPO){
        if(sd != HAL_OK){
            printf("#E: i2c read addr:%x[%d] %d bytes \r\n", address, reg,sz);
        }
    }
    while (HAL_I2C_GetState(&hi2c2) != HAL_I2C_STATE_READY);
    return sd==HAL_OK;
}

uint8_t i2c_read8(int i2cnum, uint16_t address, uint16_t reg)
{
    return _i2c_read(i2cnum, address, reg, 1).u8&0xFF;
}

uint16_t i2c_read16(int i2cnum, uint16_t address, uint16_t reg)
{
    return _i2c_read(i2cnum, address, reg, 2).u16&0xFFFF;
}


uint32_t i2c_read24(int i2cnum, uint16_t address, uint16_t reg)
{
    return _i2c_read(i2cnum, address, reg, 3).u32;
}


uint32_t i2c_read32(int i2cnum, uint16_t address, uint16_t reg)
{
    return	_i2c_read(i2cnum, address, reg, 4).u32;
}


bool i2c_write8(int i2cnum, uint16_t address, uint16_t reg, const uint8_t val)
{
    I2cReg r={0};
    r.u8 = val;
    return _i2c_write(i2cnum, address, reg, &r, sizeof(uint8_t));
}


bool i2c_write16(int i2cnum, uint16_t address, uint16_t reg, uint16_t val)
{
    I2cReg r={0};
    r.u16 = val;
    return _i2c_write(i2cnum, address, reg, &r, sizeof(uint16_t));
}

bool i2c_write32(int i2cnum, uint16_t address, uint16_t reg, uint32_t val)
{
    I2cReg r;
    r.u32 = val;
    return _i2c_write(i2cnum, address, reg, &r, sizeof(uint32_t));
}


//////////////////////////////////////////////////////////////////////////////////
// PA9      ------> I2C1_SCL
// PA10     ------> I2C1_SDA
void i2c_try_to_reset(void)
{
    I2C1 -> CR1 &= ~(I2C_CR1_PE);                       // disable I2C
    HAL_I2C_MspDeInit(&hi2c1);
    s_sleep(32);
    HAL_I2C_MspInit(&hi2c1);

    HAL_GPIO_WritePin(GPIOA, GPIO_PIN_9, 1);
    HAL_GPIO_WritePin(GPIOA, GPIO_PIN_10, 1);          
    s_sleep(32);
    HAL_GPIO_WritePin(GPIOA, GPIO_PIN_9, 0);
    HAL_GPIO_WritePin(GPIOA, GPIO_PIN_10, 0);          
    s_sleep(32);                                       
    HAL_GPIO_WritePin(GPIOA, GPIO_PIN_9, 1);
    HAL_GPIO_WritePin(GPIOA, GPIO_PIN_10, 1);
    s_sleep(32);

    HAL_I2C_MspDeInit(&hi2c1);
    s_sleep(32);
    HAL_I2C_MspInit(&hi2c1);
    s_sleep(32);
    I2C1->CR1 |= I2C_CR1_SWRST;
    I2C1->CR1 &= ~(I2C_CR1_SWRST);
    I2C1->CR1 |= I2C_CR1_PE;

#ifdef TRY_ON_YOUR_OWN_RISK
    __HAL_RCC_I2C1_CLK_DISABLE();
    __HAL_RCC_I2C1_CLK_ENABLE();
    __HAL_RCC_I2C1_FORCE_RESET();
    s_sleep(32);
    __HAL_RCC_I2C1_RELEASE_RESET();
    s_sleep(32);
#endif
}


void i2c_detect_ontca_mux(void)
{
	uint8_t     data[4]={0x00};
	
	for(uint8_t pin=1;pin<=8;pin++)
	{
		    TCA9648_select(pin);
   		 for(uint8_t addr=0x1;addr<0x80;addr++)
		    {
			         if(addr == 0x70 ) continue;
			         if(i2c_writebytes(addr, 0, data, 1))
			         {
                //found I2C on pin pin address addr
            }
				   }//addr
}//mux pins
 


/* USER CODE END 1 */



/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
