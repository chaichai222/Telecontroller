#ifndef BSP_UART_H
#define BSP_UART_H

#include "TypeDef.h"
#include "stm32f1xx_hal.h"

#define UART_BUF_SIZE 16
#define UART_QUEUE_SIZE 4
#define UART_FLOW_CONTROL_THROUGH_RTOS_SEM


#define UART_PL huart1
#define UART_PS huart3
#define UART_INSTANCE_PL  USART1
#define UART_INSTANCE_PS  USART3

/*
	led enum
*/
typedef enum UART_ID_ENUM
{	
	UART_TX_TO_ZYNQ_PL			= 0,
//	UART_TX_TO_ZYNQ_PS			,	
	UART_TX_TO_ZYNQ_NUM,		
}UART_ID;

typedef struct QUEUE_UART_FORMAT_STRU
{
	u8 Size;
	u8 Buf[UART_BUF_SIZE];
}QUEUE_UART_FORMAT;

typedef struct QUEUE_UART_CONTROL_STRU
{
	QUEUE_UART_FORMAT QueueUart[UART_QUEUE_SIZE];
#ifdef UART_FLOW_CONTROL_THROUGH_RTOS_SEM
	u8 Index;
#else
	u8 Tail;
	u8 Head;
#endif
}QUEUE_UART_CONTROL;


extern void UartInit(void);
extern void CallBackUartTxToPC(void);

extern QUEUE_UART_CONTROL g_QueueUartControl[UART_TX_TO_ZYNQ_NUM];
extern u8 g_Huart1RxOriginalData;

extern uint8_t g_ToQueue[];

#endif

