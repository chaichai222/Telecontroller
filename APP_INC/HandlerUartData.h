#ifndef HANDLER_UART_DATA_H
#define HANDLER_UART_DATA_H


#define DATATYPE_TX_TO_PL  		0xA0
#define DATAID_TX_TO_PL   	 	0x81
#define MAX_BUFFER_LEN_UART_RX  200
#define MIN_BUFFER_LEN_UART_RX	4
#define MAX_VALID_DATA_LEN_RXPL	98


typedef struct UartRxBuf_st
{
	uint16_t RxSptr;		//start
	uint16_t RxEptr;		//end
	uint8_t RxBuf[MAX_BUFFER_LEN_UART_RX];
	uint8_t RxBufTemp[MAX_BUFFER_LEN_UART_RX];
}UartRxBuf;

/**
  *	enum: Data ID receive form PL
  */
typedef enum PCRx_ID_ENUM
{
	PLRXID_FLY_PARA = 0x02,
	
}PLRx_ID;

typedef enum PCTx_ID_ENUM
{
    PCTXID_REMOTE =  0x01,
//    PCTXID_SELFIE =  0x20,
//    PCTXID_FPV    =  0x30,
//    PCTXID_HAWA   =  0X81,
    TXID_NUM,
}PCTx_ID;

extern void InitUartToPLDataCorresponding(void);
extern void UartRxFromPLDataHandler(void);
extern void UartToTxDataHandler(void);
extern void InitUartRxBuffer(void);

#endif

