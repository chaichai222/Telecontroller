#include "stm32f1xx_hal.h"
#include "cmsis_os.h"
#include "BspUart.h"
#include "HalCallBack.h"
#include "Bsp.h"
#include "HandlerUartData.h"
#include "main.h"
#include "SEGGER_RTT.h"

#define PRIMARY_OPERATION_MODE	1000
#define MIDDLE_OPERATION_MODE	1500
#define ADVANCED_OPERATION_MODE	2000
#define TIME_UART_TX			1000//20
#define NUM_KEYS_TO_SEND		2
#define NUM_ADCS_TO_SEND 		4
#define FM_OK       			0x01     
#define FM_ERR      			0x02     
#define FM_LESS     			0x03 
#define DATATYPE_RX_FROM_PL		0x0A

static UartRxBuf s_tUartRx;
static uint16_t s_NumUartSendTime = 0;
static uint8_t 	s_IndexQueue = 0;
static uint8_t  s_tAdcSendCorresponding[NUM_ADCS_TO_SEND];
static uint8_t  s_tKeySendCorresponding[NUM_KEYS_TO_SEND];
//static uint8_t  message[] = {"\nUart_Rx is ok !\n"};
uint8_t UartPLDataCheckCode(uint8_t *Buf,uint16_t Len);
void 	UartToPCTxPutData(void);
void	PackUartToPCData(void);
void 	UartRxFromPLGetValidData(void);
uint8_t UartRxPLSearchFrame(void);
uint8_t UartPLCheckFrame(uint8_t *Buf,uint16_t Len);
void 	PLRxHandleFlyPara(uint8_t *Buffer,uint16_t BufLength);

void InitUartToPLDataCorresponding(void)
{
    s_tAdcSendCorresponding[0] = ADC_1;
	s_tAdcSendCorresponding[1] = ADC_2;
	s_tAdcSendCorresponding[2] = ADC_3;
	s_tAdcSendCorresponding[3] = ADC_4;
	s_tKeySendCorresponding[0] = K3;
	s_tKeySendCorresponding[1] = K4;
}

void InitUartRxBuffer(void)
    
{
	memset(&s_tUartRx,0,sizeof(UartRxBuf));
}

/*****************发送数据******************/

/** Uart send data every 20 ms
  */
void UartToTxDataHandler(void)
{
	s_NumUartSendTime ++;
	
	if(s_NumUartSendTime >= TIME_UART_TX) 
	{
		s_NumUartSendTime = 0;
		UartToPCTxPutData();
	}
}

