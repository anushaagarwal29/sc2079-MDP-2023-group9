/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attentiontdxdt6xdrd
  *
  * Copyright (c) 2022 STMicroelectronics.
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
#include "cmsis_os.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "oled.h"

/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
ADC_HandleTypeDef hadc1;
DMA_HandleTypeDef hdma_adc1;

I2C_HandleTypeDef hi2c1;

TIM_HandleTypeDef htim1;
TIM_HandleTypeDef htim2;
TIM_HandleTypeDef htim3;
TIM_HandleTypeDef htim4;
TIM_HandleTypeDef htim8;

UART_HandleTypeDef huart3;

/* Definitions for defaultTask */
osThreadId_t defaultTaskHandle;
const osThreadAttr_t defaultTask_attributes = {
  .name = "defaultTask",
  .stack_size = 128 * 4,
  .priority = (osPriority_t) osPriorityNormal,
};
/* Definitions for EncoderTask */
osThreadId_t EncoderTaskHandle;
const osThreadAttr_t EncoderTask_attributes = {
  .name = "EncoderTask",
  .stack_size = 128 * 4,
  .priority = (osPriority_t) osPriorityNormal,
};
/* Definitions for MotorTask */
osThreadId_t MotorTaskHandle;
const osThreadAttr_t MotorTask_attributes = {
  .name = "MotorTask",
  .stack_size = 128 * 4,
  .priority = (osPriority_t) osPriorityNormal,
};
/* Definitions for GyroTask */
osThreadId_t GyroTaskHandle;
const osThreadAttr_t GyroTask_attributes = {
  .name = "GyroTask",
  .stack_size = 128 * 4,
  .priority = (osPriority_t) osPriorityNormal,
};
/* Definitions for UltrasoundTask */
osThreadId_t UltrasoundTaskHandle;
const osThreadAttr_t UltrasoundTask_attributes = {
  .name = "UltrasoundTask",
  .stack_size = 128 * 4,
  .priority = (osPriority_t) osPriorityNormal,
};
/* USER CODE BEGIN PV */

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_TIM8_Init(void);
static void MX_TIM2_Init(void);
static void MX_TIM1_Init(void);
static void MX_USART3_UART_Init(void);
static void MX_TIM3_Init(void);
static void MX_TIM4_Init(void);
static void MX_I2C1_Init(void);
static void MX_DMA_Init(void);
static void MX_ADC1_Init(void);
void StartDefaultTask(void *argument);
void encoder_task(void *argument);
void motorTask(void *argument);
void gyroTask(void *argument);
void ultrasoundTask(void *argument);


/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

//OLED
#include "../../PeripheralDriver/Inc/oled.h"

#include "stdlib.h"

#include "../../PeripheralDriver/Inc/oledfont.h"

uint8_t OLED_GRAM[128][8];
void OLED_Refresh_Gram(void)
{
	uint8_t i,n;
	for(i=0;i<8;i++)
	{
		OLED_WR_Byte (0xb0+i,OLED_CMD);
		OLED_WR_Byte (0x00,OLED_CMD);
		OLED_WR_Byte (0x10,OLED_CMD);
		for(n=0;n<128;n++)OLED_WR_Byte(OLED_GRAM[n][i],OLED_DATA);
	}
}

void OLED_WR_Byte(uint8_t dat,uint8_t cmd)
{
	uint8_t i;
	if(cmd)
	  OLED_RS_Set();
	else
	  OLED_RS_Clr();
	for(i=0;i<8;i++)
	{
		OLED_SCLK_Clr();
		if(dat&0x80)
		   OLED_SDIN_Set();
		else
		   OLED_SDIN_Clr();
		OLED_SCLK_Set();
		dat<<=1;
	}
	OLED_RS_Set();
}

/**************************************************************************
Clear OLED
**************************************************************************/
void OLED_Clear(void)
{
	uint8_t i,n;
	for(i=0;i<8;i++)for(n=0;n<128;n++)OLED_GRAM[n][i]=0X00;
	OLED_Refresh_Gram();//Refresh
}

 /**************************************************************************
Turn On Display
**************************************************************************/
void OLED_Display_On(void)
{
	OLED_WR_Byte(0X8D,OLED_CMD);  //SET DCDC Command
	OLED_WR_Byte(0X14,OLED_CMD);  //DCDC ON
	OLED_WR_Byte(0XAF,OLED_CMD);  //DISPLAY ON
}
/**************************************************************************
Turn Off Display
**************************************************************************/
void OLED_Display_Off(void)
{
	OLED_WR_Byte(0X8D,OLED_CMD);  //SET DCDC Command
	OLED_WR_Byte(0X10,OLED_CMD);  //DCDC OFF
	OLED_WR_Byte(0XAE,OLED_CMD);  //DISPLAY OFF
}

/**************************************************************************
Draw A Point
**************************************************************************/
void OLED_DrawPoint(uint8_t x,uint8_t y,uint8_t t)
{
	uint8_t pos,bx,temp=0;
	if(x>127||y>63)return;//Out of reach
	pos=7-y/8;
	bx=y%8;
	temp=1<<(7-bx);
	if(t)OLED_GRAM[x][pos]|=temp;
	else OLED_GRAM[x][pos]&=~temp;
}
/**************************************************************************
Show Char
**************************************************************************/
void OLED_ShowChar(uint8_t x,uint8_t y,uint8_t chr,uint8_t size,uint8_t mode)
{
	uint8_t temp,t,t1;
	uint8_t y0=y;
	chr=chr-' ';
    for(t=0;t<size;t++)
    {
		if(size==12)temp=oled_asc2_1206[chr][t];  //1206 Size
		else temp=oled_asc2_1608[chr][t];		 //1608 Size
        for(t1=0;t1<8;t1++)
		{
			if(temp&0x80)OLED_DrawPoint(x,y,mode);
			else OLED_DrawPoint(x,y,!mode);
			temp<<=1;
			y++;
			if((y-y0)==size)
			{
				y=y0;
				x++;
				break;
			}
		}
    }
}

uint32_t oled_pow(uint8_t m,uint8_t n)
{
	uint32_t result=1;
	while(n--)result*=m;
	return result;
}

/**************************************************************************
Show Two Number
**************************************************************************/
void OLED_ShowNumber(uint8_t x,uint8_t y,uint32_t num,uint8_t len,uint8_t size)
{
	uint8_t t,temp;
	uint8_t enshow=0;
	for(t=0;t<len;t++)
	{
		temp=(num/oled_pow(10,len-t-1))%10;
		if(enshow==0&&t<(len-1))
		{
			if(temp==0)
			{
				OLED_ShowChar(x+(size/2)*t,y,' ',size,1);
				continue;
			}else enshow=1;

		}
	 	OLED_ShowChar(x+(size/2)*t,y,temp+'0',size,1);
	}
}
/**************************************************************************
Show The String
**************************************************************************/
void OLED_ShowString(uint8_t x,uint8_t y,const uint8_t *p)
{
#define MAX_CHAR_POSX 122
#define MAX_CHAR_POSY 58
    while(*p!='\0')
    {
        if(x>MAX_CHAR_POSX){x=0;y+=16;}
        if(y>MAX_CHAR_POSY){y=x=0;OLED_Clear();}
        OLED_ShowChar(x,y,*p,12,1);
        x+=8;
        p++;
    }
}

void OLED_Init(void)
{
	HAL_PWR_EnableBkUpAccess(); //Enable access to the RTC and Backup Register
	__HAL_RCC_LSE_CONFIG(RCC_LSE_OFF); //turn OFF the LSE oscillator, LSERDY flag goes low after 6 LSE oscillator clock cycles.
	                                   //LSE oscillator switch off to let PC13 PC14 PC15 be IO


	HAL_PWR_DisableBkUpAccess();

	OLED_RST_Clr();
	HAL_Delay(100);
	OLED_RST_Set();

	OLED_WR_Byte(0xAE,OLED_CMD); //Off Display

	OLED_WR_Byte(0xD5,OLED_CMD); //Set Oscillator Division
	OLED_WR_Byte(80,OLED_CMD);    //[3:0]: divide ratio of the DCLK, [7:4], set the oscillator frequency. Reset
	OLED_WR_Byte(0xA8,OLED_CMD); //multiplex ratio
	OLED_WR_Byte(0X3F,OLED_CMD); //duty = 0X3F(1/64)
	OLED_WR_Byte(0xD3,OLED_CMD);  //set display offset
	OLED_WR_Byte(0X00,OLED_CMD); //0

	OLED_WR_Byte(0x40,OLED_CMD); //set display start line [5:0]- from 0-63. RESET

	OLED_WR_Byte(0x8D,OLED_CMD); //Set charge pump
	OLED_WR_Byte(0x14,OLED_CMD); //Enable Charge Pump
	OLED_WR_Byte(0x20,OLED_CMD); //Set Memory Addressing Mode
	OLED_WR_Byte(0x02,OLED_CMD); //Page Addressing Mode (RESET)
	OLED_WR_Byte(0xA1,OLED_CMD); //Set segment remap, bit0:0,0->0;1,0->127;
	OLED_WR_Byte(0xC0,OLED_CMD); //Set COM Output Scan Direction
	OLED_WR_Byte(0xDA,OLED_CMD); //Set COM Pins
	OLED_WR_Byte(0x12,OLED_CMD); //[5:4] setting

	OLED_WR_Byte(0x81,OLED_CMD); //Contrast Control
	OLED_WR_Byte(0xEF,OLED_CMD); //1~256; Default: 0X7F
	OLED_WR_Byte(0xD9,OLED_CMD); //Set Pre-charge Period
	OLED_WR_Byte(0xf1,OLED_CMD); //[3:0],PHASE 1;[7:4],PHASE 2;
	OLED_WR_Byte(0xDB,OLED_CMD); //Set VCOMH
	OLED_WR_Byte(0x30,OLED_CMD);  //[6:4] 000,0.65*vcc;001,0.77*vcc;011,0.83*vcc;

	OLED_WR_Byte(0xA4,OLED_CMD); //Enable display outputs according to the GDDRAM contents
	OLED_WR_Byte(0xA6,OLED_CMD); //Set normal display
	OLED_WR_Byte(0xAF,OLED_CMD); //DISPLAY ON
	OLED_Clear();
}


