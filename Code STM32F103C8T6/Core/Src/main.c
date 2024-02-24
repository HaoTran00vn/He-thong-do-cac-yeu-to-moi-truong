/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2023 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "main.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "stdio.h"
#include "string.h"
#include "stdbool.h"
#include "HM3301.h"
#include "SHT30.h"
#include "BH1750.h"
#include "DS1307.h"
#include "CLCD_I2C.h"
#include "AT24C32.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

//Khoi tao mode 
typedef enum{
	Mode1, // Measure
	Mode2, // Memory
	Mode3, // Setting
}Mode_choose;

typedef enum{
	Mode1_Begin,
	Mode1_Measure_Temp1Hum1,
	Mode1_Measure_Temp2Hum2,
	Mode1_Measure_Lux1,
	Mode1_Measure_Lux2,
	Mode1_Measure_PM25,
	
	Mode1_Save,
}Mode_1_t;

typedef enum{
	Mode2_Begin,
	Mode2_Display_Memory,
}Mode_2_t;

typedef enum{
	Mode3_Begin,
	Mode3_Setting_Time
}Mode_3_t;

//Gan kieu du lieu 
struct{
	uint16_t Temp1;
	uint16_t Hum1;
	uint16_t Temp2;
	uint16_t Hum2;
	uint16_t Lux1;
	uint16_t Lux2;
	uint16_t Dust_PM25;
	uint16_t Day;
	uint16_t Month;
	uint16_t Year;
	uint16_t Hour;
	uint16_t Min;
}Buffer_2byte;

struct{

	uint8_t Temp1_High;
	uint8_t Temp1_Low;
	uint8_t Hum1_High;
	uint8_t Hum1_Low;
	uint8_t Temp2_High;
	uint8_t Temp2_Low;
	uint8_t Hum2_High;
	uint8_t Hum2_Low;
	uint8_t Lux1_High;
	uint8_t Lux1_Low;
	uint8_t Lux2_High;
	uint8_t Lux2_Low;
	uint8_t Dust_PM25_High;
	uint8_t Dust_PM25_Low;
	uint8_t Day_High;
	uint8_t Day_Low;
	uint8_t Month_High;
	uint8_t Month_Low;
	uint8_t Year_High;
	uint8_t Year_Low;
	uint8_t Hour_High;
	uint8_t Hour_Low;
	uint8_t Min_High;
	uint8_t Min_Low;

}EEprom_Buffer;


// Khoi tao struct nut nhan
typedef struct {
	GPIO_TypeDef* Button_Port;
	uint16_t Button_Pin;
	bool Press;
}BUTTON_Name;
/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
 I2C_HandleTypeDef hi2c1;

UART_HandleTypeDef huart2;

/* USER CODE BEGIN PV */

//Khoi tao Count
extern uint16_t volatile timecount,timepress;
//Khoi tao bien UART
char Buffer_Tx[70];

//Khoi tao bien doc Eeprom
uint8_t Page;
uint16_t Read_Temp1_From_EEprom;
uint16_t Read_Hum1_From_EEprom;
uint16_t Read_Temp2_From_EEprom;
uint16_t Read_Hum2_From_EEprom;
uint16_t Read_Lux1_From_EEprom;
uint16_t Read_Lux2_From_EEprom;
uint16_t Read_PM25_From_EEprom;
uint16_t Read =0;


//Khoi tao cam bien HM3301
HM3301_Name HM3301_device_1 =
{
	.i2c_HM3301_handle = &hi2c1,
	.HM3301_address = HM3301_I2C_DEVICE_ADDRESS_ADDR,
};
uint16_t HM3301_PM25,HM3301_PM10,HM3301_PM100;

// Khoi tao cam bien SHT30
SHT30_Name SHT30_device_1=
{
	.i2c_SHT30_handle = &hi2c1,
	.SHT30_address = SHT30_I2C_DEVICE_ADDRESS_ADDR_PIN_HIGH,	
};
uint16_t SHT30_Temp1,SHT30_Hum1;

SHT30_Name SHT30_device_2=
{
	.i2c_SHT30_handle = &hi2c1,
	.SHT30_address = SHT30_I2C_DEVICE_ADDRESS_ADDR_PIN_LOW,	
};
uint16_t SHT30_Temp2,SHT30_Hum2;

//Khoi tao cam bien BH1750
BH1750_Name BH1750_device_1 =
{
	.i2c_BH1750_handle = &hi2c1,
	.BH1750_address = BH1750_I2C_DEVICE_ADDRESS_ADDR_PIN_HIGH,
};
uint16_t BH1750_Lux1;

