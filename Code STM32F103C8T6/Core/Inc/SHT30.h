
#ifndef SHT30_H_
#define SHT30_H_

#include "stm32f1xx_hal.h"
#include "main.h"
#include "stdbool.h"
#include "stdint.h"
#include <assert.h>


#ifndef SHT30_I2C_TIMEOUT
#define SHT30_I2C_TIMEOUT 30
#endif

//dia chi sht30
#define SHT30_I2C_DEVICE_ADDRESS_ADDR_PIN_LOW 0x44
#define SHT30_I2C_DEVICE_ADDRESS_ADDR_PIN_HIGH 0x45

// lenh xu ly sht30
#define   SHT30_COMMAND_MEASURE_HIGHREP_STRETCH  0x2c06 // gui lenh yeu cau cam bien xu ly du lieu , SCL = LOW
#define   SHT30_COMMAND_CLEAR_STATUS  0x3041 // gui lenh xoa trang thai 
#define   SHT30_COMMAND_SOFT_RESET  0x30A2 // Reset cam bien
#define   SHT30_COMMAND_HEATER_ENABLE 0x306d
#define   SHT30_COMMAND_HEATER_DISABLE 0x3066
#define   SHT30_COMMAND_READ_STATUS 0xf32d // doc trang thai
#define   SHT30_COMMAND_FETCH_DATA  0xe000
#define   SHT30_COMMAND_MEASURE_HIGHREP_10HZ  0x2737
#define   SHT30_COMMAND_MEASURE_LOWREP_10HZ  0x272a


typedef struct {

	
	I2C_HandleTypeDef *i2c_SHT30_handle;// tao con tro Sht30


	uint16_t SHT30_address; // dia chi cua sht30

}SHT30_Name;


bool SHT30_Read_Temperature_And_Humidity(SHT30_Name *handle, uint16_t *Temperature, uint16_t *Humidity);
bool SHT30_Set_Heater_Enable(SHT30_Name *handle, bool Enable);
bool SHT30_Send_Command(SHT30_Name *handle, unsigned int Command);


#endif /* SHT30_H_ */

