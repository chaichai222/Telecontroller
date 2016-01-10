#ifndef BSP_MOTOR_H
#define BSP_MOTOR_H

#include "TypeDef.h"
#include "stm32f1xx_hal.h"
 	

/*
	motor enum
*/
typedef enum MOTOR_ID_ENUM
{	
	MOTOR_WARNING			= 0,//
	
	MOTOR_NUM,		
}MOTOR_ID;

/*
	motor struct
*/

typedef struct MOTOR_STATUS_STRU
{
	PIN_STRUCT MotorPinStruct;
	//BOOL	   LedOnOff;
}MOTOR_STATUS;



/*
	led defined: 
*/

#define MOTOR_WARNING_PIN				(GPIO_PIN_13)
#define MOTOR_WARNING_GPIOX				(GPIOD)


void MotorInit(void);
void MotorStart(u32 ContinueMillisec);

void MotorStop(void);
void MotorPro(void);


#endif

