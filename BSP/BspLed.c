#include "BspLed.h"
#include "stm32f1xx_hal.h"
#include <string.h>

LED_STATUS LedStatus[LED_NUM];
static void LedControl(LED_ID LedId,SWITCH sw);
//static void LedToggle(LED_ID LedId);

static void LedControl(LED_ID LedId,SWITCH sw)
{
	GPIO_PinState PinStateTemp = ((sw == ON)?(GPIO_PIN_RESET):(GPIO_PIN_SET));
	HAL_GPIO_WritePin(LedStatus[LedId].LedPinStruct.GPIOX, LedStatus[LedId].LedPinStruct.PIN, PinStateTemp);	
}

/*static void LedToggle(LED_ID LedId)
{
	HAL_GPIO_TogglePin(LedStatus[LedId].LedPinStruct.GPIOX, LedStatus[LedId].LedPinStruct.PIN);	
}*/

void LedInit(void)
{
	memset(LedStatus,0,sizeof(LedStatus));
	LedStatus[LED].LedPinStruct.GPIOX	= LED_GPIO_Port;
	LedStatus[LED].LedPinStruct.PIN 	= LED_Pin;

}
/************************************

    LED随按键按下亮起

************************************/

void LedPro(void)
{
    HAL_GPIO_TogglePin(LED_GPIO_Port, LED_Pin);
}

