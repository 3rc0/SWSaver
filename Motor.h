/******************************************************************************
***************************Intermediate driver layer***************************
* | file      	:	Motor.h
* |	version		:	V1.0
* | date		:	2017-12-14
* | function	:	28BYJ-48 Stepper motor Drive function
******************************************************************************/
#ifndef __MOTOR_H
#define __MOTOR_H

#include "DEV_Config.h"

//define two device
#define MOTOR_DEV_1  1
#define MOTOR_DEV_2  2

struct MOTOR{
	BYTE L1;    //Orange line
	BYTE L2;    //Yellow line
	BYTE L3;    //Powder line
	BYTE L4;    //Blue line
};

/********************************************************************************
  function:
			Macro definition variable name
********************************************************************************/
void Motor_Init(BYTE Motor1, BYTE Motor2);
void Motor_Init0(BYTE Motor1, BYTE Motor2);
void Motor_Trun(BYTE Motor, unsigned long Angle);
void Motor_Trun0(BYTE Motor, unsigned long Angle);
#endif
