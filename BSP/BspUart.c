#include "BspUart.h"
#include "cmsis_os.h"
#include "stm32f1xx_hal.h"
#include <string.h>
#include "HandlerUartData.h"
#include "main.h"
#include "SEGGER_RTT.h"


uint8_t g_Huart1RxOriginalData;
uint8_t g_ToQueue[90];
QUEUE_UART_CONTROL g_QueueUartControl[UART_TX_TO_ZYNQ_NUM];

void UartInit(void)
{
	memset(g_QueueUartControl,0,sizeof(g_QueueUartControl));
	InitUartRxBuffer();
	
	InitUartToPLDataCorresponding();

//	/*  UART_PL receive Initial */
	HAL_UART_Receive_IT(&huart1, &g_Huart1RxOriginalData, 1);
}

void CallBackUartTxToPC(void)
{
	osEvent ToPLEvent;
	//s32 PLAvailableSem;
	//u32 PrintTemp;
	ToPLEvent = osMessageGet (g_QueueToUartHandle, osWaitForever);
	if(ToPLEvent.status == osEventMessage)
	{
		//PrintTemp = ((QUEUE_UART_FORMAT*)(ToPLEvent.value.p))->Size;
		//SEGGER_RTT_printf(0, "TxSize = %d\n",PrintTemp);
		HAL_UART_Transmit_DMA(&UART_PL,// g_ToQueue,sizeof(g_ToQueue));
								(((QUEUE_UART_FORMAT*)(ToPLEvent.value.p))->Buf), 
								(((QUEUE_UART_FORMAT*)(ToPLEvent.value.p))->Size));
		
		/*PLAvailableSem =  */
        osSemaphoreWait (g_BinarySemUartHandle, osWaitForever);//wait for transmit completed
		osSemaphoreRelease(g_CountingSemUartHandle);//release Tx Buf[index]
	}
}


