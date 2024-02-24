

#ifndef BH1750_H_
#define BH1750_H_

#include "stm32f1xx_hal.h"
#include "main.h"
#include "stdbool.h"
#include "stdint.h"
#include <assert.h>



#ifndef BH1750_I2C_TIMEOUT
#define BH1750_I2C_TIMEOUT 30
#endif

#define BH1750_I2C_DEVICE_ADDRESS_ADDR_PIN_LOW 0x23
#define BH1750_I2C_DEVICE_ADDRESS_ADDR_PIN_HIGH 0x5C

#define	BH1750_Command_Power_Dowm 0x00
#define	BH1750_Command_Power_On 0x01
#define	BH1750_Command_Reset 0x07
#define	BH1750_Command_Continously_H_Mode 0x10 //Ðo liên tuc Scale = 1
#define	BH1750_Command_Continously_H_Mode_2 0x11 //Ðo liên tuc Scale = 0.5
#define	BH1750_Command_Continously_L_Mode 0x13// Ðo liên tuc Scale =4;
#define	BH1750_Command_One_Time_H_Mode 0x20 // Ðo 1 lan Scale =1;
#define	BH1750_Command_One_Time_H_Mode_2 0x21 //Ðo 1 lan Scale = 0.5
#define	BH1750_Command_One_Time_L_Mode 0x23 //Ðo 1 lan Scale = 4

typedef struct{
	I2C_HandleTypeDef *i2c_BH1750_handle;
	uint16_t BH1750_address;
}BH1750_Name;

bool BH1750_Send_Command(BH1750_Name *handle,unsigned int Command);
bool BH1750_Read_Lux_Mode_One_Time(BH1750_Name *handle,uint16_t *LUX);
bool BH1750_Read_Lux_Mode_Continously_H_Mode(BH1750_Name *handle,uint16_t *LUX);

#endif /* BH1750_H_ */

