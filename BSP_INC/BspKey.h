#ifndef BSP_KEY_H
#define BSP_KEY_H

#include "TypeDef.h"
#include "stm32f1xx_hal.h"


#define USE_SHORT_LONG_DOWN_CHECK 			//是否区分检测长按短按键
#define KEY_PRO_CALL_BACK_TIME 		10		//10ms调用一次
#define KEY_FILTER_TIME		   		40		//40ms
#define KEY_LONG_TIME 		   		2000  	//2000ms

#define  VAL_KEY_UP    1900
#define  VAL_KEY_DOWN  1100
#define  VAL_KEY_MID   1500

#define KEY_FILTER_TICK		   KEY_FILTER_TIME/KEY_PRO_CALL_BACK_TIME		//4
#define KEY_LONG_TICK 		   KEY_LONG_TIME/KEY_PRO_CALL_BACK_TIME  	    //20


/*
	key enum
*/
typedef enum KEY_ID_ENUM
{	
	K3				= 0,	//夜航灯       1
	K4				,	//定点飞行模式 2

	KEY_NUM,		
}KEY_ID;

/*
	key struct
*/

typedef struct KEY_UP_DOWN_STRU
{
	u16 	Count;//10ms 
	BOOL	Flag;
}KEY_UP_DOWN;

typedef struct KEY_STATUS_STRU
{
	PIN_STRUCT KeyPinStruct;
	KEY_UP_DOWN KeyDown;
	KEY_UP_DOWN KeyUp;
#ifdef USE_SHORT_LONG_DOWN_CHECK
	BOOL KeyShortDown;
	BOOL KeyLongDown;
	BOOL CheckLongDownEn;//允许检测长按键开关
#endif
}KEY_STATUS;


extern void KeyInit(void);
extern void KeyPro(void);

extern uint32_t g_KeyChangedVal[KEY_NUM];


#endif

