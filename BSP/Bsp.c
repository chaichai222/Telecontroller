#include "Bsp.h"

void BspInit(void)
{
	AdcInit();
//	BeepInit();
	KeyInit();
	LedInit();
//	MotorInit();
	UartInit();	
//	InitEEPROM();
}