BH1750_Name BH1750_device_2 =
{
	.i2c_BH1750_handle = &hi2c1,
	.BH1750_address = BH1750_I2C_DEVICE_ADDRESS_ADDR_PIN_LOW,
};
uint16_t BH1750_Lux2;


//Khoi tao Uart2
char Tx[100];
int8_t volatile Connect_Status = -1;// 1 esp 8266 da ket noi wifi, 0 ko ket noi dc wifi
char Rx;

// Khoi tao nut nhan
BUTTON_Name Select, Up, Down, Back;

//Khoi tao LCD
CLCD_I2C_Name LCD;
bool Display = true;
char String[70];
// Khoi tao DS1307
DS1307_Name DS1307;
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_I2C1_Init(void);
static void MX_USART2_UART_Init(void);
/* USER CODE BEGIN PFP */
void Button_Init(BUTTON_Name* Button, GPIO_TypeDef* Button_Port, uint16_t Button_Pin);//ham khoi tao nut nhan
bool Button_Press(BUTTON_Name* Button);	//ham khoi tao nut nhan
uint16_t Uint8_To_Uint16(uint8_t Msb, uint8_t Lsb);
void Update_Setting(void);
uint8_t Setting_Data(uint8_t Data, uint8_t Limit_Down, uint8_t Limit_Up, bool Inc);
/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */
#ifdef __GNUC__
#define PUTCHAR_PROTOTYPE int __io_putchar(int ch)
#else
#define PUTCHAR_PROTOTYPE int fputc(int ch, FILE *f)
#endif /* __GNUC__ */
 
/**
  * @brief  Retargets the C library printf function to the USART.
  * @param  None
  * @retval None
  */

