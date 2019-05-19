/******************************************************************************
**************************Hardware interface layer*****************************
* | file      	:	DEV_Config.cpp
* |	version		:	V1.0
* | date		:	2017-12-14
* | function	:
	Provide the hardware underlying interface
******************************************************************************/
#include "DEV_Config.h"

/********************************************************************************
function:	System Init and exit
note:
	Initialize the communication method
********************************************************************************/
uint8_t System_Init(void)
{
    Serial.begin(115200);
    Serial.println("GPIO Init...");

    return 0;
}

void System_Exit(void)
{
    Serial.println("Demo over...");
}

void DEV1_Init(void)
{
	//set pin output
    pinMode(MOTOR1_A,   OUTPUT);
    pinMode(MOTOR1_B,   OUTPUT);
    pinMode(MOTOR2_A,   OUTPUT);
    pinMode(MOTOR2_B,   OUTPUT);
    pinMode(MOTOR1_EN,  OUTPUT);
    pinMode(MOTOR2_EN,  OUTPUT);
	
	//enable L293D
    digitalWrite(MOTOR1_EN, 1);
    digitalWrite(MOTOR2_EN, 1);
	
	//motor1
    digitalWrite(MOTOR1_A, 1);
    digitalWrite(MOTOR1_B, 1);
    digitalWrite(MOTOR2_A, 1);
    digitalWrite(MOTOR2_B, 1);
}

void DEV2_Init(void)
{
	//set pin output
    pinMode(MOTOR3_A,   OUTPUT);
    pinMode(MOTOR3_B,   OUTPUT);
    pinMode(MOTOR4_A,   OUTPUT);
    pinMode(MOTOR4_B,   OUTPUT);
    pinMode(MOTOR3_EN,  OUTPUT);
    pinMode(MOTOR4_EN,  OUTPUT);

    //enable L293D
    digitalWrite(MOTOR3_EN, 1);
    digitalWrite(MOTOR4_EN, 1);
	
	//motor2
    digitalWrite(MOTOR3_A, 1);
    digitalWrite(MOTOR3_B, 1);
    digitalWrite(MOTOR4_A, 1);
    digitalWrite(MOTOR4_B, 1);
}

/********************************************************************************
function:	Delay function
note:
	Driver_Delay_ms(xms) : Delay x ms
	Driver_Delay_us(xus) : Delay x us
********************************************************************************/
void Driver_Delay_ms(unsigned long xms)
{
    delay(xms);
}

void Driver_Delay_us(int xus)
{
    for(int j=xus; j > 0; j--);
}
