#ifndef __I2C_H__
#define __I2C_H__

#include "HW32l003_hal.h"

void I2C_Init(void);
HAL_StatusTypeDef I2C_Write_EEPROM(uint8_t dev_address,uint8_t reg_address,uint8_t *wdata,uint8_t size);
HAL_StatusTypeDef I2C_Read_EEPROM(uint8_t dev_address,uint8_t reg_address,uint8_t *rdata,uint8_t size);

#endif