void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{ 
  if(Rx == '1') Connect_Status = 1;
	if(Rx == '0') Connect_Status = 0;
	HAL_UART_Receive_IT(&huart2,(uint8_t *)&Rx,1);
}
/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{
  /* USER CODE BEGIN 1 */

  /* USER CODE END 1 */

  /* MCU Configuration--------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();

  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();

  /* USER CODE BEGIN SysInit */

  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_I2C1_Init();
  MX_USART2_UART_Init();
  /* USER CODE BEGIN 2 */
	HAL_UART_Receive_IT(&huart2,(uint8_t *)&Rx,1);

// khoi tao nut nhan
	Button_Init(&Select,Select_GPIO_Port,Select_Pin);
	Button_Init(&Up,Up_GPIO_Port,Up_Pin);
	Button_Init(&Down,Down_GPIO_Port,Down_Pin);
	Button_Init(&Back,Back_GPIO_Port,Back_Pin);
	if(DS1307_GetClockHalt() != 0)	DS1307_SetClockHalt(0);
	

	CLCD_I2C_Init(&LCD,&hi2c1,0x7e,20,4);
	CLCD_I2C_SetCursor(&LCD, 0, 0);
	CLCD_I2C_WriteString(&LCD,"Thiet lap he thong");
	
	HAL_Delay(2000);
///*
		while(1)
		{
			if(Connect_Status ==1)
			{
				CLCD_I2C_Clear(&LCD);
				CLCD_I2C_SetCursor(&LCD,0,0);
				CLCD_I2C_WriteString(&LCD,"ESP8266 ket noi wifi");
				HAL_Delay(2000);
				break;
			}
			if(Connect_Status == 0)
			{
				if(Display == true)
				{
					Display =false;
				CLCD_I2C_Clear(&LCD);
				CLCD_I2C_SetCursor(&LCD,0,0);
				CLCD_I2C_WriteString(&LCD,"ESP8266 chua ket noi ");
				CLCD_I2C_SetCursor(&LCD,0,1);
				CLCD_I2C_WriteString(&LCD,"Truy cap wifi va IP ");	
				CLCD_I2C_SetCursor(&LCD,0,2);	
				CLCD_I2C_WriteString(&LCD,"ESP_154933");	
				CLCD_I2C_SetCursor(&LCD,0,3);		
				CLCD_I2C_WriteString(&LCD,"192.168.4.1");		
				}	
			}
		}
//*/
	Mode_choose Current_Mode = Mode1;
	Mode_1_t Mode_1_Status = Mode1_Begin;
	Mode_2_t Mode_2_Status = Mode2_Begin;
	Mode_3_t Mode_3_Status = Mode3_Begin;
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {

    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
		switch(Current_Mode)
		{
			case Mode1:
			{
				switch(Mode_1_Status)
				{
					case Mode1_Begin:
					{
						if(timecount >1000)
						{
							timecount=0;
							CLCD_I2C_SetCursor(&LCD, 0, 0);
							DS1307_gettime(&DS1307);
							sprintf((char *)String,"%02d/%02d/20%02d %02d:%02d:%02d ",DS1307.date,DS1307.month,DS1307.year,DS1307.hour,DS1307.min,DS1307.sec);
							CLCD_I2C_WriteString(&LCD,String);
							Display = true;
						}
						if(Display == true)
						{
							Display = false;
							timecount = 0;
							CLCD_I2C_SetCursor(&LCD, 0, 0);
							DS1307_gettime(&DS1307);
							sprintf((char *)String,"%02d/%02d/20%02d %02d:%02d:%02d",DS1307.date,DS1307.month,DS1307.year,DS1307.hour,DS1307.min,DS1307.sec);
							CLCD_I2C_WriteString(&LCD,String);		
							CLCD_I2C_SetCursor(&LCD, 0, 1);
							CLCD_I2C_WriteString(&LCD,"T1:");
							CLCD_I2C_SetCursor(&LCD, 6, 1);
							CLCD_I2C_WriteString(&LCD,"H1:");
							CLCD_I2C_SetCursor(&LCD, 12, 1);
							CLCD_I2C_WriteString(&LCD,"L1:");
							CLCD_I2C_SetCursor(&LCD, 0, 2);
							CLCD_I2C_WriteString(&LCD,"T2:");
							CLCD_I2C_SetCursor(&LCD, 6, 2);
							CLCD_I2C_WriteString(&LCD,"H2:");
							CLCD_I2C_SetCursor(&LCD, 12, 2);
							CLCD_I2C_WriteString(&LCD,"L2:");
							CLCD_I2C_SetCursor(&LCD, 0, 3);
							CLCD_I2C_WriteString(&LCD,"PM2.5:");
							CLCD_I2C_SetCursor(&LCD, 14, 3);
							CLCD_I2C_WriteString(&LCD,"Mode1");
						}
						if(Button_Press(&Up))
						{
							
							Current_Mode = Mode2;
							CLCD_I2C_Clear(&LCD);
							Display=true;
						}
						if(Button_Press(&Select))
						{
							CLCD_I2C_SetCursor(&LCD, 14, 3);
							CLCD_I2C_WriteString(&LCD,"      ");							
							timecount = 0;
							HM3301_PM25 =0; // Reset gia tri cu
							SHT30_Temp1=0;
							SHT30_Hum1 =0;
							SHT30_Temp2 =0;
							SHT30_Hum2 =0;
							BH1750_Lux1 = 0;
							BH1750_Lux2 = 0;
							CLCD_I2C_SetCursor(&LCD, 0, 0);
							CLCD_I2C_WriteString(&LCD,"Bat dau do gia tri:");
							HAL_Delay(2000);
							Mode_1_Status = Mode1_Measure_Temp1Hum1;
						}						
					}
					break;
					case Mode1_Measure_Temp1Hum1:
					{
							CLCD_I2C_SetCursor(&LCD, 0, 0);
							CLCD_I2C_WriteString(&LCD,"Do Temp1 Hum1:     ");
							SHT30_Read_Temperature_And_Humidity(&SHT30_device_1,&SHT30_Temp1,&SHT30_Hum1);
							HAL_Delay(500);
							CLCD_I2C_SetCursor(&LCD, 3, 1);
							sprintf((char *)String,"%2d",SHT30_Temp1);
							CLCD_I2C_WriteString(&LCD,String);
							CLCD_I2C_SetCursor(&LCD, 9, 1);
							sprintf((char *)String,"%2d",SHT30_Hum1);
							CLCD_I2C_WriteString(&LCD,String);
							HAL_Delay(2000);
							Mode_1_Status = Mode1_Measure_Temp2Hum2;
								
					}	
					break;
					case Mode1_Measure_Temp2Hum2:
					{
							CLCD_I2C_SetCursor(&LCD, 0, 0);
							CLCD_I2C_WriteString(&LCD,"Do Temp2 Hum2:     ");
							SHT30_Read_Temperature_And_Humidity(&SHT30_device_2,&SHT30_Temp2,&SHT30_Hum2);
							HAL_Delay(500);
							CLCD_I2C_SetCursor(&LCD, 3, 2);
							sprintf((char *)String,"%2d",SHT30_Temp2);
							CLCD_I2C_WriteString(&LCD,String);
							CLCD_I2C_SetCursor(&LCD, 9, 2);
							sprintf((char *)String,"%2d",SHT30_Hum2);
							CLCD_I2C_WriteString(&LCD,String);
							HAL_Delay(2000);
							Mode_1_Status = Mode1_Measure_Lux1;

					}
					break;
					case Mode1_Measure_Lux1:
					{
							CLCD_I2C_SetCursor(&LCD, 0, 0);
							CLCD_I2C_WriteString(&LCD,"Do cuong do sang1:  ");
							BH1750_Read_Lux_Mode_One_Time(&BH1750_device_1,&BH1750_Lux1);
							HAL_Delay(500);
							CLCD_I2C_SetCursor(&LCD, 15, 1);
							sprintf((char *)String,"%2d",BH1750_Lux1);
							CLCD_I2C_WriteString(&LCD,String);
							HAL_Delay(2000);
							Mode_1_Status = Mode1_Measure_Lux2;
					}	
					break;
					case Mode1_Measure_Lux2:
					{
							CLCD_I2C_SetCursor(&LCD, 0, 0);
							CLCD_I2C_WriteString(&LCD,"Do cuong do sang2:  ");
							BH1750_Read_Lux_Mode_One_Time(&BH1750_device_2,&BH1750_Lux2);
							HAL_Delay(500);
							CLCD_I2C_SetCursor(&LCD, 15, 2);
							sprintf((char *)String,"%2d",BH1750_Lux2);
							CLCD_I2C_WriteString(&LCD,String);
							HAL_Delay(2000);
							Mode_1_Status = Mode1_Measure_PM25;
					}
					break;
					case Mode1_Measure_PM25:
					{
						CLCD_I2C_SetCursor(&LCD, 0, 0);
						CLCD_I2C_WriteString(&LCD,"Do bui PM2.5:       ");
						HM3301_Read_Dust_Value_Standard_Particulate(&HM3301_device_1,&HM3301_PM10,&HM3301_PM25,&HM3301_PM100);
						HAL_Delay(500);
						CLCD_I2C_SetCursor(&LCD, 6, 3);
						sprintf((char *)String,"%2d",HM3301_PM25);
						CLCD_I2C_WriteString(&LCD,String);
						HAL_Delay(2000);
						CLCD_I2C_SetCursor(&LCD, 0, 0);
						CLCD_I2C_WriteString(&LCD,"Da do xong gia tri:");
						Mode_1_Status = Mode1_Save;						
					}
					
					break;
					case Mode1_Save:
					{
						
						if(Button_Press(&Back))
						{
							
							CLCD_I2C_SetCursor(&LCD, 0, 0);
							CLCD_I2C_WriteString(&LCD,"Khong luu lai:      ");
							HM3301_PM25 =0; // Reset gia tri
							SHT30_Temp1=0;
							SHT30_Hum1 =0;
							SHT30_Temp2 =0;
							SHT30_Hum2 =0;
							BH1750_Lux1 = 0;
							BH1750_Lux2 = 0;
							HAL_Delay(2000);
							Mode_1_Status = Mode1_Begin;
							CLCD_I2C_Clear(&LCD);
							Display = true;
						}
						
						if(Button_Press(&Select))
						{
							CLCD_I2C_SetCursor(&LCD, 0, 0);
							CLCD_I2C_WriteString(&LCD,"Dang luu gia tri:   ");
							for(int i=10;i>=0;i--)
							{
								AT24C32_ReadEEPROM(i*25,&EEprom_Buffer,24);
								AT24C32_WriteEEPROM((i+1)*25,&EEprom_Buffer,24);
							}

							Buffer_2byte.Temp1 = SHT30_Temp1;
							Buffer_2byte.Hum1 = SHT30_Hum1;
							Buffer_2byte.Temp2 = SHT30_Temp2;
							Buffer_2byte.Hum2 = SHT30_Hum2;
							Buffer_2byte.Lux1 = BH1750_Lux1;
							Buffer_2byte.Lux2 = BH1750_Lux2;
							Buffer_2byte.Dust_PM25 = HM3301_PM25;
							Buffer_2byte.Day =DS1307.date;
							Buffer_2byte.Month = DS1307.month;
							Buffer_2byte.Year = DS1307.year;
							Buffer_2byte.Hour = DS1307.hour;
							Buffer_2byte.Min = DS1307.min;
							
							EEprom_Buffer.Temp1_High = (Buffer_2byte.Temp1 >>8u) & 0xff;
							EEprom_Buffer.Temp1_Low = Buffer_2byte.Temp1  & 0xff;
							EEprom_Buffer.Hum1_High = (Buffer_2byte.Hum1 >>8u) & 0xff;
							EEprom_Buffer.Hum1_Low = Buffer_2byte.Hum1  & 0xff;
							EEprom_Buffer.Temp2_High = (Buffer_2byte.Temp2 >>8u) & 0xff;
							EEprom_Buffer.Temp2_Low = Buffer_2byte.Temp2  & 0xff;
							EEprom_Buffer.Hum2_High = (Buffer_2byte.Hum2 >>8u) & 0xff;
							EEprom_Buffer.Hum2_Low = Buffer_2byte.Hum2  & 0xff;
							EEprom_Buffer.Lux1_High = (Buffer_2byte.Lux1 >>8u) & 0xff;
							EEprom_Buffer.Lux1_Low = Buffer_2byte.Lux1  & 0xff;
							EEprom_Buffer.Lux2_High = (Buffer_2byte.Lux2 >>8u) & 0xff;
							EEprom_Buffer.Lux2_Low = Buffer_2byte.Lux2  & 0xff;
							EEprom_Buffer.Dust_PM25_High = (Buffer_2byte.Dust_PM25 >>8u) &0xff;
							EEprom_Buffer.Dust_PM25_Low = Buffer_2byte.Dust_PM25  &0xff;
							EEprom_Buffer.Day_High = (Buffer_2byte.Day>>8u) & 0xff;
							EEprom_Buffer.Day_Low = 	Buffer_2byte.Day & 0xff;
							EEprom_Buffer.Month_High = (Buffer_2byte.Month>>8u) & 0xff;
							EEprom_Buffer.Month_Low = 	Buffer_2byte.Month & 0xff;
							EEprom_Buffer.Year_High = (Buffer_2byte.Year>>8u) & 0xff;
							EEprom_Buffer.Year_Low = 	Buffer_2byte.Year & 0xff;
							EEprom_Buffer.Hour_High = (Buffer_2byte.Hour>>8u) & 0xff;
							EEprom_Buffer.Hour_Low = 	Buffer_2byte.Hour & 0xff;
							EEprom_Buffer.Min_High = (Buffer_2byte.Min>>8u) & 0xff;
							EEprom_Buffer.Min_Low = 	Buffer_2byte.Min & 0xff;
							AT24C32_WriteEEPROM(0,&EEprom_Buffer,24);
							HAL_Delay(2000);
							CLCD_I2C_SetCursor(&LCD, 0, 0);
							CLCD_I2C_WriteString(&LCD,"Da luu gia tri:    ");
							sprintf((char *)Tx,"A%2dB%2dC%2dD%2dE%2dF%2dG%2dH",SHT30_Temp1,SHT30_Hum1,SHT30_Temp2,SHT30_Hum2,BH1750_Lux1,BH1750_Lux2,HM3301_PM25);		
							HAL_UART_Transmit(&huart2,(uint8_t*)Tx,sizeof(Tx),1000);
							HAL_Delay(10000);
							Mode_1_Status = Mode1_Begin;
							CLCD_I2C_Clear(&LCD);
							Display = true;
						}				
					}	
					break;	
				}
			}
			break;
			case Mode2:
			{
				switch(Mode_2_Status)
				{
					case Mode2_Begin:
					{
						if(Display == true)
						{
							Display = false;
							CLCD_I2C_SetCursor(&LCD, 0, 0);
							CLCD_I2C_WriteString(&LCD,"Mode2");
						}
							
						if(Button_Press(&Up))
						{
							Current_Mode = Mode3;
							CLCD_I2C_Clear(&LCD);
							Display=true;
						}
							if(Button_Press(&Down))
						{
							Current_Mode = Mode1;
							CLCD_I2C_Clear(&LCD);
							Display=true;
						}
						if(Button_Press(&Select))
						{
							Mode_2_Status = Mode2_Display_Memory;
							Page =0;
							CLCD_I2C_Clear(&LCD);
							Display=true;
						}	
					}	
					break;
					case Mode2_Display_Memory:
					{
						if(Display == true)
						{
							Display = false;
							CLCD_I2C_SetCursor(&LCD, 0, 0);
							AT24C32_ReadEEPROM(Page*25,&EEprom_Buffer,24);
							sprintf((char *)String,"M%1d:%02d/%02d/20%02d %02d:%02d",Page+1,EEprom_Buffer.Day_Low,EEprom_Buffer.Month_Low,EEprom_Buffer.Year_Low,EEprom_Buffer.Hour_Low,EEprom_Buffer.Min_Low);
							CLCD_I2C_WriteString(&LCD,String);
							//Hien thi Temp1
							CLCD_I2C_SetCursor(&LCD, 0, 1);
							Read_Temp1_From_EEprom = Uint8_To_Uint16(EEprom_Buffer.Temp1_High,EEprom_Buffer.Temp1_Low);
							sprintf((char *)String,"T1:%2d",Read_Temp1_From_EEprom);
							CLCD_I2C_WriteString(&LCD,String);
							
							//Hien thi Hum1
							CLCD_I2C_SetCursor(&LCD, 6, 1);
							Read_Hum1_From_EEprom = Uint8_To_Uint16(EEprom_Buffer.Hum1_High,EEprom_Buffer.Hum1_Low);
							sprintf((char *)String,"H1:%2d",Read_Hum1_From_EEprom);
							CLCD_I2C_WriteString(&LCD,String);
							
							// Hien thi Lux1
							CLCD_I2C_SetCursor(&LCD, 12, 1);
							Read_Lux1_From_EEprom = Uint8_To_Uint16(EEprom_Buffer.Lux1_High,EEprom_Buffer.Lux1_Low);
							sprintf((char *)String,"L1:%5d",Read_Lux1_From_EEprom);
							CLCD_I2C_WriteString(&LCD,String);
							
							//Hien thi Temp2
							CLCD_I2C_SetCursor(&LCD, 0, 2);
							Read_Temp2_From_EEprom = Uint8_To_Uint16(EEprom_Buffer.Temp2_High,EEprom_Buffer.Temp2_Low);
							sprintf((char *)String,"T2:%2d",Read_Temp2_From_EEprom);
							CLCD_I2C_WriteString(&LCD,String);
							
							//Hien thi Hum2
							CLCD_I2C_SetCursor(&LCD, 6, 2);
							Read_Hum2_From_EEprom = Uint8_To_Uint16(EEprom_Buffer.Hum2_High,EEprom_Buffer.Hum2_Low);
							sprintf((char *)String,"H2:%2d",Read_Hum2_From_EEprom);
							CLCD_I2C_WriteString(&LCD,String);
							
							//Hien thi Lux2
							CLCD_I2C_SetCursor(&LCD, 12, 2);
							Read_Lux2_From_EEprom = Uint8_To_Uint16(EEprom_Buffer.Lux2_High,EEprom_Buffer.Lux2_Low);
							sprintf((char *)String,"L2:%5d",Read_Lux2_From_EEprom);
							CLCD_I2C_WriteString(&LCD,String);
							
							//Hien thi PM2.5
							CLCD_I2C_SetCursor(&LCD, 0, 3);
							Read_PM25_From_EEprom = Uint8_To_Uint16(EEprom_Buffer.Dust_PM25_High,EEprom_Buffer.Dust_PM25_Low);
							sprintf((char *)String,"PM25:%4d",Read_PM25_From_EEprom);
							CLCD_I2C_WriteString(&LCD,String);							
						}
						if(Button_Press(&Select) && timepress > 1000)
						{
							//Dich vung nho
							for(int8_t i=Page;i<=10 ;i++)
							{
								AT24C32_ReadEEPROM((i+1)*25,&EEprom_Buffer,24);
								AT24C32_WriteEEPROM(i*25,&EEprom_Buffer,24);
							}
							//Xoa o nho cuoi cung
							EEprom_Buffer.Temp1_High = 0;
							EEprom_Buffer.Temp1_Low = 0;
							EEprom_Buffer.Hum1_High = 0;
							EEprom_Buffer.Hum1_Low = 0;
							EEprom_Buffer.Temp2_High = 0;
							EEprom_Buffer.Temp2_Low = 0;
							EEprom_Buffer.Hum2_High = 0;
							EEprom_Buffer.Hum2_Low = 0;
							EEprom_Buffer.Lux1_High = 0;
							EEprom_Buffer.Lux1_Low = 0;
							EEprom_Buffer.Lux2_High = 0;
							EEprom_Buffer.Lux2_Low = 0;
							EEprom_Buffer.Dust_PM25_High = 0;
							EEprom_Buffer.Dust_PM25_Low = 0;
							EEprom_Buffer.Day_High = 0;
							EEprom_Buffer.Day_Low = 	0;
							EEprom_Buffer.Month_High =0;
							EEprom_Buffer.Month_Low = 	0;
							EEprom_Buffer.Year_High = 0;
							EEprom_Buffer.Year_Low = 0;
							EEprom_Buffer.Hour_High = 0;
							EEprom_Buffer.Hour_Low = 	0;
							EEprom_Buffer.Min_High = 0;
							EEprom_Buffer.Min_Low = 	0;
							AT24C32_WriteEEPROM(275,&EEprom_Buffer,24);
							CLCD_I2C_Clear(&LCD);
							Display = true;
						}
							if(Button_Press(&Back))
						{
							CLCD_I2C_Clear(&LCD);
							Display=true;
							Mode_2_Status = Mode2_Begin;
							
						}
						if(Button_Press(&Up))
						{
							if(Page<10)
							{
								CLCD_I2C_Clear(&LCD);
								Display=true;
								Page++;
							}
						}	
						if(Button_Press(&Down))
						{
							
							if(Page>0)
							{
								CLCD_I2C_Clear(&LCD);
								Display=true;
								Page--;
							}
						}
							
						
					}	
					break;
				}	
			}	
			break;
			case Mode3:
			{
				switch(Mode_3_Status)
				{
					case Mode3_Begin:
					{
						if(Display ==true)
						{
							Display =false;
							CLCD_I2C_SetCursor(&LCD,0,0);
							CLCD_I2C_Clear(&LCD);
							CLCD_I2C_WriteString(&LCD,"Mode3");
						}
						if(Button_Press(&Down))
						{
							Current_Mode = Mode2;
							CLCD_I2C_Clear(&LCD);
							Display=true;
						}
						if(Button_Press(&Select))
						{
							Mode_3_Status = Mode3_Setting_Time;
							CLCD_I2C_Clear(&LCD);
							Display=true;
						}	
					}
					break;
					case Mode3_Setting_Time:
					{
				

						if(Display ==true)
						{
						Display=false;						
						CLCD_I2C_SetCursor(&LCD,0,0);
						CLCD_I2C_WriteString(&LCD,"Cai dat thoi gian:  ");
												
						}
						if(timecount >1000)
						{	
						timecount =0;	
						DS1307_gettime(&DS1307);		
						CLCD_I2C_SetCursor(&LCD,0,1);
						sprintf((char *)String,"%02d/%02d/20%02d %02d:%02d:%02d",DS1307.date,DS1307.month,DS1307.year,DS1307.hour,DS1307.min,DS1307.sec);
						CLCD_I2C_WriteString(&LCD,String);
						}
						if(Button_Press(&Select) && timepress >1000)
						{
							timepress = 0;							
							CLCD_I2C_BlinkOn(&LCD);
							uint8_t number=1;
							while(1)
							{	
							if(Button_Press(&Select))
							{
								timepress =0;
								number++;
							}
							if(number>5)
							{
								number =1;
							}	
								if(Button_Press(&Back))
							{	
								CLCD_I2C_SetCursor(&LCD,0,0);
								CLCD_I2C_WriteString(&LCD,"Da luu lai cai dat: ");
								DS1307_settime(0,DS1307.min,DS1307.hour,DS1307.date,DS1307.month,DS1307.year);	
								CLCD_I2C_BlinkOff(&LCD);							
								HAL_Delay(2000);
								Display =true;
								break;
							}
							switch (number)
							{
								case 1:
								{
									CLCD_I2C_SetCursor(&LCD,0,1);
									if(Button_Press(&Up)){DS1307.date = Setting_Data(DS1307.date,1,31,1);}
									if(Button_Press(&Down)){DS1307.date = Setting_Data(DS1307.date,1,31,0);}	
									
								}
								break;
								case 2:
								{
									CLCD_I2C_SetCursor(&LCD,3,1);
									if(Button_Press(&Up)){DS1307.month = Setting_Data(DS1307.month,1,12,1);}
									if(Button_Press(&Down)){DS1307.month = Setting_Data(DS1307.month,1,12,0);}	
								}
								break;
								case 3:
								{
									CLCD_I2C_SetCursor(&LCD,8,1);
									if(Button_Press(&Up)){DS1307.year = Setting_Data(DS1307.year,0,99,1);}
									if(Button_Press(&Down)){DS1307.year = Setting_Data(DS1307.year,0,99,0);}	
								}	
								break;
								case 4:
								{
									CLCD_I2C_SetCursor(&LCD,11,1);
									if(Button_Press(&Up)){DS1307.hour = Setting_Data(DS1307.hour,0,23,1);}
									if(Button_Press(&Down)){DS1307.hour = Setting_Data(DS1307.hour,0,23,0);}	
								}
								break;
								case 5:
								{
									CLCD_I2C_SetCursor(&LCD,14,1);
									if(Button_Press(&Up)){DS1307.min = Setting_Data(DS1307.min,0,59,1);}
									if(Button_Press(&Down)){DS1307.min = Setting_Data(DS1307.min,0,59,0);}	
								}	
								break;
							}

							}
							
						}
								if(Button_Press(&Back))
						{
							Mode_3_Status = Mode3_Begin;
							CLCD_I2C_Clear(&LCD);
							Display =true;
							
						}
					}	
					break;
				}
			}	
			break;
		}
  }
  /* USER CODE END 3 */
}

