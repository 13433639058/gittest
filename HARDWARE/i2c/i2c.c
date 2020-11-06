#include "i2c.h"
#include "HW32l003_hal.h"

GPIO_InitTypeDef GPIO_Initure1;
I2C_HandleTypeDef I2C_Initure;

void I2C_Init(void)
{
	__HAL_RCC_GPIOC_CLK_ENABLE();
	__HAL_RCC_I2C_CLK_ENABLE();
	
	/*******PC4->SCL************/
	GPIO_Initure1.Pin = GPIO_PIN_4;
	GPIO_Initure1.Mode = GPIO_MODE_AF;
	GPIO_Initure1.Alternate = GPIO_AF4_I2C_SCL;
	GPIO_Initure1.Debounce.Enable = GPIO_DEBOUNCE_DISABLE;
	GPIO_Initure1.SlewRate = GPIO_SLEW_RATE_LOW;
	GPIO_Initure1.DrvStrength = GPIO_DRV_STRENGTH_LOW;
	GPIO_Initure1.Pull = GPIO_NOPULL;
	HAL_GPIO_Init(GPIOC, &GPIO_Initure1);
	
	/*******PC3->SDA************/
	GPIO_Initure1.Pin = GPIO_PIN_3;
	GPIO_Initure1.Mode = GPIO_MODE_AF;
	GPIO_Initure1.Alternate = GPIO_AF4_I2C_SDA;
	HAL_GPIO_Init(GPIOC, &GPIO_Initure1);
	
	I2C_Initure.Mode = HAL_I2C_MODE_MASTER;
	I2C_Initure.Instance = I2C;
	I2C_Initure.State = HAL_I2C_STATE_RESET;
	I2C_Initure.Init.master = I2C_MASTER_MODE_ENABLE;
	I2C_Initure.Init.slave = I2C_SLAVE_MODE_DISABLE;
	I2C_Initure.Init.broadack = I2C_BROAD_ACK_DISABLE;
	I2C_Initure.Init.speedclock = 100;  //kHz
	HAL_I2C_Init(&I2C_Initure);
}

/**
  * @brief I2C_Write_EEPROM
	* @param  dev_address i2c device address
	*					reg_address register address
	*					wdata handle to write data
	*					size write size <=30
  * @retval ret result HAL_OK  or HAL_ERROR 
  */
HAL_StatusTypeDef I2C_Write_EEPROM(uint8_t dev_address,uint8_t reg_address,uint8_t *wdata,uint8_t size)
{
	uint8_t data[30] = {0};
	uint8_t i = 0;
	HAL_StatusTypeDef  ret = HAL_ERROR;
	
	data[0] = reg_address;
	for(i=0;i<size;i++)
		data[i+1]= wdata[i];
	ret = HAL_I2C_Master_Transmit(&I2C_Initure, dev_address, data, size+1);

	return ret;
}
/**
  * @brief I2C_Read_EEPROM
	* @param  dev_address i2c device address
	*					reg_address register address
	*					rdata handle to read data
	*					size read size <=16
  * @retval ret result HAL_OK  or HAL_ERROR
  */
HAL_StatusTypeDef I2C_Read_EEPROM(uint8_t dev_address,uint8_t reg_address,uint8_t *rdata,uint8_t size)
{
	HAL_StatusTypeDef  ret = HAL_ERROR;
	
	ret = HAL_I2C_Master_Transmit(&I2C_Initure, dev_address, &reg_address, 1);
	ret = HAL_I2C_Master_Receive(&I2C_Initure, dev_address, rdata, size);
	return ret;
}
