#ifndef BSP_ADC_H
#define BSP_ADC_H

#include "TypeDef.h"
#include "stm32f1xx_hal.h"
 	
#define		SOURCE_NUM    	(10)//Դ������
#define     DELETE_MAX_NUM  (3)//ɾ�����ļ���
#define     DELETE_MIN_NUM  (3)//ɾ����С�ļ���
#define     USEFUL_NUM  	(SOURCE_NUM - DELETE_MAX_NUM - DELETE_MIN_NUM)//������  4

/*
	led enum
*/
typedef enum ADC_ID_ENUM
{
    ADC_1 = 0 ,
    ADC_2     ,
    ADC_3     ,
    ADC_4     ,
    
    ADC_NUM ,
}ADC_ID;
/*
	led struct
*/

typedef struct ADC_STATUS_STRU
{
	u32 Value[SOURCE_NUM][ADC_NUM];
	u32 UsefulValue[ADC_NUM];
	
}ADC_STATUS;

typedef struct ADC_CHANGE_STRU
{
  
	u32 L_AdjustValue[ADC_NUM];
	u32 H_AdjustValue[ADC_NUM];
	u16 ChangedValue[ADC_NUM];                               
	
}ADC_CHANGE;

extern void AdcPro(void);
extern ADC_CHANGE g_AdcChange;
extern ADC_HandleTypeDef hadc1;
extern void AdcInit(void);
#endif

