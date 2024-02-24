

#ifndef DS1307_H_
#define DS1307_H_


#include "stm32f1xx_hal.h"
#include "main.h"
#include "stdbool.h"
#include "stdint.h"

#define	DS1307_I2C					hi2c1
#define DS1307_I2C_ADDR     0x68
#define DS1307_REG_SECOND     0x00
#define DS1307_REG_MINUTE     0x01
#define DS1307_REG_HOUR      0x02
#define DS1307_REG_DOW        0x03
#define DS1307_REG_DATE       0x04
#define DS1307_REG_MONTH      0x05
#define DS1307_REG_YEAR       0x06
#define DS1307_REG_CONTROL     0x07
#define DS1307_REG_UTC_HR    0x08
#define DS1307_REG_UTC_MIN    0x09
#define DS1307_REG_CENT        0x10
#define DS1307_TIMEOUT        1000


extern I2C_HandleTypeDef DS1307_I2C;

typedef struct {
		
    uint8_t sec;
    uint8_t min;
    uint8_t hour;

    uint8_t date;
    uint8_t month;
    uint16_t year;
}DS1307_Name;

uint8_t DS1307_DecodeBCD(uint8_t bin); 
uint8_t DS1307_EncodeBCD(uint8_t dec) ;
void DS1307_SetRegByte(uint8_t regAddr, uint8_t val);
uint8_t DS1307_GetRegByte(uint8_t regAddr);
void DS1307_SetClockHalt(uint8_t halt) ;
uint8_t DS1307_GetClockHalt(void);
void DS1307_settime(uint8_t sec,uint8_t min,uint8_t hour_24mode,uint8_t date,uint8_t month, uint16_t year);
void DS1307_gettime(DS1307_Name* DS1307);

#endif
