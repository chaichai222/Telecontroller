#include "stm32f1xx_hal.h"
#include "cmsis_os.h"
#include "BspUart.h"
#include "HalCallBack.h"
#include "HandlerUartData.h"
#include "main.h"

void HAL_UART_TxCpltCallback(UART_HandleTypeDef *huart)
{
  /* NOTE: This function should not be modified, when the callback is needed,
           the HAL_UART_TxCpltCallback can be implemented in the user file
   */ 
	if(huart->Instance == UART_INSTANCE_PL)
   	{
   		osSemaphoreRelease(g_BinarySemUartHandle);
	}
}

void CallBackTick(void)//1ms callback
{
	UartToTxDataHandler();
}

void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
	if(huart->Instance == UART_INSTANCE_PL)
   	{
   		UartRxFromPLDataHandler();
	}
	else if(huart->Instance == UART_INSTANCE_PS)
	{
		/* Waiting for writing */
	}

	/* set to recieve next data received by UART */
	HAL_UART_Receive_IT(&UART_PL, &g_Huart1RxOriginalData, 1);
}

