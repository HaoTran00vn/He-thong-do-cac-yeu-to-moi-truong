

#include "BH1750.h"



	
bool BH1750_Send_Command(BH1750_Name *handle,unsigned int Command)
{
	uint8_t Command_buffer[1] = {Command};
	
	if(HAL_I2C_Master_Transmit(handle -> i2c_BH1750_handle,handle -> BH1750_address <<1u,Command_buffer,sizeof(Command_buffer),BH1750_I2C_TIMEOUT) != HAL_OK){
	
		return false;
		
	}
	return true;
}

uint16_t BH1750_Uint8_To_Uint16(uint8_t Msb, uint8_t Lsb) // chuyen 2 byte 8 bit thanh 1 byte 16bit
{
   return (uint16_t)((uint16_t)Msb << 8u) | Lsb;
}

bool BH1750_Read_Lux_Mode_One_Time(BH1750_Name *handle,uint16_t *LUX)
{
	uint8_t Lux_Buffer[2];
	BH1750_Send_Command(handle,BH1750_Command_Power_Dowm);
	HAL_Delay(5);
	BH1750_Send_Command(handle,BH1750_Command_Power_On);
	HAL_Delay(5);
	BH1750_Send_Command(handle,BH1750_Command_One_Time_H_Mode);
	HAL_Delay(140);
	if(HAL_I2C_Master_Receive(handle ->i2c_BH1750_handle,handle ->BH1750_address <<1u,Lux_Buffer,sizeof(Lux_Buffer),BH1750_I2C_TIMEOUT) != HAL_OK){
		return false;
	}
	uint16_t Lux_raw = BH1750_Uint8_To_Uint16(Lux_Buffer[0],Lux_Buffer[1]);
	*LUX = Lux_raw/1.2;
	return true;
	
}
bool BH1750_Read_Lux_Mode_Continously_H_Mode(BH1750_Name *handle,uint16_t *LUX)
{
	uint8_t Lux_Buffer[2];
	BH1750_Send_Command(handle,BH1750_Command_Power_Dowm);
	HAL_Delay(5);
	BH1750_Send_Command(handle,BH1750_Command_Power_On);
	HAL_Delay(5);
	BH1750_Send_Command(handle,BH1750_Command_Continously_H_Mode);
	HAL_Delay(140);
	if(HAL_I2C_Master_Receive(handle ->i2c_BH1750_handle,handle ->BH1750_address <<1u,Lux_Buffer,sizeof(Lux_Buffer),BH1750_I2C_TIMEOUT) != HAL_OK){
		return false;
	}
	uint16_t Lux_raw = BH1750_Uint8_To_Uint16(Lux_Buffer[0],Lux_Buffer[1]);
	*LUX = Lux_raw/1.2;
	return true;
}	
