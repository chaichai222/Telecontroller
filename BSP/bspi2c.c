#include "cmsis_os.h"
#include "stm32f1xx_hal.h"
//#include "main.h"
#include "TypeDef.h"
#include "bspi2c.h"
#include "stm32f1xx_hal_i2c.h"

I2C_HandleTypeDef* g_I2cHandleMaster;

void Init_I2cMaster(void)
{
  g_I2cHandleMaster = &hi2c1;
  g_I2cHandleMaster->hdmarx = &hdma_i2c1_rx;
  g_I2cHandleMaster->hdmatx = &hdma_i2c1_tx;  
}


HAL_StatusTypeDef I2C_ReadData(I2C_HandleTypeDef *hi2c, uint16_t DevAddr, uint16_t MemAddr, uint16_t MemAddrSize, uint8_t *pData, uint16_t Size)
{
	HAL_StatusTypeDef status = HAL_OK;
	
	status = HAL_I2C_Mem_Read_DMA(hi2c, DevAddr, MemAddr, MemAddrSize, pData, Size);
	if(status != HAL_OK )
  	{
  	#ifdef SUPPORT_PRINT
		SEGGER_RTT_printf(0,"I2C_ReadData ERROR at %d\r\n", status);
	#endif
	}
	else
	{
	#ifdef SUPPORT_PRINT
		//SEGGER_RTT_printf(0,"I2C Read OK!!!!!\r\n");
	#endif
	}
	osDelay(((Size/25)<<1)+10);	
	if (HAL_I2C_GetState(hi2c) != HAL_I2C_STATE_READY)
	{
	#ifdef SUPPORT_PRINT
		SEGGER_RTT_printf(0,"I2C_State read err at %d\r\n", status);
	#endif
		osDelay(100);
	} 
	
	return status;
}


HAL_StatusTypeDef I2C_WriteData(I2C_HandleTypeDef *hi2c, uint16_t DevAddr, uint16_t MemAddr, uint16_t MemAddrSize, uint8_t *pData, uint16_t Size)
{

	HAL_StatusTypeDef status = HAL_OK;
	
	status = HAL_I2C_Mem_Write_DMA(hi2c, DevAddr, MemAddr, MemAddrSize, pData, Size);
	if( status != HAL_OK )
  	{
  	#ifdef SUPPORT_PRINT
		SEGGER_RTT_printf(0,"I2C Write ERROR at %d\r\n", status);
	#endif
	}
	else
	{
	#ifdef SUPPORT_PRINT
		//SEGGER_RTT_printf(0,"I2C Write OK!!!!!\r\n");
	#endif
	}
  
	osDelay(((Size/25)<<1)+10);//osDelay(((Size/25)<<1)+8);
	if (HAL_I2C_GetState(hi2c) != HAL_I2C_STATE_READY)
	{
	#ifdef SUPPORT_PRINT
		SEGGER_RTT_printf(0,"I2C_State write err at %d\r\n", status);
	#endif
		osDelay(100);
	}

	return status;	
}
