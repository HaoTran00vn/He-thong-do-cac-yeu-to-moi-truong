#include "SHT30.h"



bool SHT30_Send_Command(SHT30_Name *handle, unsigned int Command)
{
	   uint8_t Command_buffer[2] = {(Command & 0xff00u) >> 8u, Command & 0x00ffu};

   if (HAL_I2C_Master_Transmit(handle->i2c_SHT30_handle, handle->SHT30_address << 1u, Command_buffer, sizeof(Command_buffer),
                               SHT30_I2C_TIMEOUT) != HAL_OK) {
      return false;
   }

   return true;
}	

uint16_t SHT30_Uint8_To_Uint16(uint8_t Msb, uint8_t Lsb) // chuyen 2 byte 8 bit thanh 1 byte 16bit
{
   return (uint16_t)((uint16_t)Msb << 8u) | Lsb;
}


bool SHT30_Read_Temperature_And_Humidity(SHT30_Name *handle, uint16_t *Temperature, uint16_t *Humidity)
{
   SHT30_Send_Command(handle, SHT30_COMMAND_MEASURE_HIGHREP_STRETCH);// gui lenh yeu cau cam bien doc du lieu khi SCL = 0

   HAL_Delay(200);

   uint8_t buffer[6];
   if (HAL_I2C_Master_Receive(handle->i2c_SHT30_handle, handle->SHT30_address << 1u, buffer, sizeof(buffer), SHT30_I2C_TIMEOUT) != HAL_OK) {
      return false;
   }

 //  uint8_t temperature_crc = calculate_crc(buffer, 2);
 //  uint8_t humidity_crc = calculate_crc(buffer + 3, 2);
 //  if (temperature_crc != buffer[2] || humidity_crc != buffer[5]) {
   //   return false;
  // }

   uint16_t Temperature_raw = (int16_t)SHT30_Uint8_To_Uint16(buffer[0], buffer[1]);
   uint16_t Humidity_raw = SHT30_Uint8_To_Uint16(buffer[3], buffer[4]);

   *Temperature = -45 + 175.0f * Temperature_raw / 65535;
   *Humidity = 100 * Humidity_raw / 65535;

   return true;
}
bool SHT30_Set_Header_Enable(SHT30_Name *handle, bool Enable)
{
   if (Enable) {
      return SHT30_Send_Command(handle, SHT30_COMMAND_HEATER_ENABLE);
   } else {
      return SHT30_Send_Command(handle, SHT30_COMMAND_HEATER_DISABLE);
   }
}
