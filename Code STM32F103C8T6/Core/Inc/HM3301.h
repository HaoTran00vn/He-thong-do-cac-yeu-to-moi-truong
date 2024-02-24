
#ifndef HM3301_H_
#define HM3301_H_

#include "stm32f1xx_hal.h"
#include "main.h"
#include "stdbool.h"
#include "stdint.h"
#include <assert.h>

#ifndef HM3301_I2C_TIMEOUT
#define HM3301_I2C_TIMEOUT 30
#endif

#define HM3301_I2C_DEVICE_ADDRESS_ADDR 0x40

#define	HM3301_Command 0x88

typedef struct{
	I2C_HandleTypeDef *i2c_HM3301_handle;
	uint16_t HM3301_address;
}HM3301_Name;

bool HM3301_Read_Dust_Value_Atmospheric_Environment(HM3301_Name *handle,uint16_t *PM_10, uint16_t *PM25,uint16_t *PM100);
bool HM3301_Read_Dust_Value_Standard_Particulate(HM3301_Name *handle,uint16_t *PM_10, uint16_t *PM25,uint16_t *PM100);


#endif /* HM3301_H_ */