void UartToPCTxPutData(void)
{
    
    uint16_t IndexBuffer = 0;
    uint16_t IndexCorresponding = 0;
    uint16_t ValCorresponding = 0;
    uint8_t i;
   
    if(osSemaphoreWait(g_CountingSemUartHandle,0) != osOK)  // 申请一个信号量
    {
            // print error
        SEGGER_RTT_printf(0, "ERROR\n");
       return;
    }    

    if(s_IndexQueue < (UART_QUEUE_SIZE-1))
    {
        s_IndexQueue = g_QueueUartControl[UART_TX_TO_ZYNQ_PL].Index;
        g_QueueUartControl[UART_TX_TO_ZYNQ_PL].Index ++;
    }
    else
    {
        g_QueueUartControl[UART_TX_TO_ZYNQ_PL].Index = 0;
        s_IndexQueue = g_QueueUartControl[UART_TX_TO_ZYNQ_PL].Index;
        g_QueueUartControl[UART_TX_TO_ZYNQ_PL].Index ++;
    }
    
    //s_IndexQueue = g_QueueUartControl[UART_TX_TO_ZYNQ_PL].Index;

    //	填充uart buffer
    g_QueueUartControl[UART_TX_TO_ZYNQ_PL].QueueUart[s_IndexQueue].Buf[IndexBuffer++] = DATATYPE_TX_TO_PL;	
    g_QueueUartControl[UART_TX_TO_ZYNQ_PL].QueueUart[s_IndexQueue].Buf[IndexBuffer++] = 0;
    g_QueueUartControl[UART_TX_TO_ZYNQ_PL].QueueUart[s_IndexQueue].Buf[IndexBuffer++] = PCTXID_REMOTE;
     
    for(i=0;i<6;i++)
    {
        if(i<4)
        {
            IndexCorresponding = s_tAdcSendCorresponding[i];
            ValCorresponding = g_AdcChange.ChangedValue[IndexCorresponding];             
            g_QueueUartControl[UART_TX_TO_ZYNQ_PL].QueueUart[s_IndexQueue].Buf[IndexBuffer++] = (ValCorresponding>>8)&0x00FF;
            g_QueueUartControl[UART_TX_TO_ZYNQ_PL].QueueUart[s_IndexQueue].Buf[IndexBuffer++] = ValCorresponding&0x00FF;
        }
        if(i>=4)
        {
            IndexCorresponding = s_tKeySendCorresponding[4+i];
            ValCorresponding = g_KeyChangedVal[IndexCorresponding];             
            g_QueueUartControl[UART_TX_TO_ZYNQ_PL].QueueUart[s_IndexQueue].Buf[IndexBuffer++] = (ValCorresponding>>8)&0x00FF;
            g_QueueUartControl[UART_TX_TO_ZYNQ_PL].QueueUart[s_IndexQueue].Buf[IndexBuffer++] = ValCorresponding&0x00FF;
        }
    }
    g_QueueUartControl[UART_TX_TO_ZYNQ_PL].QueueUart[s_IndexQueue].Buf[1] = IndexBuffer + 1;
    g_QueueUartControl[UART_TX_TO_ZYNQ_PL].QueueUart[s_IndexQueue].Buf[IndexBuffer] 
        = UartPLDataCheckCode(g_QueueUartControl[UART_TX_TO_ZYNQ_PL].QueueUart[s_IndexQueue].Buf, IndexBuffer);
    
    IndexBuffer++;
    g_QueueUartControl[UART_TX_TO_ZYNQ_PL].QueueUart[s_IndexQueue].Size = IndexBuffer;
    
    osMessagePut(g_QueueToUartHandle, (uint32_t)(&g_QueueUartControl[UART_TX_TO_ZYNQ_PL].QueueUart[s_IndexQueue]), osWaitForever);        
}



/****************接受数据*****************/

uint8_t UartPLDataCheckCode(uint8_t *Buf,uint16_t Len) //返回字节总数检验
{
	u32 sum_buffer = 0;
	uint8_t i,Ret_ValCheckOut;

	for(i=0; i<Len; i++)
	{
		sum_buffer += Buf[i];
	}

	Ret_ValCheckOut = sum_buffer & 0xFF;

	return Ret_ValCheckOut;
}
void UartRxFromPLDataHandler(void)
{
	s_tUartRx.RxBuf[s_tUartRx.RxEptr++] = g_Huart1RxOriginalData;//这里疑问？一次只能接受一个数据，为什么这里是接受了很多呢
	s_tUartRx.RxEptr%=MAX_BUFFER_LEN_UART_RX;//这一步的意义:接受数据缓冲区固定大小为MAX_BUFFER_LEN_UART_RX，超过从0开始
    
	UartRxFromPLGetValidData();
}

void UartRxFromPLGetValidData(void)
{
	uint8_t *RxBuffer;
	uint8_t DataLen,DataID,DataType;
	//uint8_t ;
	
	if(UartRxPLSearchFrame()!= TRUE)
		return ;

	
	RxBuffer = &s_tUartRx.RxBuf[s_tUartRx.RxSptr];
	DataType = *RxBuffer;//第一个字节
	DataLen = *(RxBuffer+1);//第二个字节
	DataID = *(RxBuffer+2);//第三个字节

	s_tUartRx.RxSptr = s_tUartRx.RxEptr;

	switch (DataID)
	{
		case PLRXID_FLY_PARA:
			PLRxHandleFlyPara(RxBuffer, DataLen);//指令ID对应的动作
			break;

		default:
   	 		break;
	}
}

