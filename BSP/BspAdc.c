#include "BspAdc.h"
#include "stm32f1xx_hal.h"
#include <string.h>
#include "SEGGER_RTT.h"

static ADC_STATUS s_tAdcStatus;
ADC_CHANGE g_AdcChange;
#define MIN_ADC_CHANGED_VAL 1100
#define MAX_ADC_CHANGED_VAL 1900
#define MIDD                1500
#define ADJUST_ADC_M        100
#define ADJUST_ADC_T        3650
#define ADJUST_ADC_B        100
#define ADJUST_M            (ADJUST_ADC_T+ADJUST_ADC_B)/2

uint8_t k;
void AdcCheck(void);
void AdcDataChange(void);
void AdcAdjustValueInit(void);

void AdcInit(void)
{
	memset(&s_tAdcStatus,0,sizeof(ADC_STATUS));
	AdcAdjustValueInit();
	
	HAL_ADC_Start_DMA(&hadc1, &(s_tAdcStatus.Value[0][0]), (sizeof(ADC_STATUS))/(sizeof(u32)));
}

void AdcAdjustValueInit(void)
{
	u16 i;
	memset(&g_AdcChange,0,sizeof(ADC_CHANGE));

	for(i=0; i<ADC_NUM; i++)
	{
		g_AdcChange.H_AdjustValue[i] = 0xED8;//0xFFF
		g_AdcChange.L_AdjustValue[i] = 0x64; //0
	}
}

void AdcPro(void)
{
    HAL_ADC_Start_DMA(&hadc1, *(s_tAdcStatus.Value),(sizeof(ADC_STATUS))/(sizeof(u32)));    //SOURCE_NUM*ADC_NUM);//sizeof(SOURCE_NUM*ADC_NUM))
	AdcCheck();
	AdcDataChange();
}

void AdcCheck(void)
{
	u32 AdcProTemp[SOURCE_NUM][ADC_NUM];
	u32 ValueTemp;
	u8 i,j,k;  
	for(i = 0; i < SOURCE_NUM; i++)
	{
		for(j = 0; j < ADC_NUM; j++)
		{
			AdcProTemp[i][j] = s_tAdcStatus.Value[i][j];
		}
	}
	
/*以下任意一种方法都可以计算出想要的结果，只是不同情况下效率不同*/
#if ((DELETE_MIN_NUM >= USEFUL_NUM)&&(DELETE_MIN_NUM >=DELETE_MAX_NUM))

	for(j = ADC_1; j < ADC_NUM; j++)//排序，最小的ELETE_MIN_NUM不会排序，找出最大和最小的几个，以便扔掉
	{
		for(k = 0; k < (SOURCE_NUM - DELETE_MIN_NUM); k++)//找出最大的SOURCE_NUM -DELETE_MIN_NUM 个
		{
			for(i = 0; i < (SOURCE_NUM-1-k); i++)
			{
				if(AdcProTemp[i][j] > AdcProTemp[i+1][j])
				{
					ValueTemp 			= AdcProTemp[i][j];
					AdcProTemp[i][j] 	= AdcProTemp[i+1][j];
					AdcProTemp[i+1][j] 	= ValueTemp;
				}
			}
		}
	}
	
#elif ((DELETE_MAX_NUM >= USEFUL_NUM)&&(DELETE_MAX_NUM >=DELETE_MIN_NUM))

	for(j = ADC_1; j < ADC_NUM; j++)//排序，找出最大和最小的几个，以便扔掉
	{
		for(k = 0; k < (SOURCE_NUM - DELETE_MAX_NUM); k++)//找出最小的SOURCE_NUM -DELETE_MAX_NUM 个
		{
			for(i = (SOURCE_NUM-1); i > k; i--)
			{
				if(AdcProTemp[i-1][j] > AdcProTemp[i][j])
				{
					ValueTemp 			= AdcProTemp[i][j];
					AdcProTemp[i][j] 	= AdcProTemp[i-1][j];
					AdcProTemp[i-1][j] 	= ValueTemp;
				}
			}
		}
	}
	
#else

	for(j = ADC_1; j < ADC_NUM; j++)//排序，找出最大和最小的几个，以便扔掉
	{
		for(k = 0; k < DELETE_MAX_NUM; k++)//找出最大的DELETE_MAX_NUM 个
		{
			for(i = 0; i < (SOURCE_NUM-1-k); i++)
			{
				if(AdcProTemp[i][j] > AdcProTemp[i+1][j])
				{
					ValueTemp 			= AdcProTemp[i][j];
					AdcProTemp[i][j] 	= AdcProTemp[i+1][j];
					AdcProTemp[i+1][j] 	= ValueTemp;
				}
			}
		}
	}

	for(j = ADC_1; j < ADC_NUM; j++)//排序，找出最大和最小的几个，以便扔掉
	{
		for(k = 0; k < DELETE_MIN_NUM; k++)//找出最小的DELETE_MIN_NUM 个
		{
			for(i = (SOURCE_NUM-1); i > k; i--)
			{
				if(AdcProTemp[i-1][j] > AdcProTemp[i][j])
				{
					ValueTemp 			= AdcProTemp[i][j];
					AdcProTemp[i][j] 	= AdcProTemp[i-1][j];
					AdcProTemp[i-1][j] 	= ValueTemp;
				}
			}
		}
	}

#endif

	for(j = ADC_1; j < ADC_NUM; j++)                //去掉最大和最小，去中间的值放在s_tAdcStatus.UsefulValue[j]
	{
		ValueTemp = 0;
		for(i = DELETE_MIN_NUM; i < (DELETE_MIN_NUM + USEFUL_NUM); i++)
		{
			ValueTemp += s_tAdcStatus.Value[i][j];
		}		
		s_tAdcStatus.UsefulValue[j] = (ValueTemp/USEFUL_NUM);
	}
}

