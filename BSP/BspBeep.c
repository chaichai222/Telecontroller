#include "BspBeep.h"
#include "stm32f1xx_hal.h"
#include "cmsis_os.h"

extern osTimerId TimerBeepHandle;

static BEEP_STATUS BeepStatus;
void BeepInit(void)
{
	BeepStatus.BeepPinStruct.GPIOX = BEEP_WARNING_GPIOX;
	BeepStatus.BeepPinStruct.PIN   = BEEP_WARNING_PIN;
	BeepStop();
}

void BeepStart(u32 ContinueMillisec)
{
	HAL_GPIO_WritePin(BeepStatus.BeepPinStruct.GPIOX, BeepStatus.BeepPinStruct.PIN, GPIO_PIN_SET);		
	osTimerStart(TimerBeepHandle, ContinueMillisec);
}

void BeepStop(void)
{
	HAL_GPIO_WritePin(BeepStatus.BeepPinStruct.GPIOX, BeepStatus.BeepPinStruct.PIN, GPIO_PIN_RESET);	
}

void BeepPro(void)
{
	BeepStop();
}

