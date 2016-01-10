#ifndef BSP_BEEP_H
#define BSP_BEEP_H

#include "TypeDef.h"
#include "stm32f1xx_hal.h"
 	

/*
	beep enum
*/
typedef enum BEEP_ID_ENUM
{	
	BEEP_WARNING			= 0,//
	
	BEEP_NUM,		
}BEEP_ID;

/*
	beep struct
*/

typedef struct BEEP_STATUS_STRU
{
	PIN_STRUCT BeepPinStruct;
	//BOOL	   LedOnOff;
}BEEP_STATUS;



/*
	led defined: 
*/

#define BEEP_WARNING_PIN				(GPIO_PIN_10)
#define BEEP_WARNING_GPIOX				(GPIOE)


void BeepInit(void);
void BeepStart(u32 ContinueMillisec);

void BeepStop(void);
void BeepPro(void);


#endif

