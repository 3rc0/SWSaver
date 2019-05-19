/******************************************************************************
**************************Hardware interface layer*****************************
* | file      	:	DEV_Config.h
* |	version		:	V1.0
* | date		:	2017-12-14
* | function	:
	Provide the hardware underlying interface
******************************************************************************/
#ifndef _DEV_CONFIG_H_
#define _DEV_CONFIG_H_

#include <Arduino.h>

#define	BYTE				uint8_t		
#define	WORD				uint16_t

#define DEV_DEBUG 0
#if DEV_DEBUG
	#define DEBUG(__info) Serial.print(__info)
#else
	#define DEBUG(__info)  
#endif
/****************************************************************************************
		//Use the  library function definition
*****************************************************************************************/
//GPIO config
#define MOTOR1_A    2
#define MOTOR1_B    3
#define MOTOR1_EN   6

#define MOTOR2_A    4
#define MOTOR2_B    5
#define MOTOR2_EN   9

#define MOTOR3_A    7
#define MOTOR3_B    8
#define MOTOR3_EN   10

#define MOTOR4_A    12
#define MOTOR4_B    13
#define MOTOR4_EN   11

#define SET_PIN(__PIN, __VOL) digitalWrite(__PIN, __VOL)

/*------------------------------------------------------------------------------------------------------*/
uint8_t System_Init(void);
void System_Exit(void);
void DEV1_Init(void);
void DEV2_Init(void);
void Driver_Delay_ms(unsigned long xms);
void Driver_Delay_us(int xus);

#endif
