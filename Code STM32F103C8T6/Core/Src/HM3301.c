


#include "HM3301.h"


bool HM3301_Send_Command(HM3301_Name *handle,unsigned int Command)
{
   uint8_t Command_buffer[1] = {Command};
   
   if(HAL_I2C_Master_Transmit(handle -> i2c_HM3301_handle,handle -> HM3301_address <<1u,Command_buffer,sizeof(Command_buffer),HM3301_I2C_TIMEOUT) != HAL_OK){
   
      return false;
      
   }
   return true;
}

uint16_t HM3301_Uint8_To_Uint16(uint8_t Msb, uint8_t Lsb) // chuyen 2 byte 8 bit thanh 1 byte 16bit
{
   return (uint16_t)((uint16_t)Msb << 8u) | Lsb;
}

bool HM3301_Read_Dust_Value_Standard_Particulate(HM3301_Name *handle,uint16_t *PM10, uint16_t *PM25,uint16_t *PM100)
{
	uint8_t Dust_Buffer[29];
	HM3301_Send_Command(handle,HM3301_Command);
	HAL_Delay(10);
	if(HAL_I2C_Master_Receive(handle -> i2c_HM3301_handle,handle ->HM3301_address <<1u,Dust_Buffer,sizeof(Dust_Buffer),HM3301_I2C_TIMEOUT) != HAL_OK)
	{
		return false;
	}
	*PM10 = HM3301_Uint8_To_Uint16(Dust_Buffer[6],Dust_Buffer[5]);
	*PM25 = HM3301_Uint8_To_Uint16(Dust_Buffer[8],Dust_Buffer[7]);
	*PM100 = HM3301_Uint8_To_Uint16(Dust_Buffer[10],Dust_Buffer[9]);
	
	return true;
}

bool HM3301_Read_Dust_Value_Atmospheric_Environment(HM3301_Name *handle,uint16_t *PM_10, uint16_t *PM25,uint16_t *PM100)
{
	uint8_t Dust_Buffer[30];
	HM3301_Send_Command(handle,HM3301_Command);
	HAL_Delay(10);
	if(HAL_I2C_Master_Receive(handle -> i2c_HM3301_handle,handle ->HM3301_address <<1u,Dust_Buffer,sizeof(Dust_Buffer),HM3301_I2C_TIMEOUT) != HAL_OK)
	{
		return false;
	}
	*PM_10 = HM3301_Uint8_To_Uint16(Dust_Buffer[12],Dust_Buffer[11]);
	*PM25 = HM3301_Uint8_To_Uint16(Dust_Buffer[14],Dust_Buffer[13]);
	*PM100 = HM3301_Uint8_To_Uint16(Dust_Buffer[16],Dust_Buffer[15]);
	return true;
}