uint8_t UartRxPLSearchFrame(void)
{
	uint16_t bRet,bRxdSptr,bLen,i;  

	bRxdSptr=s_tUartRx.RxSptr;//取出开始指针
	bLen = (s_tUartRx.RxEptr + MAX_BUFFER_LEN_UART_RX - bRxdSptr)%MAX_BUFFER_LEN_UART_RX;//接受的数据长度
	if(bLen<MIN_BUFFER_LEN_UART_RX)
		return FALSE;//小于最小长度不执行

	for(i=0; (i<bLen)&&(i<MAX_BUFFER_LEN_UART_RX); i++)
	{
		s_tUartRx.RxBufTemp[i] = s_tUartRx.RxBuf[(bRxdSptr+i)%MAX_BUFFER_LEN_UART_RX];//取出有效数据
	}

	while(bLen>=MIN_BUFFER_LEN_UART_RX)//这里验证是否是指令
	{
		bRet = UartPLCheckFrame(s_tUartRx.RxBufTemp, bLen);

		if(bRet == FM_OK)
		{
			s_tUartRx.RxSptr = bRxdSptr;
			return TRUE;
		}
		if(bRet == FM_LESS)
		{
			s_tUartRx.RxSptr = bRxdSptr;
			return FALSE;
		}

		bRxdSptr ++;
		bRxdSptr %= MAX_BUFFER_LEN_UART_RX;
		if(bRxdSptr == s_tUartRx.RxEptr)
		{
			s_tUartRx.RxSptr = s_tUartRx.RxEptr = 0;
			return FALSE;
		}

		bLen--;
		for(i=0;(i<bLen)&&(i<MAX_BUFFER_LEN_UART_RX);i++)
		{
			s_tUartRx.RxBufTemp[i] = s_tUartRx.RxBuf[(bRxdSptr+i)%MAX_BUFFER_LEN_UART_RX];
		}
	}

	s_tUartRx.RxSptr = bRxdSptr;
	return FALSE;
}

uint8_t UartPLCheckFrame(uint8_t *Buf,uint16_t Len)
{
	uint16_t LenValid;
	uint8_t  DataID, CheckCode;

	if(Len<MIN_BUFFER_LEN_UART_RX)  //这里为什么是这样
		return FM_LESS;
		
	if(Buf[0]!=DATATYPE_RX_FROM_PL)//是否为0x0A，否出错
		return FM_ERR;

	LenValid = Buf[1];
	if(LenValid>MAX_VALID_DATA_LEN_RXPL)//验证长度
	{
		return FM_ERR;
	}

	DataID = Buf[2];

	switch (DataID)       //这里修改ID，对应的是接受的指令ID
	{
		case PLRXID_FLY_PARA:
			break;

		default:
   	 		return FM_ERR;
	}

	if(LenValid>Len)      //LenValid是正确的值
	{
		return FM_LESS; //没有传输完成
	}

	CheckCode = UartPLDataCheckCode(Buf, (LenValid-1));

	if(CheckCode == Buf[LenValid-1])
	{
		return FM_OK;
	}
	else
	{
		return FM_ERR;
	}
}

/**
  * @brief  handle fly parameter received form PL.
  * @param  None
  * @retval None
  */
void PLRxHandleFlyPara(uint8_t *Buffer,uint16_t BufLength)
{
//        HAL_UART_Transmit(&huart1, message, sizeof(message)-1, 2);

//        HAL_UART_Transmit(&huart1, Buffer, BufLength, 2);
//    uint8_t i,*Buf,k;
//    
//    Buf=Buffer;
//    k=BufLength;
//    
//    for(i=0;i<k;i++)
//    {
//        useful_data[i]=Buf[i];
//    }
}