//OLED
int dir = 1;
uint8_t aRxBuffer[20];
uint8_t receiveBuffer[20];
int turnFinished = 1;
uint8_t buff[20];
uint8_t ICMAddr = 0x68;
int motorOnA = 0;
int motorOnB = 0;
double speedA = 0;
double speedB = 0;
int progress = 10;
int flag = 0;
uint16_t lengthFront[4096];
uint16_t lengthSide[4096];
int32_t distanceFront = 0;
int32_t distanceSide = 0;
uint8_t hello[40];
char bye[20];
uint8_t yay[40];

int total_distancea = 0;
int total_distanceb = 0;
int SPEED = 1200;
int count = 0;
int turnSPEED = 1200;
//int turnSpeed = 100;
double total_angle = 0;
double global_angle = 0;
int turnDir;
uint32_t ir1;
int ir1_dist;
uint8_t us_dist = 0;
uint8_t us_dist_obst = 0;
int travel = 0;

char inOrOut='7';

void motorStop(){

	motorOnA = 0;
	motorOnB = 0;

}


void motorForward(int target_distance, int dirA){


	dir = dirA;
	gyroInit();

	htim1.Instance -> CCR4 = 146.5;

	double encoder_count = 0;
	if(dir == 1){
		encoder_count = 1560.0;
	} else if(dir == -1) {
		encoder_count = 1560.0;
	}



    int pidVal =0;

    total_angle = 0;
    double Aint = 0;

    if(target_distance>10){
    	motorOnA = SPEED*dir;
    }
    else{
    	motorOnA = 1100*dir;
    }


	motorOnB = motorOnA;


				while(1){
				osDelayUntil(10);
				if(dirA == 1){
				pidVal = (int)(146.5 + (total_angle*10 + 0.00*Aint));


				} else if(dirA == -1) {
					pidVal = (int)(146.5 - (total_angle*10 + 0.00*Aint));

				}
				if(inOrOut=='7'){
					if(dirA == 1){
						if(pidVal <= 141){
							pidVal = 141;
						}
						if(pidVal >= 151){
							pidVal = 149;
						}
					}
					else if(dirA == -1){
						if(pidVal <= 143){
							pidVal = 142;
						}
						if(pidVal >= 151){
							pidVal = 149;
						}
					}
				}
				//OLED_ShowString(10,10, atoi(pidVal));
				htim1.Instance -> CCR4 = 146.5;
				osDelayUntil(5);
				htim1.Instance -> CCR4 = pidVal;
				osDelayUntil(5);
				Aint += total_angle;


				if(((total_distancea+total_distanceb)) >= 2*((target_distance)/21.04f)*encoder_count){

					motorStop();

					break;
				}

				}

//				while((total_distancea + total_distanceb) > 2*(target_distance/21.04)*encoder_count){
//									motorOnA = -1000*dir;
//									motorOnB = -1000*dir;
//									osDelayUntil(10);
//				}

				motorStop();
				//osDelay(100);
				total_distancea = 0;
				total_distanceb = 0;
				htim1.Instance -> CCR4 = 146.5;
}

void motorBStop(){


	HAL_TIM_PWM_Stop(&htim8, TIM_CHANNEL_2);
}

void motorAStart(){

	HAL_TIM_PWM_Start(&htim8, TIM_CHANNEL_1);


}

void motorBStart(){
	HAL_TIM_PWM_Start(&htim8, TIM_CHANNEL_2);
}



void motorAForward(){

	HAL_GPIO_WritePin(GPIOA, AIN2_Pin, GPIO_PIN_RESET);
	HAL_GPIO_WritePin(GPIOA, AIN1_Pin, GPIO_PIN_SET);


}
void motorAReverse(){

	HAL_GPIO_WritePin(GPIOA, AIN2_Pin, GPIO_PIN_SET);
	HAL_GPIO_WritePin(GPIOA, AIN1_Pin, GPIO_PIN_RESET);
}
void motorBForward(){

	HAL_GPIO_WritePin(GPIOA, BIN2_Pin, GPIO_PIN_RESET);
	HAL_GPIO_WritePin(GPIOA, BIN1_Pin, GPIO_PIN_SET);
}
void motorBReverse(){

	HAL_GPIO_WritePin(GPIOA, BIN2_Pin, GPIO_PIN_SET);
	HAL_GPIO_WritePin(GPIOA, BIN1_Pin, GPIO_PIN_RESET);
}

//void correctDirection(double target_angle){
//	double Aint = 0;
//	int pidVal;
//
//	//
//	pidVal = (int)(150 + ((total_angle - target_angle)*2.0 + 0.8*Aint));
//	if(pidVal <= 100){
//	pidVal = 100;
//	}
//
//	if(pidVal >= 200){
//		pidVal = 200;
//	}
//	Aint += total_angle - target_angle;
////	htim1.Instance -> CCR4 = 150;
////	osDelayUntil(10);
//	htim1.Instance -> CCR4 = pidVal;
//
//
//
//
//}


void readByte(uint8_t addr, uint8_t* data){
	buff[0] = addr;
	HAL_I2C_Master_Transmit(&hi2c1, ICMAddr<<1, buff, 1, 10);
	HAL_I2C_Master_Receive(&hi2c1, ICMAddr<<1, data, 2, 20);

}

void writeByte(uint8_t addr, uint8_t data){
	buff[0] = addr;
	buff[1] = data;
	HAL_I2C_Master_Transmit(&hi2c1, ICMAddr << 1, buff, 2, 20);
}

void gyroStart(){
	writeByte(0x07, 0x07);
	osDelayUntil(10);

	writeByte(0x07, 0x00);
	osDelayUntil(10);


}


void gyroInit(){

	writeByte(0x06, 0x00);
		osDelayUntil(10);
		writeByte(0x03, 0x80);
		osDelayUntil(10);
		writeByte(0x07, 0x07);
		osDelayUntil(10);
		writeByte(0x06, 0x01);
		osDelayUntil(10);
		writeByte(0x7F, 0x20);
		osDelayUntil(10);
		writeByte(0x01, 0x2F);
		osDelayUntil(10);
		writeByte(0x0, 0x00);
		osDelayUntil(10);
		writeByte(0x7F, 0x00);
		osDelayUntil(10);
		writeByte(0x07, 0x00);
		osDelayUntil(10);

}

//double abs(double a, double b){
//	if(a - b < 0){
//		return -1*(a-b);
//	} else {
//		return a-b;
//	}
//}





void motorForwardA(int target_distance, int condition){

	// WITH IR STOPPING CONDITION

	dir = 1;
	htim1.Instance -> CCR4 = 148;

	double encoder_count = 1560.0;

    total_angle = 0;
    if (target_distance==101)
    {
    	motorOnA = 4000*dir;
    	motorOnB = motorOnA;
    }
    else{
    	motorOnA = SPEED*dir;
    	motorOnB = motorOnA;
    }

			while(1){
					/*
				osDelayUntil(10);
				if(dirA == 1){
				pidVal = (int)(150 + (total_angle*10 + 0.00*Aint));


				} else if(dirA == -1) {
					pidVal = (int)(150 - (total_angle*10 + 0.00*Aint));

				}
				if(pidVal <= 140){
					pidVal = 140;
				}

				if(pidVal >= 165){
					pidVal = 165;
				}
				htim1.Instance -> CCR4 = 150;
				osDelayUntil(10);
				htim1.Instance -> CCR4 = pidVal;
				osDelayUntil(10);
				Aint += total_angle;*/


				if(((total_distancea+total_distanceb)) >= 2*((target_distance)/21.04f)*encoder_count || (us_dist < 32 || ir1_dist < 20)){
					motorStop();
					if (condition==1){
						travel = 2*((target_distance)/21.04f)*encoder_count - ((total_distancea+total_distanceb));
						travel = target_distance - ((travel/3120.0)*21.04);
					}
					break;
				}

			}

				/*while(total_distancea + total_distanceb > 2*(target_distance/21.04)*encoder_count){
									motorOnA = -1000*dir;
									motorOnB = -1000*dir;
									osDelayUntil(10);
				}
				motorStop();
				*/
			total_distancea = 0;
			total_distanceb = 0;
			htim1.Instance -> CCR4 = 150;
}


void motorForwardB(int target_distance, int dirA){

	// NO IR STOPPING CONDITION

	dir = dirA;
	htim1.Instance -> CCR4 = 150;

	double encoder_count = 0;
	if(dir == 1){
		encoder_count = 1560.0;
	} else if(dir == -1) {
		encoder_count = 1560.0;
	}


    int pidVal;

    total_angle = 0;
    double Aint = 0;
    if (target_distance==21){
    	motorOnA = 1500*dir;
    	motorOnB = motorOnA;
    }
    else{
    	motorOnA = (SPEED + 500)*dir;
    	motorOnB = motorOnA;
    }


				while(1){
				osDelayUntil(10);
				if(dirA == 1){
				pidVal = (int)(150 + (total_angle*10 + 0.00*Aint));


				} else if(dirA == -1) {
					pidVal = (int)(150 - (total_angle*10 + 0.00*Aint));

				}
				if(pidVal <= 140){
					pidVal = 140;
				}

				if(pidVal >= 165){
					pidVal = 165;
				}
				htim1.Instance -> CCR4 = 150;
				osDelayUntil(10);
				htim1.Instance -> CCR4 = pidVal;
				osDelayUntil(10);
				Aint += total_angle;


				if(((total_distancea+total_distanceb)) >= 2*((target_distance)/21.04f)*encoder_count){
					motorStop();
					break;
				}

				}


				/*while(total_distancea + total_distanceb > 2*(target_distance/21.04)*encoder_count){
									motorOnA = -1000*dir;
									motorOnB = -1000*dir;
									osDelayUntil(10);
				}*/
				motorStop();
				total_distancea = 0;
				total_distanceb = 0;
				htim1.Instance -> CCR4 = 150;
}


void motorRight(int target_angle){


		htim1.Instance -> CCR4 = 260;
		//gyroStart();


		osDelay(500);
		//gyroStart();

		turnDir = -1;
		total_angle =  0;
		motorOnA = turnSPEED;
		motorOnB = motorOnA;
		while(total_angle > -1*target_angle){
			osDelayUntil(10);
		}

		motorStop();

		double Aint = 0;


		/*
		while((int)total_angle < (-1*target_angle)){
			motorOnA = (int)(200*(total_angle + target_angle) + 0.1*Aint);
			motorOnB = motorOnA;
			Aint += total_angle + target_angle;

			osDelayUntil(10);
		}
		motorStop();
		*/
		total_distancea = 0;
		total_distanceb = 0;

		htim1.Instance -> CCR4 = 146.5;
		osDelay(100);

}

