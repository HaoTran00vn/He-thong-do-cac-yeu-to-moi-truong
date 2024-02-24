


#include "DS1307.h"


uint8_t DS1307_DecodeBCD(uint8_t bin) 
{
    return (((bin & 0xf0) >> 4) * 10) + (bin & 0x0f);
}
uint8_t DS1307_EncodeBCD(uint8_t dec) 
{
    return (dec % 10 + ((dec / 10) << 4));
}	

void DS1307_SetRegByte(uint8_t regAddr, uint8_t val) 
{
    uint8_t bytes[2] = { regAddr, val };
    HAL_I2C_Master_Transmit(&DS1307_I2C, DS1307_I2C_ADDR << 1, bytes, 2, DS1307_TIMEOUT);
}
uint8_t DS1307_GetRegByte(uint8_t regAddr) 
{
    uint8_t val;
    HAL_I2C_Master_Transmit(&DS1307_I2C, DS1307_I2C_ADDR << 1, &regAddr, 1, DS1307_TIMEOUT);
    HAL_I2C_Master_Receive(&DS1307_I2C, DS1307_I2C_ADDR << 1, &val, 1, DS1307_TIMEOUT);
    return val;
}

void DS1307_SetClockHalt(uint8_t halt) 
{
    uint8_t ch = (halt ? 1 << 7 : 0);
    DS1307_SetRegByte(DS1307_REG_SECOND, ch | (DS1307_GetRegByte(DS1307_REG_SECOND) & 0x7f));
}	

uint8_t DS1307_GetClockHalt(void) 
{
    return (DS1307_GetRegByte(DS1307_REG_SECOND) & 0x80) >> 7;
}



void DS1307_settime(uint8_t sec,uint8_t min,uint8_t hour_24mode,uint8_t date,uint8_t month, uint16_t year)
{
    DS1307_SetRegByte(DS1307_REG_SECOND, DS1307_EncodeBCD(sec | DS1307_GetClockHalt()));
    DS1307_SetRegByte(DS1307_REG_MINUTE, DS1307_EncodeBCD(min));
    DS1307_SetRegByte(DS1307_REG_HOUR, DS1307_EncodeBCD(hour_24mode & 0x3f));//hour_24mode Hour in 24h format, 0 to 23.
    
    DS1307_SetRegByte(DS1307_REG_DATE, DS1307_EncodeBCD(date));//date Day of month, 1 to 31.
    DS1307_SetRegByte(DS1307_REG_MONTH, DS1307_EncodeBCD(month));//month Month, 1 to 12.
    DS1307_SetRegByte(DS1307_REG_CENT, year / 100);
    DS1307_SetRegByte(DS1307_REG_YEAR, DS1307_EncodeBCD(year % 100));//2000 to 2099.
}

void DS1307_gettime(DS1307_Name* DS1307)
{
    uint16_t cen;
    DS1307->sec=DS1307_DecodeBCD(DS1307_GetRegByte(DS1307_REG_SECOND) & 0x7f);
    DS1307->min=DS1307_DecodeBCD(DS1307_GetRegByte(DS1307_REG_MINUTE));
    DS1307->hour=DS1307_DecodeBCD(DS1307_GetRegByte(DS1307_REG_HOUR) & 0x3f);
 
    DS1307->date=DS1307_DecodeBCD(DS1307_GetRegByte(DS1307_REG_DATE));
    DS1307->month=DS1307_DecodeBCD(DS1307_GetRegByte(DS1307_REG_MONTH));
    cen = DS1307_GetRegByte(DS1307_REG_CENT) * 100;
    DS1307->year=DS1307_DecodeBCD(DS1307_GetRegByte(DS1307_REG_YEAR)) + cen;
}


