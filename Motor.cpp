/******************************************************************************
***************************Intermediate driver layer***************************
* | file      	:	Motor.cpp
* |	version		:	V1.0
* | date		:	2017-12-14
* | function	:	28BYJ-48 Stepper motor Drive function
******************************************************************************/
#include "Motor.h"
struct MOTOR sMotor1, sMotor2;

BYTE BeatCode[8] = { //Stepper motor eight eight beat code
    0x0E, 0x0C, 0x0D, 0x09, 0x0B, 0x03, 0x07, 0x06
};

static void Motor_Setbit(struct MOTOR sMotor,BYTE Data)
{
	BYTE Bit[4] = {sMotor.L1, sMotor.L2, sMotor.L3, sMotor.L4};
	Data = (~Data) & 0x0f;
	
	BYTE Temp;
	for(Temp = 0;Temp < 4; Temp++){
		if(Data & 0x01 == 1){
			DEBUG(" 0 ");
			SET_PIN(Bit[Temp], 0);
		}else{
			DEBUG(" 1 ");
			SET_PIN(Bit[Temp], 1);
		}
		Data = Data >> 1;
	}
   Driver_Delay_ms(2);
	DEBUG("\r\n");
}

void Motor_Trun(BYTE Motor_dev, unsigned long Angle)
{	
	struct MOTOR sMotor;
	if(Motor_dev == MOTOR_DEV_1){
		sMotor = sMotor1;
	}else if(Motor_dev == MOTOR_DEV_2){
		sMotor = sMotor2;
	}else{
		DEBUG("not motor device \r\n");
	}
	
    BYTE Index = 0;
     long beats = (Angle * 4076) / 360 ; //Need to turn the beat

	for(beats = beats; beats > 0; beats--){
		Motor_Setbit(sMotor, BeatCode[Index]);
        Index++;
		if(Index % 8 == 0){
			Index = Index & 0x07; //Greater than 8 clear 0		
			DEBUG("****************\r\n");
		}			
	}
	Motor_Setbit(sMotor, 0x0f);
}
// :: :: :: :: ::
void Motor_Trun0(BYTE Motor_dev, unsigned long Angle)
{  
  struct MOTOR sMotor;
  if(Motor_dev == MOTOR_DEV_1){
    sMotor = sMotor1;
  }else if(Motor_dev == MOTOR_DEV_2){
    sMotor = sMotor2;
  }else{
    DEBUG("not motor device \r\n");
  }
  
    BYTE Index = 0;
     long beats = (Angle * 4076) / 360 ; //Need to turn the beat

  for(beats = beats; beats > 0; beats--){
    Motor_Setbit(sMotor, BeatCode[Index]);
        Index++;
    if(Index % 8 == 0){
      Index = Index & 0x07; //Greater than 8 clear 0    
      DEBUG("****************\r\n");
    }     
  }
  Motor_Setbit(sMotor, 0x0f);
}
// :: :: :: :: ::

void Motor_Init(BYTE Motor1_dev, BYTE Motor2_dev)
{
	if(Motor1_dev == MOTOR_DEV_1){
		DEV1_Init();
		
		sMotor1.L1 = MOTOR1_A;
		sMotor1.L2 = MOTOR1_B;
		sMotor1.L3 = MOTOR2_A;
		sMotor1.L4 = MOTOR2_B;
	}
	
	if(Motor2_dev == MOTOR_DEV_2){
		DEV2_Init();
		
		sMotor2.L1 = MOTOR3_A;
		sMotor2.L2 = MOTOR3_B;
		sMotor2.L3 = MOTOR4_A;
		sMotor2.L4 = MOTOR4_B;
	}	
}


void Motor_Init0(BYTE Motor1_dev, BYTE Motor2_dev)
{
  if(Motor1_dev == MOTOR_DEV_1){
    DEV1_Init();
    
    sMotor1.L1 = MOTOR1_B;
    sMotor1.L2 = MOTOR1_A;
    sMotor1.L3 = MOTOR2_B;
    sMotor1.L4 = MOTOR2_A;
  }
  
  if(Motor2_dev == MOTOR_DEV_2){
    DEV2_Init();
    
    sMotor2.L1 = MOTOR3_B;
    sMotor2.L2 = MOTOR3_A;
    sMotor2.L3 = MOTOR4_B;
    sMotor2.L4 = MOTOR4_A;
  } 
}