void motorLeft(int target_angle){

		htim1.Instance -> CCR4 =  100;
		//gyroStart();

		osDelay(500);
		//gyroStart();

        turnDir = 1;
		total_angle = 0;

		motorOnB = turnSPEED;
		motorOnA = motorOnB;
		double Aint = 0;

		while(total_angle < target_angle){
			osDelayUntil(10);
		}

		motorStop();

				/*
				while((int)total_angle > target_angle){
					//motorOnA = -500*(total_angle - target_angle);
					motorOnB = ((int)(200*(total_angle - target_angle) + 0.1*Aint))*-1;

					motorOnA = motorOnB;
					Aint += total_angle - target_angle;



					osDelayUntil(10);
				}

				motorStop();
				*/

		total_distancea = 0;
		total_distanceb = 0;

		htim1.Instance -> CCR4 =  146.5;
		osDelay(100);

}


void motorRightB(int target_angle){


		htim1.Instance -> CCR4 = 260;
		//gyroStart();


		osDelay(500);
		//gyroStart();

		turnDir = -1;
		total_angle =  0;
		motorOnA = 2500;
		motorOnB = 500;
//		motorOnA = 2400;
//		motorOnB = 400;

		while(total_angle > -1*target_angle){
			osDelayUntil(10);
		}

		motorStop();

		double Aint = 0;


		/*
		while((int)total_angle < (-1*target_angle)){
			motorOnA = (int)(200*(total_angle + target_angle) + 0.1*Aint);
			motorOnB = motorOnA;
			Aint += total_angle + target_angle;

			osDelayUntil(10);
		}
		motorStop();
		*/
		total_distancea = 0;
		total_distanceb = 0;

		htim1.Instance -> CCR4 = 150;
		osDelay(100);

}

void motorLeftB(int target_angle){

		htim1.Instance -> CCR4 =  100;
		//gyroStart();

		osDelay(500);
		//gyroStart();

        turnDir = 1;
		total_angle = 0;

		motorOnB = 2500;
		motorOnA = 500;
//		motorOnA = 2400;
//		motorOnB = 400;

		double Aint = 0;

		while(total_angle < target_angle){
			osDelayUntil(10);
		}

		motorStop();

				/*
				while((int)total_angle > target_angle){
					//motorOnA = -500*(total_angle - target_angle);
					motorOnB = ((int)(200*(total_angle - target_angle) + 0.1*Aint))*-1;

					motorOnA = motorOnB;
					Aint += total_angle - target_angle;



					osDelayUntil(10);
				}

				motorStop();
				*/

		total_distancea = 0;
		total_distanceb = 0;

		htim1.Instance -> CCR4 =  150;
		osDelay(100);

}

void motorRightC(int target_angle){


        //motorOnA = 6000;
		htim1.Instance -> CCR4 = 235;
		//gyroStart();



		osDelay(500);
		//gyroStart();

		turnDir = -1;
		total_angle =  0;
		motorOnA = 4500;
		motorOnB = motorOnA;
	while(total_angle > -1*target_angle){
			osDelayUntil(10);
		}

		motorStop();

		double Aint = 0;



		while((int)total_angle < (-1*target_angle)){
			motorOnA = (int)(200*(total_angle + target_angle) + 0.1*Aint);
			motorOnB = motorOnA;
			Aint += total_angle + target_angle;

			osDelayUntil(10);
		}
		motorStop();
		total_distancea = 0;
		total_distanceb = 0;






			htim1.Instance -> CCR4 = 146.5;
			osDelay(500);





}
void motorLeftC(int target_angle){







		htim1.Instance -> CCR4 =  95;
		//gyroStart();


		osDelay(500);
		//gyroStart();


        turnDir = 1;
		total_angle = 0;

		motorOnB = 4500;
		motorOnA = motorOnB;
		double Aint = 0;

		while(total_angle < target_angle){
					osDelayUntil(10);
				}

				motorStop();




				while((int)total_angle > target_angle){
					//motorOnA = -500*(total_angle - target_angle);
					motorOnB = ((int)(200*(total_angle - target_angle) + 0.1*Aint))*-1;

					motorOnA = motorOnB;
					Aint += total_angle - target_angle;



					osDelayUntil(10);
				}
				motorStop();
				total_distancea = 0;
						total_distanceb = 0;



				htim1.Instance -> CCR4 =  146.5;
				osDelay(500);






}


void backRight(int target_angle){


		htim1.Instance -> CCR4 = 260;
		//gyroStart();


		osDelay(500);
		//gyroStart();
		turnDir = 1;
		total_angle = 0;
		motorOnA = -1*turnSPEED;
		motorOnB = motorOnA;
		double Aint = 0;

		while(total_angle < target_angle){
					osDelayUntil(10);
				}

				motorStop();

				/*
				while((int)total_angle > target_angle){
					//motorOnA = 500*(total_angle - target_angle);

					motorOnA = ((int)(200*(total_angle - target_angle) + 0.1*Aint));
					Aint += total_angle -  target_angle;
					motorOnB = motorOnA;


					osDelayUntil(10);
				}
				motorStop();
				*/
				total_distancea = 0;
				total_distanceb = 0;

				htim1.Instance -> CCR4 = 150;
				osDelay(100);

}
void backLeft(int target_angle){


		htim1.Instance -> CCR4 =  100;
		//gyroStart();

		osDelay(500);
		//gyroStart();

		turnDir = -1;
		total_angle = 0;
		motorOnB = -1*turnSPEED;
		motorOnA = motorOnB;
		double Aint = 0;

		while(total_angle > -1*target_angle){
					osDelayUntil(10);
				}

				motorStop();

				/*
				while((int)total_angle < (-1*target_angle)){
					//motorOnB = -500*(total_angle + target_angle);

					motorOnB = -1*((int)(200*(total_angle + target_angle) + 0.1*Aint));
					motorOnA = motorOnB;

					Aint += total_angle + target_angle;

					osDelayUntil(10);
				}
				motorStop();
				*/
				total_distancea = 0;
						total_distanceb = 0;

				htim1.Instance -> CCR4 = 150;
				osDelay(100);

}



void motorBack(int target_distance){

		htim1.Instance -> CCR4 = 147;
		osDelayUntil(200);
		motorForward(target_distance, -1);

}

void motorBackA(int target){

	int disto = 100;
	dir = -1;
	htim1.Instance -> CCR4 = 150;
	osDelayUntil(200);
	double encoder_count = 1560.0;

    total_angle = 0;
    double Aint = 0;
	motorOnA = 800*dir;
	motorOnB = motorOnA;



	while(1){
					/*
				osDelayUntil(10);
				if(dirA == 1){
				pidVal = (int)(150 + (total_angle*10 + 0.00*Aint));


				} else if(dirA == -1) {
					pidVal = (int)(150 - (total_angle*10 + 0.00*Aint));

				}
				if(pidVal <= 140){
					pidVal = 140;
				}

				if(pidVal >= 165){
					pidVal = 165;
				}
				htim1.Instance -> CCR4 = 150;
				osDelayUntil(10);
				htim1.Instance -> CCR4 = pidVal;
				osDelayUntil(10);
				Aint += total_angle;*/


				if(((total_distancea+total_distanceb)) >= 2*((disto)/21.04f)*encoder_count || (us_dist > target && ir1_dist > 15)){
					motorStop();
					break;
					}

				}

				/*while(total_distancea + total_distanceb > 2*(target_distance/21.04)*encoder_count){
									motorOnA = -1000*dir;
									motorOnB = -1000*dir;
									osDelayUntil(10);
				}
				motorStop();
				*/
				total_distancea = 0;
				total_distanceb = 0;
				htim1.Instance -> CCR4 = 150;
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
  MX_TIM8_Init();
  MX_TIM2_Init();
  MX_TIM1_Init();
  MX_USART3_UART_Init();
  MX_TIM3_Init();
  MX_TIM4_Init();
  MX_I2C1_Init();
  MX_DMA_Init();
  MX_ADC1_Init();
  /* USER CODE BEGIN 2 */

  HAL_TIM_IC_Start(&htim4, TIM_CHANNEL_1);


  OLED_Init();

  HAL_TIM_Encoder_Start(&htim2, TIM_CHANNEL_ALL);
  	HAL_TIM_Encoder_Start(&htim3, TIM_CHANNEL_ALL);

  			HAL_TIM_PWM_Start(&htim8, TIM_CHANNEL_1);

  			HAL_TIM_PWM_Start(&htim8, TIM_CHANNEL_2);

  			HAL_TIM_PWM_Start(&htim1, TIM_CHANNEL_4);

  HAL_UART_Receive_IT(&huart3, (uint8_t *) aRxBuffer, 4);

  /* USER CODE END 2 */

  /* Init scheduler */
  osKernelInitialize();

  /* USER CODE BEGIN RTOS_MUTEX */
  /* add mutexes, ... */
  /* USER CODE END RTOS_MUTEX */

  /* USER CODE BEGIN RTOS_SEMAPHORES */
  /* add semaphores, ... */
  /* USER CODE END RTOS_SEMAPHORES */

  /* USER CODE BEGIN RTOS_TIMERS */
  /* start timers, add new ones, ... */
  /* USER CODE END RTOS_TIMERS */

  /* USER CODE BEGIN RTOS_QUEUES */
  /* add queues, ... */
  /* USER CODE END RTOS_QUEUES */

  /* Create the thread(s) */
  /* creation of defaultTask */
  defaultTaskHandle = osThreadNew(StartDefaultTask, NULL, &defaultTask_attributes);

  /* creation of EncoderTask */
  EncoderTaskHandle = osThreadNew(encoder_task, NULL, &EncoderTask_attributes);

  /* creation of MotorTask */
  MotorTaskHandle = osThreadNew(motorTask, NULL, &MotorTask_attributes);

  /* creation of GyroTask */
  GyroTaskHandle = osThreadNew(gyroTask, NULL, &GyroTask_attributes);

  /* creation of UltrasoundTask */
  UltrasoundTaskHandle = osThreadNew(ultrasoundTask, NULL, &UltrasoundTask_attributes);

  /* USER CODE BEGIN RTOS_THREADS */
  /* add threads, ... */
  /* USER CODE END RTOS_THREADS */

  /* USER CODE BEGIN RTOS_EVENTS */
  /* add events, ... */
  /* USER CODE END RTOS_EVENTS */

  /* Start scheduler */
  osKernelStart();

  /* We should never get here as control is now taken by the scheduler */
  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
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

  /** Configure the main internal regulator output voltage
  */
  __HAL_RCC_PWR_CLK_ENABLE();
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);
  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_NONE;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }
  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_HSI;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV2;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_0) != HAL_OK)
  {
    Error_Handler();
  }
}