void AdcDataChange(void)
{
	uint8_t i;
	memset(g_AdcChange.ChangedValue,0,ADC_NUM);  
    for(i=ADC_1; i<ADC_NUM;i++)
    {        
        if((s_tAdcStatus.UsefulValue[i] > (ADJUST_M-ADJUST_ADC_M)) && (s_tAdcStatus.UsefulValue[i] < (ADJUST_M+ADJUST_ADC_M)))
        {
           g_AdcChange.ChangedValue[i] = MIDD;
        }
        else
        {
            if(s_tAdcStatus.UsefulValue[i] < (ADJUST_M-ADJUST_ADC_M))
            {
                if(s_tAdcStatus.UsefulValue[i] > ADJUST_ADC_B)
                   g_AdcChange.ChangedValue[i] = (s_tAdcStatus.UsefulValue[i]-g_AdcChange.L_AdjustValue[i])*(MIDD-MIN_ADC_CHANGED_VAL)/(ADJUST_M-ADJUST_ADC_M-g_AdcChange.L_AdjustValue[i])+MIN_ADC_CHANGED_VAL;
                else
                   g_AdcChange.ChangedValue[i] = MIN_ADC_CHANGED_VAL;
            }
            else
            {
                if(s_tAdcStatus.UsefulValue[i] < ADJUST_ADC_T)
                   g_AdcChange.ChangedValue[i] = (MAX_ADC_CHANGED_VAL - MIDD)*(s_tAdcStatus.UsefulValue[i] - ADJUST_M-ADJUST_ADC_M)/(ADJUST_ADC_T - ADJUST_M-ADJUST_ADC_M)+MIDD;
                else
                   g_AdcChange.ChangedValue[i] = MAX_ADC_CHANGED_VAL;
                
            }
        }

    }
    //SEGGER_RTT_printf(0, "ADC_1 = %d ADC_2 = %d ADC_3 = %d ADC_4 = %d\n",g_AdcChange.ChangedValue[0],g_AdcChange.ChangedValue[1],g_AdcChange.ChangedValue[2],g_AdcChange.ChangedValue[3]);
}