/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.HSEPredivValue = RCC_HSE_PREDIV_DIV1;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLMUL = RCC_PLL_MUL9;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV2;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_2) != HAL_OK)
  {
    Error_Handler();
  }
}

/**
  * @brief I2C1 Initialization Function
  * @param None
  * @retval None
  */
static void MX_I2C1_Init(void)
{

  /* USER CODE BEGIN I2C1_Init 0 */

  /* USER CODE END I2C1_Init 0 */

  /* USER CODE BEGIN I2C1_Init 1 */

  /* USER CODE END I2C1_Init 1 */
  hi2c1.Instance = I2C1;
  hi2c1.Init.ClockSpeed = 100000;
  hi2c1.Init.DutyCycle = I2C_DUTYCYCLE_2;
  hi2c1.Init.OwnAddress1 = 0;
  hi2c1.Init.AddressingMode = I2C_ADDRESSINGMODE_7BIT;
  hi2c1.Init.DualAddressMode = I2C_DUALADDRESS_DISABLE;
  hi2c1.Init.OwnAddress2 = 0;
  hi2c1.Init.GeneralCallMode = I2C_GENERALCALL_DISABLE;
  hi2c1.Init.NoStretchMode = I2C_NOSTRETCH_DISABLE;
  if (HAL_I2C_Init(&hi2c1) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN I2C1_Init 2 */

  /* USER CODE END I2C1_Init 2 */

}

/**
  * @brief USART2 Initialization Function
  * @param None
  * @retval None
  */
static void MX_USART2_UART_Init(void)
{

  /* USER CODE BEGIN USART2_Init 0 */

  /* USER CODE END USART2_Init 0 */

  /* USER CODE BEGIN USART2_Init 1 */

  /* USER CODE END USART2_Init 1 */
  huart2.Instance = USART2;
  huart2.Init.BaudRate = 9600;
  huart2.Init.WordLength = UART_WORDLENGTH_8B;
  huart2.Init.StopBits = UART_STOPBITS_1;
  huart2.Init.Parity = UART_PARITY_NONE;
  huart2.Init.Mode = UART_MODE_TX_RX;
  huart2.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart2.Init.OverSampling = UART_OVERSAMPLING_16;
  if (HAL_UART_Init(&huart2) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN USART2_Init 2 */

  /* USER CODE END USART2_Init 2 */

}

/**
  * @brief GPIO Initialization Function
  * @param None
  * @retval None
  */
static void MX_GPIO_Init(void)
{
  GPIO_InitTypeDef GPIO_InitStruct = {0};

  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOC_CLK_ENABLE();
  __HAL_RCC_GPIOD_CLK_ENABLE();
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOC, GPIO_PIN_13, GPIO_PIN_RESET);

  /*Configure GPIO pin : PC13 */
  GPIO_InitStruct.Pin = GPIO_PIN_13;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);

  /*Configure GPIO pins : Back_Pin Down_Pin Up_Pin Select_Pin */
  GPIO_InitStruct.Pin = Back_Pin|Down_Pin|Up_Pin|Select_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_PULLUP;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

}

