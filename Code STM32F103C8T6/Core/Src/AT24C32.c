#include "AT24C32.h"

static unsigned inpage_addr_mask = EEPROM_PAGESIZE - 1;

static HAL_StatusTypeDef AT24C32_WriteReadEEPROM(unsigned address, const void* src, unsigned len, bool write);
static unsigned size_to_page_end(unsigned addr);

HAL_StatusTypeDef AT24C32_ReadEEPROM(unsigned address, const void* src, unsigned len)
{
	return AT24C32_WriteReadEEPROM(address, src, len, false);
}
HAL_StatusTypeDef AT24C32_WriteEEPROM(unsigned address, const void* src, unsigned len)
{
	return AT24C32_WriteReadEEPROM(address, src, len, true);
}
static HAL_StatusTypeDef AT24C32_WriteReadEEPROM(unsigned address, const void* src, unsigned len, bool write)
{
	uint8_t *pdata = (uint8_t*) src;
	HAL_StatusTypeDef result = HAL_OK;
  unsigned max_portion = size_to_page_end(address);
  unsigned portion;

  while (len != 0 && result == HAL_OK)
  {
		portion = len;
		if (portion > max_portion)
		{
			portion = max_portion;
		}
		if(write)
		{
			result = HAL_I2C_Mem_Write(&EEPROM_I2C, EEPROM_I2C_ADDR, address, I2C_MEMADD_SIZE_16BIT, pdata, portion, 100);
		}
		else
		{
			result = HAL_I2C_Mem_Read(&EEPROM_I2C, EEPROM_I2C_ADDR, address, I2C_MEMADD_SIZE_16BIT, pdata, portion, 100);
		}
		// Neu tran bo nho thi ghi/doc lai dia chi dau tien
		len     -= portion;
		address += portion;
		pdata   += portion;
		max_portion = EEPROM_PAGESIZE;
		if(write)
		{
			HAL_Delay(EEPROM_WRITE);
		}
		else
		{
			HAL_Delay(EEPROM_WRITE / 2);
		}
   }
    return result;
}
static unsigned size_to_page_end(unsigned addr)
{
    return (~addr & inpage_addr_mask) + 1;
}
