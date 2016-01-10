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

/*****************��������******************/

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
   
    if(osSemaphoreWait(g_CountingSemUartHandle,0) != osOK)  // ����һ���ź���
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

    //	���uart buffer
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



/****************��������*****************/

uint8_t UartPLDataCheckCode(uint8_t *Buf,uint16_t Len) //�����ֽ���������
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
	s_tUartRx.RxBuf[s_tUartRx.RxEptr++] = g_Huart1RxOriginalData;//�������ʣ�һ��ֻ�ܽ���һ�����ݣ�Ϊʲô�����ǽ����˺ܶ���
	s_tUartRx.RxEptr%=MAX_BUFFER_LEN_UART_RX;//��һ��������:�������ݻ������̶���СΪMAX_BUFFER_LEN_UART_RX��������0��ʼ
    
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
	DataType = *RxBuffer;//��һ���ֽ�
	DataLen = *(RxBuffer+1);//�ڶ����ֽ�
	DataID = *(RxBuffer+2);//�������ֽ�

	s_tUartRx.RxSptr = s_tUartRx.RxEptr;

	switch (DataID)
	{
		case PLRXID_FLY_PARA:
			PLRxHandleFlyPara(RxBuffer, DataLen);//ָ��ID��Ӧ�Ķ���
			break;

		default:
   	 		break;
	}
}

uint8_t UartRxPLSearchFrame(void)
{
	uint16_t bRet,bRxdSptr,bLen,i;  

	bRxdSptr=s_tUartRx.RxSptr;//ȡ����ʼָ��
	bLen = (s_tUartRx.RxEptr + MAX_BUFFER_LEN_UART_RX - bRxdSptr)%MAX_BUFFER_LEN_UART_RX;//���ܵ����ݳ���
	if(bLen<MIN_BUFFER_LEN_UART_RX)
		return FALSE;//С����С���Ȳ�ִ��

	for(i=0; (i<bLen)&&(i<MAX_BUFFER_LEN_UART_RX); i++)
	{
		s_tUartRx.RxBufTemp[i] = s_tUartRx.RxBuf[(bRxdSptr+i)%MAX_BUFFER_LEN_UART_RX];//ȡ����Ч����
	}

	while(bLen>=MIN_BUFFER_LEN_UART_RX)//������֤�Ƿ���ָ��
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

	if(Len<MIN_BUFFER_LEN_UART_RX)  //����Ϊʲô������
		return FM_LESS;
		
	if(Buf[0]!=DATATYPE_RX_FROM_PL)//�Ƿ�Ϊ0x0A�������
		return FM_ERR;

	LenValid = Buf[1];
	if(LenValid>MAX_VALID_DATA_LEN_RXPL)//��֤����
	{
		return FM_ERR;
	}

	DataID = Buf[2];

	switch (DataID)       //�����޸�ID����Ӧ���ǽ��ܵ�ָ��ID
	{
		case PLRXID_FLY_PARA:
			break;

		default:
   	 		return FM_ERR;
	}

	if(LenValid>Len)      //LenValid����ȷ��ֵ
	{
		return FM_LESS; //û�д������
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