/**
  * @brief ADC1 Initialization Function
  * @param None
  * @retval None
  */
static void MX_ADC1_Init(void)
{

  /* USER CODE BEGIN ADC1_Init 0 */

  /* USER CODE END ADC1_Init 0 */

  ADC_ChannelConfTypeDef sConfig = {0};

  /* USER CODE BEGIN ADC1_Init 1 */

  /* USER CODE END ADC1_Init 1 */
  /** Configure the global features of the ADC (Clock, Resolution, Data Alignment and number of conversion)
  */
  hadc1.Instance = ADC1;
  hadc1.Init.ClockPrescaler = ADC_CLOCK_SYNC_PCLK_DIV2;
  hadc1.Init.Resolution = ADC_RESOLUTION_12B;
  hadc1.Init.ScanConvMode = DISABLE;
  hadc1.Init.ContinuousConvMode = DISABLE;
  hadc1.Init.DiscontinuousConvMode = DISABLE;
  hadc1.Init.ExternalTrigConvEdge = ADC_EXTERNALTRIGCONVEDGE_NONE;
  hadc1.Init.ExternalTrigConv = ADC_SOFTWARE_START;
  hadc1.Init.DataAlign = ADC_DATAALIGN_RIGHT;
  hadc1.Init.NbrOfConversion = 1;
  hadc1.Init.DMAContinuousRequests = DISABLE;
  hadc1.Init.EOCSelection = ADC_EOC_SINGLE_CONV;
  if (HAL_ADC_Init(&hadc1) != HAL_OK)
  {
    Error_Handler();
  }
  /** Configure for the selected ADC regular channel its corresponding rank in the sequencer and its sample time.
  */
  sConfig.Channel = ADC_CHANNEL_0;
  sConfig.Rank = 1;
  sConfig.SamplingTime = ADC_SAMPLETIME_3CYCLES;
  if (HAL_ADC_ConfigChannel(&hadc1, &sConfig) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN ADC1_Init 2 */

  /* USER CODE END ADC1_Init 2 */

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
  * @brief TIM1 Initialization Function
  * @param None
  * @retval None
  */
static void MX_TIM1_Init(void)
{

  /* USER CODE BEGIN TIM1_Init 0 */

  /* USER CODE END TIM1_Init 0 */

  TIM_ClockConfigTypeDef sClockSourceConfig = {0};
  TIM_MasterConfigTypeDef sMasterConfig = {0};
  TIM_OC_InitTypeDef sConfigOC = {0};
  TIM_BreakDeadTimeConfigTypeDef sBreakDeadTimeConfig = {0};

  /* USER CODE BEGIN TIM1_Init 1 */

  /* USER CODE END TIM1_Init 1 */
  htim1.Instance = TIM1;
  htim1.Init.Prescaler = 160;
  htim1.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim1.Init.Period = 1000;
  htim1.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
  htim1.Init.RepetitionCounter = 0;
  htim1.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_ENABLE;
  if (HAL_TIM_Base_Init(&htim1) != HAL_OK)
  {
    Error_Handler();
  }
  sClockSourceConfig.ClockSource = TIM_CLOCKSOURCE_INTERNAL;
  if (HAL_TIM_ConfigClockSource(&htim1, &sClockSourceConfig) != HAL_OK)
  {
    Error_Handler();
  }
  if (HAL_TIM_PWM_Init(&htim1) != HAL_OK)
  {
    Error_Handler();
  }
  sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
  sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
  if (HAL_TIMEx_MasterConfigSynchronization(&htim1, &sMasterConfig) != HAL_OK)
  {
    Error_Handler();
  }
  sConfigOC.OCMode = TIM_OCMODE_PWM1;
  sConfigOC.Pulse = 0;
  sConfigOC.OCPolarity = TIM_OCPOLARITY_HIGH;
  sConfigOC.OCFastMode = TIM_OCFAST_ENABLE;
  sConfigOC.OCIdleState = TIM_OCIDLESTATE_RESET;
  sConfigOC.OCNIdleState = TIM_OCNIDLESTATE_RESET;
  if (HAL_TIM_PWM_ConfigChannel(&htim1, &sConfigOC, TIM_CHANNEL_4) != HAL_OK)
  {
    Error_Handler();
  }
  sBreakDeadTimeConfig.OffStateRunMode = TIM_OSSR_DISABLE;
  sBreakDeadTimeConfig.OffStateIDLEMode = TIM_OSSI_DISABLE;
  sBreakDeadTimeConfig.LockLevel = TIM_LOCKLEVEL_OFF;
  sBreakDeadTimeConfig.DeadTime = 0;
  sBreakDeadTimeConfig.BreakState = TIM_BREAK_DISABLE;
  sBreakDeadTimeConfig.BreakPolarity = TIM_BREAKPOLARITY_HIGH;
  sBreakDeadTimeConfig.AutomaticOutput = TIM_AUTOMATICOUTPUT_DISABLE;
  if (HAL_TIMEx_ConfigBreakDeadTime(&htim1, &sBreakDeadTimeConfig) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN TIM1_Init 2 */

  /* USER CODE END TIM1_Init 2 */
  HAL_TIM_MspPostInit(&htim1);

}

/**
  * @brief TIM2 Initialization Function
  * @param None
  * @retval None
  */
static void MX_TIM2_Init(void)
{

  /* USER CODE BEGIN TIM2_Init 0 */

  /* USER CODE END TIM2_Init 0 */

  TIM_Encoder_InitTypeDef sConfig = {0};
  TIM_MasterConfigTypeDef sMasterConfig = {0};

  /* USER CODE BEGIN TIM2_Init 1 */

  /* USER CODE END TIM2_Init 1 */
  htim2.Instance = TIM2;
  htim2.Init.Prescaler = 0;
  htim2.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim2.Init.Period = 65535;
  htim2.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
  htim2.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
  sConfig.EncoderMode = TIM_ENCODERMODE_TI12;
  sConfig.IC1Polarity = TIM_ICPOLARITY_RISING;
  sConfig.IC1Selection = TIM_ICSELECTION_DIRECTTI;
  sConfig.IC1Prescaler = TIM_ICPSC_DIV1;
  sConfig.IC1Filter = 0;
  sConfig.IC2Polarity = TIM_ICPOLARITY_RISING;
  sConfig.IC2Selection = TIM_ICSELECTION_DIRECTTI;
  sConfig.IC2Prescaler = TIM_ICPSC_DIV1;
  sConfig.IC2Filter = 0;
  if (HAL_TIM_Encoder_Init(&htim2, &sConfig) != HAL_OK)
  {
    Error_Handler();
  }
  sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
  sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
  if (HAL_TIMEx_MasterConfigSynchronization(&htim2, &sMasterConfig) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN TIM2_Init 2 */

  /* USER CODE END TIM2_Init 2 */

}

/**
  * @brief TIM3 Initialization Function
  * @param None
  * @retval None
  */
static void MX_TIM3_Init(void)
{

  /* USER CODE BEGIN TIM3_Init 0 */

  /* USER CODE END TIM3_Init 0 */

  TIM_Encoder_InitTypeDef sConfig = {0};
  TIM_MasterConfigTypeDef sMasterConfig = {0};

  /* USER CODE BEGIN TIM3_Init 1 */

  /* USER CODE END TIM3_Init 1 */
  htim3.Instance = TIM3;
  htim3.Init.Prescaler = 0;
  htim3.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim3.Init.Period = 65535;
  htim3.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
  htim3.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
  sConfig.EncoderMode = TIM_ENCODERMODE_TI12;
  sConfig.IC1Polarity = TIM_ICPOLARITY_RISING;
  sConfig.IC1Selection = TIM_ICSELECTION_DIRECTTI;
  sConfig.IC1Prescaler = TIM_ICPSC_DIV1;
  sConfig.IC1Filter = 0;
  sConfig.IC2Polarity = TIM_ICPOLARITY_RISING;
  sConfig.IC2Selection = TIM_ICSELECTION_DIRECTTI;
  sConfig.IC2Prescaler = TIM_ICPSC_DIV1;
  sConfig.IC2Filter = 0;
  if (HAL_TIM_Encoder_Init(&htim3, &sConfig) != HAL_OK)
  {
    Error_Handler();
  }
  sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
  sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
  if (HAL_TIMEx_MasterConfigSynchronization(&htim3, &sMasterConfig) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN TIM3_Init 2 */

  /* USER CODE END TIM3_Init 2 */

}

/**
  * @brief TIM4 Initialization Function
  * @param None
  * @retval None
  */
static void MX_TIM4_Init(void)
{

  /* USER CODE BEGIN TIM4_Init 0 */

  /* USER CODE END TIM4_Init 0 */

  TIM_ClockConfigTypeDef sClockSourceConfig = {0};
  TIM_MasterConfigTypeDef sMasterConfig = {0};
  TIM_IC_InitTypeDef sConfigIC = {0};

  /* USER CODE BEGIN TIM4_Init 1 */

  /* USER CODE END TIM4_Init 1 */
  htim4.Instance = TIM4;
  htim4.Init.Prescaler = 16-1;
  htim4.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim4.Init.Period = 0xffff-1;
  htim4.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
  htim4.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
  if (HAL_TIM_Base_Init(&htim4) != HAL_OK)
  {
    Error_Handler();
  }
  sClockSourceConfig.ClockSource = TIM_CLOCKSOURCE_INTERNAL;
  if (HAL_TIM_ConfigClockSource(&htim4, &sClockSourceConfig) != HAL_OK)
  {
    Error_Handler();
  }
  if (HAL_TIM_IC_Init(&htim4) != HAL_OK)
  {
    Error_Handler();
  }
  sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
  sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
  if (HAL_TIMEx_MasterConfigSynchronization(&htim4, &sMasterConfig) != HAL_OK)
  {
    Error_Handler();
  }
  sConfigIC.ICPolarity = TIM_INPUTCHANNELPOLARITY_RISING;
  sConfigIC.ICSelection = TIM_ICSELECTION_DIRECTTI;
  sConfigIC.ICPrescaler = TIM_ICPSC_DIV1;
  sConfigIC.ICFilter = 0;
  if (HAL_TIM_IC_ConfigChannel(&htim4, &sConfigIC, TIM_CHANNEL_1) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN TIM4_Init 2 */

  /* USER CODE END TIM4_Init 2 */

}

/**
  * @brief TIM8 Initialization Function
  * @param None
  * @retval None
  */
static void MX_TIM8_Init(void)
{

  /* USER CODE BEGIN TIM8_Init 0 */

  /* USER CODE END TIM8_Init 0 */

  TIM_ClockConfigTypeDef sClockSourceConfig = {0};
  TIM_MasterConfigTypeDef sMasterConfig = {0};
  TIM_OC_InitTypeDef sConfigOC = {0};
  TIM_BreakDeadTimeConfigTypeDef sBreakDeadTimeConfig = {0};

  /* USER CODE BEGIN TIM8_Init 1 */

  /* USER CODE END TIM8_Init 1 */
  htim8.Instance = TIM8;
  htim8.Init.Prescaler = 0;
  htim8.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim8.Init.Period = 7199;
  htim8.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
  htim8.Init.RepetitionCounter = 0;
  htim8.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
  if (HAL_TIM_Base_Init(&htim8) != HAL_OK)
  {
    Error_Handler();
  }
  sClockSourceConfig.ClockSource = TIM_CLOCKSOURCE_INTERNAL;
  if (HAL_TIM_ConfigClockSource(&htim8, &sClockSourceConfig) != HAL_OK)
  {
    Error_Handler();
  }
  if (HAL_TIM_PWM_Init(&htim8) != HAL_OK)
  {
    Error_Handler();
  }
  sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
  sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
  if (HAL_TIMEx_MasterConfigSynchronization(&htim8, &sMasterConfig) != HAL_OK)
  {
    Error_Handler();
  }
  sConfigOC.OCMode = TIM_OCMODE_PWM1;
  sConfigOC.Pulse = 0;
  sConfigOC.OCPolarity = TIM_OCPOLARITY_HIGH;
  sConfigOC.OCNPolarity = TIM_OCNPOLARITY_HIGH;
  sConfigOC.OCFastMode = TIM_OCFAST_DISABLE;
  sConfigOC.OCIdleState = TIM_OCIDLESTATE_RESET;
  sConfigOC.OCNIdleState = TIM_OCNIDLESTATE_RESET;
  if (HAL_TIM_PWM_ConfigChannel(&htim8, &sConfigOC, TIM_CHANNEL_1) != HAL_OK)
  {
    Error_Handler();
  }
  if (HAL_TIM_PWM_ConfigChannel(&htim8, &sConfigOC, TIM_CHANNEL_2) != HAL_OK)
  {
    Error_Handler();
  }
  sBreakDeadTimeConfig.OffStateRunMode = TIM_OSSR_DISABLE;
  sBreakDeadTimeConfig.OffStateIDLEMode = TIM_OSSI_DISABLE;
  sBreakDeadTimeConfig.LockLevel = TIM_LOCKLEVEL_OFF;
  sBreakDeadTimeConfig.DeadTime = 0;
  sBreakDeadTimeConfig.BreakState = TIM_BREAK_DISABLE;
  sBreakDeadTimeConfig.BreakPolarity = TIM_BREAKPOLARITY_HIGH;
  sBreakDeadTimeConfig.AutomaticOutput = TIM_AUTOMATICOUTPUT_DISABLE;
  if (HAL_TIMEx_ConfigBreakDeadTime(&htim8, &sBreakDeadTimeConfig) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN TIM8_Init 2 */

  /* USER CODE END TIM8_Init 2 */
  HAL_TIM_MspPostInit(&htim8);

}

/**
  * @brief USART3 Initialization Function
  * @param None
  * @retval None
  */
static void MX_USART3_UART_Init(void)
{

  /* USER CODE BEGIN USART3_Init 0 */

  /* USER CODE END USART3_Init 0 */

  /* USER CODE BEGIN USART3_Init 1 */

  /* USER CODE END USART3_Init 1 */
  huart3.Instance = USART3;
  huart3.Init.BaudRate = 115200;
  huart3.Init.WordLength = UART_WORDLENGTH_8B;
  huart3.Init.StopBits = UART_STOPBITS_1;
  huart3.Init.Parity = UART_PARITY_NONE;
  huart3.Init.Mode = UART_MODE_TX_RX;
  huart3.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart3.Init.OverSampling = UART_OVERSAMPLING_16;
  if (HAL_UART_Init(&huart3) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN USART3_Init 2 */

  /* USER CODE END USART3_Init 2 */

}

/**
  * Enable DMA controller clock
  */
static void MX_DMA_Init(void)
{

  /* DMA controller clock enable */
  __HAL_RCC_DMA2_CLK_ENABLE();

  /* DMA interrupt init */
  /* DMA2_Stream0_IRQn interrupt configuration */
  HAL_NVIC_SetPriority(DMA2_Stream0_IRQn, 5, 0);
  HAL_NVIC_EnableIRQ(DMA2_Stream0_IRQn);

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
  __HAL_RCC_GPIOE_CLK_ENABLE();
  __HAL_RCC_GPIOH_CLK_ENABLE();
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOC_CLK_ENABLE();
  __HAL_RCC_GPIOD_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOE, OLED_SCL_Pin|OLED_SDA_Pin|OLED_RST_Pin|OLED_DC_Pin
                          |LED3_Pin|GPIO_PIN_11, GPIO_PIN_RESET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOA, AIN2_Pin|AIN1_Pin|BIN1_Pin|BIN2_Pin
                          |GPIO_PIN_8, GPIO_PIN_RESET);

  /*Configure GPIO pins : OLED_SCL_Pin OLED_SDA_Pin OLED_RST_Pin OLED_DC_Pin
                           LED3_Pin */
  GPIO_InitStruct.Pin = OLED_SCL_Pin|OLED_SDA_Pin|OLED_RST_Pin|OLED_DC_Pin
                          |LED3_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOE, &GPIO_InitStruct);

  /*Configure GPIO pins : AIN2_Pin AIN1_Pin BIN1_Pin BIN2_Pin
                           PA8 */
  GPIO_InitStruct.Pin = AIN2_Pin|AIN1_Pin|BIN1_Pin|BIN2_Pin
                          |GPIO_PIN_8;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  /*Configure GPIO pin : PE11 */
  GPIO_InitStruct.Pin = GPIO_PIN_11;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
  HAL_GPIO_Init(GPIOE, &GPIO_InitStruct);

  /*Configure GPIO pin : IRsensor_Pin */
  GPIO_InitStruct.Pin = IRsensor_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(IRsensor_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pin : Echo_Pin */
  GPIO_InitStruct.Pin = Echo_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(Echo_GPIO_Port, &GPIO_InitStruct);

}

/* USER CODE BEGIN 4 */



char direction;
int sensor_flag = 0;
int edgeCount = 0;

uint32_t t1;
uint32_t t2;
int magnitude;
int sensorDistanceL = 0;
int distanceA;
int distanceB;
int sensorDistanceR = 0;
int batteryLevel = 0;

uint32_t IC_Val1 = 0;
uint32_t IC_Val2 = 0;
uint32_t Difference = 0;
uint8_t Is_First_Captured = 0;  // is the first value captured ?

#define TRIG_PIN GPIO_PIN_11
#define TRIG_PORT GPIOE

// Callback Function

void HAL_TIM_IC_CaptureCallback(TIM_HandleTypeDef *htim)
{
	if (htim->Channel == HAL_TIM_ACTIVE_CHANNEL_1)  // if the interrupt source is channel1
	{
		if (Is_First_Captured==0) // if the first value is not captured
		{
			IC_Val1 = HAL_TIM_ReadCapturedValue(htim, TIM_CHANNEL_1); // read the first value
			Is_First_Captured = 1;  // set the first captured as true
			// Now change the polarity to falling edge
			__HAL_TIM_SET_CAPTUREPOLARITY(htim, TIM_CHANNEL_1, TIM_INPUTCHANNELPOLARITY_FALLING);
		}

		else if (Is_First_Captured==1)   // if the first is already captured
		{
			IC_Val2 = HAL_TIM_ReadCapturedValue(htim, TIM_CHANNEL_1);  // read second value
			__HAL_TIM_SET_COUNTER(htim, 0);  // reset the counter

			if (IC_Val2 > IC_Val1)
			{
				Difference = IC_Val2-IC_Val1;
			}

			else if (IC_Val1 > IC_Val2)
			{
				Difference = (0xffff - IC_Val1) + IC_Val2;
			}

			us_dist = Difference * .034/2;
			Is_First_Captured = 0; // set it back to false

			// set polarity to rising edge
			__HAL_TIM_SET_CAPTUREPOLARITY(htim, TIM_CHANNEL_1, TIM_INPUTCHANNELPOLARITY_RISING);
			__HAL_TIM_DISABLE_IT(&htim4, TIM_IT_CC1);
		}
	}
}

//Echo Pin

void delay(uint16_t time)
{
	__HAL_TIM_SET_COUNTER(&htim4, 0);
	while(__HAL_TIM_GET_COUNTER(&htim4) < time);
}

void HCSR04_Read(void)
{
	HAL_GPIO_WritePin(TRIG_PORT, TRIG_PIN, GPIO_PIN_SET);  // pull the TRIG pin HIGH
	osDelay(1);  // wait for 10 us
	HAL_GPIO_WritePin(TRIG_PORT, TRIG_PIN, GPIO_PIN_RESET);  // pull the TRIG pin low
	__HAL_TIM_ENABLE_IT(&htim4, TIM_IT_CC1); // Enable Interrupt
}


void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart){
	UNUSED(huart);


	//HAL_UART_Transmit(&huart3, (uint8_t *) &aRxBuffer[0], 4, 0xFFFF);

	if(flag == 0){
		direction = (char) aRxBuffer[0];
		magnitude = (int)(aRxBuffer[1] - '0')*100 + (int)((char)aRxBuffer[2] - '0')*10 + (int)(aRxBuffer[3] - '0');
		flag = 1;
	}

	HAL_UART_Receive_IT(&huart3, (uint8_t *) aRxBuffer, 4);

}


void HAL_ADC_ConvCpltCallback(ADC_HandleTypeDef* hadc){
//	if(hadc == &hadc1){
//	lengthFront = 0;
//
//
//	for(int i = 0; i< 4096; i++){
//
//			lengthFront += distanceFront[i];
//
//
//	}
//
//	lengthFront = lengthFront/4096.0;
//	}
//
//	if(hadc == &hadc2){
//		lengthSide = 0;
//		for(int i = 0; i < 4096; i++){
//			lengthSide += distanceSide[i];
//		}
//
//		lengthSide = lengthSide/4096.0;
//	}



}


/* USER CODE END 4 */

/* USER CODE BEGIN Header_StartDefaultTask */
/**
  * @brief  Function implementing the defaultTask thread.
  * @param  argument: Not used
  * @retval None
  */
/* USER CODE END Header_StartDefaultTask */
void StartDefaultTask(void *argument)
{

	htim1.Instance -> CCR4 = 146.5;

  for(;;)
  {


	  HAL_UART_Receive_IT(&huart3,(uint8_t *) aRxBuffer, 4);
	  sprintf(receiveBuffer, "%s\0", aRxBuffer);
	  OLED_ShowString(10,10,receiveBuffer);
	  HCSR04_Read();

	  HAL_ADC_Start(&hadc1);
	  HAL_ADC_PollForConversion(&hadc1, 10);
	  ir1 = HAL_ADC_GetValue(&hadc1);
	  ir1_dist = roundf(-14.16*log(ir1)+122.67);
	  HAL_ADC_Stop(&hadc1);

	  sprintf(hello,"IR Dist:%dcm",ir1_dist);
	  OLED_ShowString(10,20,hello);

	  sprintf(yay, "US Dist:%dcm",us_dist);
	  OLED_ShowString(10,30,yay);

	  sprintf(bye, "angle %5d \0", (int)(total_angle));
	  OLED_ShowString(10,40, bye);

	  OLED_Refresh_Gram();
	  osDelay(5);

  }
  /* USER CODE END 5 */
}

/* USER CODE BEGIN Header_encoder_task */
/**
* @brief Function implementing the EncoderTask thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_encoder_task */
void encoder_task(void *argument)
{
  /* USER CODE BEGIN encoder_task */
  /* Infinite loop */
	//motorLeft(90);

	while(1){

//	if(flag){
////		gyroInit();
////		osDelay(100);
//		if(direction == 'w'){
//			global_angle = 0;
//			motorForwardA(magnitude, 1);
//		}
//
//		if (direction == 'b'){
//			global_angle = 0;
//			motorBackA(magnitude);
//		}
//
//		if(direction == 'f'){
//			global_angle = 0;
//			motorForwardA(200,0);
//			osDelay(500);
//			motorBackA(22);
//
//		}
//
//		if(direction == 's'){
//			dir = -1;
//         	motorBack(magnitude);
//		}
//
//		if(direction == 'a'){
//			global_angle = 0;
//			motorLeft(magnitude);
//		}
//
//		if(direction == 'd'){
//			global_angle = 0;
//			motorRight(magnitude);
//		}
//
//		if(direction == 'q'){
//			global_angle = 0;
//			motorLeftB(magnitude);
//		}
//
//		if(direction == 'e'){
//			global_angle = 0;
//			motorRightB(magnitude);
//		}
//
//		if(direction == 'j'){
//			// 1st obstacle left turn
//			global_angle = 0;
//			motorLeftB(55);
//			motorRight(119);
//			motorLeftB(28);
//			backRight(28);
//			motorForwardB(21,1);
//			osDelay(100);
//			motorBackA(25);
//			osDelay(100);
//			us_dist_obst = us_dist;
//			if (us_dist_obst > 80)
//			{
//				motorForwardB(31,1);
//			}
//		}
//
//		if(direction == 'k'){
//			// 1st obstacle right turn
//			global_angle = 0;
//			motorRightB(54);
//			motorLeft(119);
//			motorRightB(25);
//			backLeft(29);
//			motorForwardB(21,1);
//			osDelay(100);
//			motorBackA(25);
//			osDelay(100);
//			us_dist_obst = us_dist;
//			if (us_dist_obst > 80)
//			{
//				motorForwardB(31,1);
//			}
//		}
//
//		if(direction == 'n'){
//
//			// 2nd obstacle left turn
//			global_angle = 0;
//			motorForwardA(200,1);
//			osDelay(100);
//			motorBackA(30);
//			motorLeftB(88);
//			motorForwardA(10,0);
////			motorForwardA(40,0);
////			osDelay(100);
////			motorBackA(37);
//			//motorRightB(209);
//			motorRight(195);
//			motorForwardB(55,1);
//			//motorForwardA(60,0);
//			//motorBackA(40);
//			motorRightB(92);
//			if (us_dist_obst > 50)
//			{
//				motorForwardB(us_dist_obst+80,1);
//			}
//			else
//			{
//				motorForwardB(us_dist_obst+57,1);
//			}
//			motorRightB(89);
//			motorForwardB(10,1);
//			motorLeftB(89);
//			motorForwardA(150,0);
//			osDelay(100);
//			motorBackA(25);
//			motorForwardB(11,1);
//
//		}
//
//		if(direction == 'm'){
//
//			// 2nd obstacle right turn
//			global_angle = 0;
//			motorForwardA(150,1);
//			osDelay(100);
//			motorBackA(30);
//			motorRightB(90);
//			motorForwardA(10,0);
////			motorForwardA(40,0);
////			osDelay(100);
////			motorBackA(35);
//			//motorLeftB(209);
//			motorLeft(206);
//			motorForwardB(55,1);
//			//motorForwardA(60,0);
//			//motorBackA(40);
//			motorLeftB(88);
//			if (us_dist_obst > 50)
//			{
//				motorForwardB(us_dist_obst+80,1);
//			}
//			else
//			{
//				motorForwardB(us_dist_obst+57,1);
//			}
//			motorLeftB(87);
//			motorForwardB(10,1);
//			motorRightB(90);
//			motorForwardA(150,0);
//			osDelay(100);
//			motorBackA(25);
//			motorForwardB(11,1);
//
//		}
//
////		if(direction == 'o'){
////
////			global_angle = 0;
////			motorForwardB(20,1);
////			motorLeftB(89);
////			motorRightB(89);
////			motorForwardB(30,1);
////			motorRightB(90);
////			motorForwardA(50,1);
////
////		}
////
////		if(direction == 'p'){
////
////			global_angle = 0;
////			motorLeftC(90);
////			motorRightC(90);
////
////		}
////
////		if(direction == 'y'){
////
////			global_angle = 0;
////			motorForwardA(101,0);
////			motorLeftB(89);
////
////		}
////
////		if(direction == 't'){
////			progress = -1;
////		}
//
//			magnitude = 0;
//			flag = 0;
//			//direction = 'x';
//			uint8_t ch = '$';
//			HAL_UART_Transmit(&huart3, (uint8_t *) &ch, 1, 0xFFFF);
//		}
//	osDelayUntil(200);
	}

}


/* USER CODE BEGIN Header_motorTask */
/**
* @brief Function implementing the MotorTask thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_motorTask */
void motorTask(void *argument)
{
  /* USER CODE BEGIN motorTask */



	int dirL;
	double prevErrorL = 0;
	double LInt = 0;

	int dirR;
	double prevErrorR = 0;
	double RInt = 0;

	int pwmA = 0;
	int pwmB = 0;

	uint8_t hello[20];



			int cnt2, diff;
			int cnt2b, diffb;
			cnt2 = 0;
			cnt2b = 0;


			uint32_t tick = 0;







  /* Infinite loop */
  for(;;)
  {

	  if(HAL_GetTick() - tick >= 10){
		  cnt2 = __HAL_TIM_GET_COUNTER(&htim2);
		  cnt2b =  __HAL_TIM_GET_COUNTER(&htim3);
	  		  	  		 if(cnt2 > 32000){
	  		  	  			 dirL = 1;

	  		  	  			  diff = (65536 - cnt2);


	  		  	  			  } else {
	  		  	  				  dirL = -1;




	  		  	  			  	 diff = cnt2;

	  		  	  		  }


	  		  			  if(cnt2b > 32000){
	  		  				  dirR = -1;


	  		  			  	  	diffb = (65536 - cnt2b);
	  		  			  } else {
	  		  				  dirR = 1;



	  		  			  	  	diffb = cnt2b;
	  		  			  }

                          if(dirR == dir){
	  		  			  total_distanceb += diffb;
                          } else {
                        	  total_distanceb -= diffb;
                          }

                          if(dirL == dir){
	  		  			  total_distancea += diff;
                          } else {
                        	  total_distancea -= diff;
                          }

	  		  			  //speedA = 60000*dirL*(diff/1320.0)/(double)(HAL_GetTick() - tick);
	  		  			  //speedB = 60000*dirR*(diffb/1320.0)/(double)(HAL_GetTick() - tick);
	  		  			  pwmA = motorOnA;
	  		  			  pwmB = motorOnB;

	  		  			  if(pwmA < 0) {
	  		  				  motorAReverse();
	  		  				  pwmA = -1*pwmA;
	  		  			  } else {
	  		  				  motorAForward();
	  		  			  }

	  		  			  if(pwmB < 0){
	  		  				  motorBReverse();
	  		  				  pwmB = -1*pwmB;
	  		  			  } else {
	  		  				  motorBForward();
	  		  			  }



	  		  			  if(pwmA >= 7000){
	  		  				  pwmA = 7000;
	  		  			  }

	  		  			  if(pwmB >= 7000){
	  		  				  pwmB = 7000;
	  		  			  }



	  		  			__HAL_TIM_SetCompare(&htim8, TIM_CHANNEL_1, pwmA);
	  		  			__HAL_TIM_SetCompare(&htim8, TIM_CHANNEL_2, pwmB);

	  		  			if(motorOnA == 0 && motorOnB == 0){
	  		  				//total_distancea = 0;
	  		  				//total_distanceb = 0;
	  		  				//total_angle = 0;

	  		  			}
	  			__HAL_TIM_SET_COUNTER(&htim2, 0);
	  			__HAL_TIM_SET_COUNTER(&htim3, 0);
	  			tick = HAL_GetTick();

	  }


	  osDelayUntil(10);
  }
  osDelay(1);
  /* USER CODE END motorTask */
}

/* USER CODE BEGIN Header_gyroTask */
/**
* @brief Function implementing the GyroTask thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_gyroTask */
void gyroTask(void *argument)
{
  /* USER CODE BEGIN gyroTask */
  /* Infinite loop */
	uint8_t val[2] = {0,0};

	char hello[20];
	int16_t angular_speed = 0;



	uint32_t tick = 0;
	gyroInit();
	int dir;
	int16_t offset = 0;







	tick = HAL_GetTick();
	osDelayUntil(10);

//	for(int i = 0; i < 3000; i++){
//		readByte(0x37, val);
//		angular_speed = val[0] << 8 | val[1];
//		offset += angular_speed;
//		//osDelayUntil(1);
//	}
//	offset = offset/3000.0f;



  for(;;)
  {


      osDelayUntil(10);

      if(HAL_GetTick() - tick >= 100){
	  readByte(0x37, val);
	  //osDelayUntil(1);
	  //angular_speed = ((int16_t)((int8_t)val[0]))*256 +  (uint16_t)val[1];
	  angular_speed = (val[0] << 8) | val[1];





	  // Default Value = 0.5
	  total_angle +=(double)(angular_speed + 1.5)*((HAL_GetTick() - tick)/16400.0)*1.16;

	  global_angle += (double)(angular_speed + 1.5)*((HAL_GetTick() - tick)/16400.0)*1.16;


	  //prevSpeed = angular_speed;
	  if(total_angle >= 720){
		  total_angle = 0;
	  }
	  if(total_angle <= -720){
		  total_angle = 0;
	  }


	  tick = HAL_GetTick();
      }







  }
  /* USER CODE END gyroTask */
}

/* USER CODE BEGIN Header_ultrasoundTask */
/**
* @brief Function implementing the UltrasoundTask thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_ultrasoundTask */
void ultrasoundTask(void *argument)
{
  /* USER CODE BEGIN ultrasoundTask */
  /* Infinite loop */

double distanceC;
	HAL_ADC_Start_DMA(&hadc1, lengthFront, 4096);
	//`HAL_ADC_Start_DMA(&hadc2, lengthSide, 4096);
	uint16_t lastFront = 0;
	uint16_t lastSide = 0;
	int nextDirection = -270;
	int gyroVal=0;
	int pwmVal=0;
	int pidVal =0;
	double Aint = 0;
	int temp_dist = 0;

	char v = '0';


	for(;;){


//		uint16_t tempA = lengthFront[0];
//		uint16_t tempB = lengthSide[0];
//		distanceFront = tempA*0.5 + 0.5*lastFront;
//		distanceSide = tempB*0.5 + lastSide*0.5;
//		lastFront = tempA;
//		lastSide = tempB;


		switch(receiveBuffer[0]){
	    //switch(v){
	    case '0':

	    	gyroInit();
		    total_angle = 0;
	    	while(us_dist>55 || us_dist<30){
	    		motorOnA=1150;
	    		motorOnB=1150;


	    		pidVal = (int)(147 + (total_angle*10 + 0.00*Aint));


	    		if(inOrOut=='7'){

	    				if(pidVal <= 140){
	    					pidVal = 144;
	    				}
	    				if(pidVal >= 153){
	    					pidVal = 149;
	    				}

	    		}


	    		htim1.Instance -> CCR4 = 147;
	    		osDelayUntil(5);
	    		htim1.Instance -> CCR4 = pidVal;
	    		osDelayUntil(5);
	    		Aint += total_angle;


	    	}
			motorStop();
			total_distancea=0;
			total_distanceb=0;


			osDelay(200);
			HAL_UART_Transmit_IT(&huart3,(uint8_t *)"0000\n",4);		//Sending ack to UART


	    	if(us_dist>37){
	    		temp_dist=us_dist-37;
	    		motorForward(temp_dist,1);
	    		osDelay(100);
	    	}
	    	else{
	    		temp_dist=37-us_dist;
	    		motorBack(temp_dist);
	    		osDelay(100);
	    	}




	    	//v='1';
	    	break;



	    case '1'://left

	    	motorLeft(45);
	    	osDelay(200);
	    	motorForward(18.5,1);
	    	osDelay(200);
	    	motorRight(109.5);
	    	osDelay(200);
	    	//motorForward(10,1);
	    	//osDelay(100);
	    	motorLeft(65.25);


	    	//spacer
	    	osDelay(100);
	    	motorBack(24);

	    	gyroInit();
		    total_angle = 0;
	    	while(us_dist>40 || us_dist<10){
	    		motorOnA=1000;
	    		motorOnB=1000;

	    		pidVal = (int)(147 + (total_angle*10 + 0.00*Aint));



	    		if(inOrOut=='7'){

	    				if(pidVal <= 143.5){
	    					pidVal = 140;
	    				}
	    				if(pidVal >= 154){
	    					pidVal = 148.5;
	    				}

	    		}

	    		htim1.Instance -> CCR4 = 147;
	    		osDelayUntil(5);
	    		htim1.Instance -> CCR4 = pidVal;
	    		osDelayUntil(5);
	    		Aint += total_angle;


	    	}
			motorStop();
			total_distancea=0;
			total_distanceb=0;

			osDelay(1000);


			total_angle=0;
	    	if(us_dist>=22){
	    		temp_dist=us_dist-22;
	    		motorForward(temp_dist,1);
	    		osDelay(100);
	    	}
	    	else{
	    		temp_dist=22-us_dist;
	    		motorBack(temp_dist);
	    		osDelay(100);
	    	}

	    	motorBack(6);
	    	HAL_UART_Transmit_IT(&huart3,(uint8_t *)"1111\n",4);		//Sending ack to UART

	    	//v ='3';
	    	break;

	    case '2'://right
	    	motorRight(45);
	    	osDelay(200);
	    	motorForward(19.5,1);
	    	osDelay(100);
	    	motorLeft(117);
	    	osDelay(100);
	    	//motorForward(10,1);
	    	//osDelay(100);
	    	motorRight(63);


	    	//spacer
	    	osDelay(100);
	    	motorBack(24);

		    total_angle = 0;
	    	while(us_dist>40 || us_dist<10){
	    		motorOnA=1000;
	    		motorOnB=1000;

	    		pidVal = (int)(147 + (total_angle*10 + 0.00*Aint));



	    		if(inOrOut=='7'){

	    				if(pidVal <= 143.5){
	    					pidVal = 140;
	    				}
	    				if(pidVal >= 154){
	    					pidVal = 148.5;
	    				}

	    		}

	    		htim1.Instance -> CCR4 = 147;
	    		osDelayUntil(5);
	    		htim1.Instance -> CCR4 = pidVal;
	    		osDelayUntil(5);
	    		Aint += total_angle;


	    	}
			motorStop();
			total_distancea=0;
			total_distanceb=0;

			osDelay(1000);



	    	if(us_dist>=22){
	    		temp_dist=us_dist-22;
	    		motorForward(temp_dist,1);
	    		osDelay(100);
	    	}
	    	else{
	    		temp_dist=22-us_dist;
	    		motorBack(temp_dist);
	    		osDelay(100);
	    	}

	    	motorBack(6);
	    	HAL_UART_Transmit_IT(&huart3,(uint8_t *)"1111\n",4);		//Sending ack to UART



	    	//v='4';
	    	break;

	    case '3'://smallest dist

	    	osDelay(200);
	    	motorLeft(95);
	    	osDelay(100);
//	    	motorForward(4,1);
//	    	osDelay(100);
	    	motorRight(195);
	    	osDelay(100);
	    	motorForward(52,1);
	    	osDelay(100);
	    	motorRight(122.5);
	    	osDelay(100);

	    	gyroInit();
	    	total_angle=0;
	    	motorForward(124,1);


	    	osDelay(100);
	    	motorRight(17);
	    	osDelay(100);
//	    	motorForward(5,1);
//	    	osDelay(100);
	    	motorLeft(28);


	    	total_angle = 0;
	    		    	while(us_dist>30 || us_dist<12){
	    		    		motorOnA=1100;
	    		    		motorOnB=1100;

	    		    		pidVal = (int)(147 + (total_angle*10 + 0.00*Aint));



	    		    		if(inOrOut=='7'){

	    		    				if(pidVal <= 142){
	    		    					pidVal = 144;
	    		    				}
	    		    				if(pidVal >= 153){
	    		    					pidVal = 149;
	    		    				}

	    		    		}

	    		    		htim1.Instance -> CCR4 = 147;
	    		    		osDelayUntil(5);
	    		    		htim1.Instance -> CCR4 = pidVal;
	    		    		osDelayUntil(5);
	    		    		Aint += total_angle;


	    		    	}
	    				motorStop();
	    				total_distancea=0;
	    				total_distanceb=0;

	    				osDelay(100);



	    		    	if(us_dist>=15){
	    		    		temp_dist=us_dist-15;
	    		    		motorForward(temp_dist,1);
	    		    		osDelay(100);
	    		    	}
	    		    	else{
	    		    		temp_dist=15-us_dist;
	    		    		motorBack(temp_dist);
	    		    		osDelay(100);
	    		    	}


	    	//v='5';
	    	break;
//
	    case '4'://smallest dist

	    	osDelay(200);
	    	motorRight(95);
	    	osDelay(100);
//	    	motorForward(3,1);
//	    	osDelay(100);
	    	motorLeft(207);
	    	osDelay(100);
	    	motorForward(63,1);
	    	osDelay(100);
	    	motorLeft(125);
	    	osDelay(100);
	    	gyroInit();
	    	total_angle=0;
	    	motorForward(130,1);


	    	osDelay(100);
	    	motorLeft(15);
	    	osDelay(100);
//	    	motorForward(7,1);
//	    	osDelay(100);
	    	motorRight(21);

	    	total_angle = 0;
	    		    		    	while(us_dist>30 || us_dist<12){
	    		    		    		motorOnA=1100;
	    		    		    		motorOnB=1100;

	    		    		    		pidVal = (int)(146.5 + (total_angle*10 + 0.00*Aint));



	    		    		    		if(inOrOut=='7'){

	    		    		    				if(pidVal <= 140){
	    		    		    					pidVal = 145;
	    		    		    				}
	    		    		    				if(pidVal >= 154){
	    		    		    					pidVal = 149;
	    		    		    				}

	    		    		    		}

	    		    		    		htim1.Instance -> CCR4 = 147;
	    		    		    		osDelayUntil(5);
	    		    		    		htim1.Instance -> CCR4 = pidVal;
	    		    		    		osDelayUntil(5);
	    		    		    		Aint += total_angle;


	    		    		    	}
	    		    				motorStop();
	    		    				total_distancea=0;
	    		    				total_distanceb=0;

	    		    				osDelay(100);



	    		    		    	if(us_dist>=15){
	    		    		    		temp_dist=us_dist-15;
	    		    		    		motorForward(temp_dist,1);
	    		    		    		osDelay(100);
	    		    		    	}
	    		    		    	else{
	    		    		    		temp_dist=15-us_dist;
	    		    		    		motorBack(temp_dist);
	    		    		    		osDelay(100);
	    		    		    	}
	    	//v='5';
	    	break;

	    case '5'://largest dist

	    	osDelay(200);
	    	motorLeft(95);
	    	osDelay(100);
//	    	motorForward(4,1);
//	    	osDelay(100);
	    	motorRight(195);
	    	osDelay(100);
	    	motorForward(51.5,1);
	    	osDelay(100);
	    	motorRight(111);
	    	osDelay(100);

	    	gyroInit();
	    	total_angle=0;
	    	motorForward(150,1);


	    	osDelay(100);
	    	motorRight(12);
	    	osDelay(100);
//	    	motorForward(5,1);
//	    	osDelay(100);
	    	motorLeft(19);


	    	total_angle = 0;
	    		    	while(us_dist>30 || us_dist<12){
	    		    		motorOnA=1100;
	    		    		motorOnB=1100;

	    		    		pidVal = (int)(147 + (total_angle*10 + 0.00*Aint));



	    		    		if(inOrOut=='7'){

	    		    				if(pidVal <= 142){
	    		    					pidVal = 144;
	    		    				}
	    		    				if(pidVal >= 153){
	    		    					pidVal = 149;
	    		    				}

	    		    		}

	    		    		htim1.Instance -> CCR4 = 147;
	    		    		osDelayUntil(5);
	    		    		htim1.Instance -> CCR4 = pidVal;
	    		    		osDelayUntil(5);
	    		    		Aint += total_angle;


	    		    	}
	    				motorStop();
	    				total_distancea=0;
	    				total_distanceb=0;

	    				osDelay(100);



	    		    	if(us_dist>=15){
	    		    		temp_dist=us_dist-15;
	    		    		motorForward(temp_dist,1);
	    		    		osDelay(100);
	    		    	}
	    		    	else{
	    		    		temp_dist=15-us_dist;
	    		    		motorBack(temp_dist);
	    		    		osDelay(100);
	    		    	}


	    	//v='5';
	    	break;
//
	    case '6'://largest dist

	    	osDelay(200);
	    	motorRight(95);
	    	osDelay(100);
//	    	motorForward(3,1);
//	    	osDelay(100);
	    	motorLeft(207);
	    	osDelay(100);
	    	motorForward(63,1);
	    	osDelay(100);
	    	motorLeft(116);
	    	osDelay(100);
	    	gyroInit();
	    	total_angle=0;
	    	motorForward(153,1);


	    	osDelay(100);
	    	motorLeft(18);
	    	osDelay(100);
//	    	motorForward(7,1);
//	    	osDelay(100);
	    	motorRight(20);

	    	total_angle = 0;
	    		    		    	while(us_dist>30 || us_dist<12){
	    		    		    		motorOnA=1100;
	    		    		    		motorOnB=1100;

	    		    		    		pidVal = (int)(146.5 + (total_angle*10 + 0.00*Aint));



	    		    		    		if(inOrOut=='7'){

	    		    		    				if(pidVal <= 140){
	    		    		    					pidVal = 145;
	    		    		    				}
	    		    		    				if(pidVal >= 154){
	    		    		    					pidVal = 149;
	    		    		    				}

	    		    		    		}

	    		    		    		if(inOrOut=='8'){

	    		    		    				if(pidVal <= 140){
	    		    		    					pidVal = 144;
	    		    		    				}
	    		    		    				if(pidVal >= 154){
	    		    		    					pidVal = 146.5;
	    		    		    				}

	    		    		    		}
	    		    		    		htim1.Instance -> CCR4 = 147;
	    		    		    		osDelayUntil(5);
	    		    		    		htim1.Instance -> CCR4 = pidVal;
	    		    		    		osDelayUntil(5);
	    		    		    		Aint += total_angle;


	    		    		    	}
	    		    				motorStop();
	    		    				total_distancea=0;
	    		    				total_distanceb=0;

	    		    				osDelay(100);



	    		    		    	if(us_dist>=15){
	    		    		    		temp_dist=us_dist-15;
	    		    		    		motorForward(temp_dist,1);
	    		    		    		osDelay(100);
	    		    		    	}
	    		    		    	else{
	    		    		    		temp_dist=15-us_dist;
	    		    		    		motorBack(temp_dist);
	    		    		    		osDelay(100);
	    		    		    	}
	    	//v='5';
	    	break;

	    case '7'://medium dist

	    	osDelay(200);
	    	motorLeft(95);
	    	osDelay(100);
//	    	motorForward(4,1);
//	    	osDelay(100);
	    	motorRight(195);
	    	osDelay(100);
	    	motorForward(51.5,1);
	    	osDelay(100);
	    	motorRight(112);
	    	osDelay(100);

	    	gyroInit();
	    	total_angle=0;
	    	motorForward(135,1);


	    	osDelay(100);
	    	motorRight(10);
	    	osDelay(100);
//	    	motorForward(5,1);
//	    	osDelay(100);
	    	motorLeft(18.5);


	    	total_angle = 0;
	    		    	while(us_dist>30 || us_dist<12){
	    		    		motorOnA=1100;
	    		    		motorOnB=1100;

	    		    		pidVal = (int)(147 + (total_angle*10 + 0.00*Aint));



	    		    		if(inOrOut=='7'){

	    		    				if(pidVal <= 142){
	    		    					pidVal = 144;
	    		    				}
	    		    				if(pidVal >= 153){
	    		    					pidVal = 149;
	    		    				}

	    		    		}

	    		    		htim1.Instance -> CCR4 = 147;
	    		    		osDelayUntil(5);
	    		    		htim1.Instance -> CCR4 = pidVal;
	    		    		osDelayUntil(5);
	    		    		Aint += total_angle;


	    		    	}
	    				motorStop();
	    				total_distancea=0;
	    				total_distanceb=0;

	    				osDelay(100);



	    		    	if(us_dist>=15){
	    		    		temp_dist=us_dist-15;
	    		    		motorForward(temp_dist,1);
	    		    		osDelay(100);
	    		    	}
	    		    	else{
	    		    		temp_dist=15-us_dist;
	    		    		motorBack(temp_dist);
	    		    		osDelay(100);
	    		    	}


	    	//v='5';
	    	break;
//
	    case '8'://medium dist

	    	osDelay(200);
	    	motorRight(95);
	    	osDelay(100);
//	    	motorForward(3,1);
//	    	osDelay(100);
	    	motorLeft(207);
	    	osDelay(100);
	    	motorForward(67,1);
	    	osDelay(100);
	    	motorLeft(124);
	    	osDelay(100);
	    	gyroInit();
	    	total_angle=0;
	    	motorForward(140,1);


	    	osDelay(100);
	    	motorLeft(13);
	    	osDelay(100);
//	    	motorForward(7,1);
//	    	osDelay(100);
	    	motorRight(16);

	    	total_angle = 0;
	    		    		    	while(us_dist>30 || us_dist<12){
	    		    		    		motorOnA=1100;
	    		    		    		motorOnB=1100;

	    		    		    		pidVal = (int)(147 + (total_angle*10 + 0.00*Aint));



	    		    		    		if(inOrOut=='7'){

	    		    		    				if(pidVal <= 140){
	    		    		    					pidVal = 145;
	    		    		    				}
	    		    		    				if(pidVal >= 154){
	    		    		    					pidVal = 149;
	    		    		    				}

	    		    		    		}


	    		    		    		htim1.Instance -> CCR4 = 147;
	    		    		    		osDelayUntil(5);
	    		    		    		htim1.Instance -> CCR4 = pidVal;
	    		    		    		osDelayUntil(5);
	    		    		    		Aint += total_angle;


	    		    		    	}
	    		    				motorStop();
	    		    				total_distancea=0;
	    		    				total_distanceb=0;

	    		    				osDelay(100);



	    		    		    	if(us_dist>=15){
	    		    		    		temp_dist=us_dist-15;
	    		    		    		motorForward(temp_dist,1);
	    		    		    		osDelay(100);
	    		    		    	}
	    		    		    	else{
	    		    		    		temp_dist=15-us_dist;
	    		    		    		motorBack(temp_dist);
	    		    		    		osDelay(100);
	    		    		    	}
	    	//v='5';
	    	break;

	    case '9':
	    	motorStop();


	    }
		osDelayUntil(5);
	}
  /* USER CODE END ultrasoundTask */
}

/**
  * @brief  Period elapsed callback in non blocking mode
  * @note   This function is called  when TIM6 interrupt took place, inside
  * HAL_TIM_IRQHandler(). It makes a direct call to HAL_IncTick() to increment
  * a global variable "uwTick" used as application time base.
  * @param  htim : TIM handle
  * @retval None
  */
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
  /* USER CODE BEGIN Callback 0 */

  /* USER CODE END Callback 0 */
  if (htim->Instance == TIM6) {
    HAL_IncTick();
  }
  /* USER CODE BEGIN Callback 1 */

  /* USER CODE END Callback 1 */
}

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

