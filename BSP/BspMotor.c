#include "BspMotor.h"
#include "stm32f1xx_hal.h"
#include "cmsis_os.h"

extern osTimerId TimerMotorHandle;

static MOTOR_STATUS MotorStatus;
void MotorInit(void)
{
	MotorStatus.MotorPinStruct.GPIOX = MOTOR_WARNING_GPIOX;
	MotorStatus.MotorPinStruct.PIN   = MOTOR_WARNING_PIN;
	MotorStop();
}

void MotorStart(u32 ContinueMillisec)
{
	HAL_GPIO_WritePin(MotorStatus.MotorPinStruct.GPIOX, MotorStatus.MotorPinStruct.PIN, GPIO_PIN_SET);		
	osTimerStart(TimerMotorHandle, ContinueMillisec);
}

void MotorStop(void)
{
	HAL_GPIO_WritePin(MotorStatus.MotorPinStruct.GPIOX, MotorStatus.MotorPinStruct.PIN, GPIO_PIN_RESET);	
}

void MotorPro(void)
{
	MotorStop();
}

