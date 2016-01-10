#ifndef MAIM_H
#define MAIN_H

extern UART_HandleTypeDef huart1;
extern UART_HandleTypeDef huart3;
extern osMessageQId g_QueueToUartHandle;
extern osSemaphoreId g_CountingSemUartHandle;
extern osSemaphoreId g_BinarySemUartHandle;
#endif