/* USER CODE BEGIN 4 */
void Button_Init(BUTTON_Name* Button, GPIO_TypeDef* Button_Port, uint16_t Button_Pin)
{
	Button->Button_Port = Button_Port;
	Button->Button_Pin = Button_Pin;
	Button->Press = false;
}
bool Button_Press(BUTTON_Name* Button)
{
	if(HAL_GPIO_ReadPin(Button->Button_Port, Button->Button_Pin) == RESET)
	{
		if(Button->Press == false)
		{
			Button->Press = true;	
	//		timepress = 0;
			HAL_Delay(50);
			return false;
		}
		else return false;
	}
	else
	{
		if(Button->Press == true)
		{
			Button->Press = false;
			return true;
		}
		else return false;
	}
}
uint16_t Uint8_To_Uint16(uint8_t Msb, uint8_t Lsb) // chuyen 2 byte 8 bit thanh 1 byte 16bit
{
   return (uint16_t)((uint16_t)Msb << 8u) | Lsb;
}
uint8_t Setting_Data(uint8_t Data, uint8_t Limit_Down, uint8_t Limit_Up, bool Inc)
{
	if(Inc == true)
	{
		if(Data < Limit_Up)	Data++;
		else	Data = Limit_Down;
	}
	else
	{
		if(Data > Limit_Down)	Data--;
		else	Data = Limit_Up;
	}
	sprintf((char *)String,"%02d",Data);
	CLCD_I2C_WriteString(&LCD,String);
	return Data;
}
void Update_Setting(void)
{	
		
	DS1307_gettime(&DS1307);
	CLCD_I2C_SetCursor(&LCD,0,1);
	sprintf((char *)String,"%02d/%02d/20%02d %02d:%02d",DS1307.date,DS1307.month,DS1307.year,DS1307.hour,DS1307.min);
	CLCD_I2C_WriteString(&LCD,String);			
}

/* USER CODE END 4 */

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */
  __disable_irq();
  while (1)
  {
  }
  /* USER CODE END Error_Handler_Debug */
}

#ifdef  USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t *file, uint32_t line)
{
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */
