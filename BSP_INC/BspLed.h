#ifndef BSP_LED_H
#define BSP_LED_H

#include "TypeDef.h"
#include "stm32f1xx_hal.h"
#include "BspKey.h"

/*
	led enum
*/
typedef enum LED_ID_ENUM
{	
	LED		= 0 ,   //Ö¸Ê¾µÆ

	LED_NUM,		
}LED_ID;

/*
	led struct
*/

typedef struct LED_STATUS_STRU
{
	PIN_STRUCT LedPinStruct;
	//BOOL	   LedOnOff;
}LED_STATUS;

extern void LedInit(void);
extern void LedPro(void);

#endif

