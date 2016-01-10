#include "BspKey.h"
#include "stm32f1xx_hal.h"
#include <string.h>
#include "SEGGER_RTT.h"

/* Private variables ---------------------------------------------------------*/
KEY_STATUS KeyStatus[KEY_NUM];

/* Private variables END */

void Keychange(void);
void Keycheck(void);

uint32_t g_KeyChangedVal[KEY_NUM];

void KeyInit(void)
{
	KEY_ID i;
	
	memset(KeyStatus,0,sizeof(KeyStatus));

    KeyStatus[K3].KeyPinStruct.GPIOX 	= K3_GPIO_Port;
	KeyStatus[K3].KeyPinStruct.PIN 	    = K3_Pin;
    KeyStatus[K4].KeyPinStruct.GPIOX 	= K4_GPIO_Port;
	KeyStatus[K4].KeyPinStruct.PIN 	    = K4_Pin;
#ifdef USE_SHORT_LONG_DOWN_CHECK
	for(i = K3; i < KEY_NUM; i++)
		KeyStatus[i].CheckLongDownEn = TRUE;
#endif
}

/*
	软件定时器回调函数调用此函数
*/
void KeyPro(void)
{
	Keycheck();
	Keychange();
}

void Keycheck(void)
{
	KEY_ID KeyIDScan;
	for(KeyIDScan = K3; KeyIDScan < KEY_NUM; KeyIDScan++)
	{
		if(HAL_GPIO_ReadPin(KeyStatus[KeyIDScan].KeyPinStruct.GPIOX,KeyStatus[KeyIDScan].KeyPinStruct.PIN))//KEY UP
		{
		#ifdef USE_SHORT_LONG_DOWN_CHECK
			if(KeyStatus[KeyIDScan].KeyDown.Flag)
			{
				if((KeyStatus[KeyIDScan].KeyDown.Count > 0)&&(KeyStatus[KeyIDScan].KeyDown.Count < KEY_LONG_TICK))
				{
					KeyStatus[KeyIDScan].KeyShortDown = TRUE;
				}
				else if(KeyStatus[KeyIDScan].KeyDown.Count >= KEY_LONG_TICK)
				{
					if(KeyStatus[KeyIDScan].CheckLongDownEn == TRUE)
					{
						KeyStatus[KeyIDScan].KeyLongDown = TRUE;
					}
				}
			}
			
			KeyStatus[KeyIDScan].CheckLongDownEn = TRUE;
		#endif
			KeyStatus[KeyIDScan].KeyDown.Count = 0;
			if(KeyStatus[KeyIDScan].KeyUp.Count < 0xFFF0)
			{
				KeyStatus[KeyIDScan].KeyUp.Count++;
			}
			
			if(KeyStatus[KeyIDScan].KeyUp.Count > KEY_FILTER_TICK)
			{
				KeyStatus[KeyIDScan].KeyUp.Flag = TRUE;
				KeyStatus[KeyIDScan].KeyDown.Flag = FALSE;
			}
		}
		else//KEY DOWN
		{
			KeyStatus[KeyIDScan].KeyUp.Count = 0;
			if(KeyStatus[KeyIDScan].KeyDown.Count < 0xFFF0)
			{
				KeyStatus[KeyIDScan].KeyDown.Count++;
			}
			
			if(KeyStatus[KeyIDScan].KeyDown.Count > KEY_FILTER_TICK)
			{
				KeyStatus[KeyIDScan].KeyDown.Flag = TRUE;
				KeyStatus[KeyIDScan].KeyUp.Flag = FALSE;
			}
		#ifdef USE_SHORT_LONG_DOWN_CHECK
			if(KeyStatus[KeyIDScan].CheckLongDownEn == TRUE)
			{
				if(KeyStatus[KeyIDScan].KeyDown.Count > KEY_LONG_TICK)
				{
					KeyStatus[KeyIDScan].KeyLongDown = TRUE;
					KeyStatus[KeyIDScan].CheckLongDownEn = FALSE;
				}
			}
		#endif
		}
	}
}

void Keychange(void)
{
	u8 i;
	memset(g_KeyChangedVal, 0, KEY_NUM);
	
	for(i = K3; i < KEY_NUM; i++)
	{

        if(KeyStatus[i].KeyUp.Flag == FALSE)
        {
            g_KeyChangedVal[i] = VAL_KEY_UP;
        }
        else
        {
            g_KeyChangedVal[i] = VAL_KEY_DOWN;
        }
    }
    SEGGER_RTT_printf(0, "K3 = %d K4 = %d \n", g_KeyChangedVal[0], g_KeyChangedVal[1]);    
}
