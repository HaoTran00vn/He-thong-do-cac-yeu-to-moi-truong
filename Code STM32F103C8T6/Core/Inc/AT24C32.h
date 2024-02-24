#ifndef AT24C32_H_
#define AT24C32_H_

#include "main.h"
#include "stdbool.h"

#define	EEPROM_I2C			hi2c1
#define EEPROM_I2C_ADDR		0xA0
#define EEPROM_PAGESIZE		32
#define EEPROM_WRITE			10

extern I2C_HandleTypeDef EEPROM_I2C;

HAL_StatusTypeDef AT24C32_ReadEEPROM(unsigned address, const void* src, unsigned len);
HAL_StatusTypeDef AT24C32_WriteEEPROM(unsigned address, const void* src, unsigned len);

#endif /* AT24C32_H_ */
