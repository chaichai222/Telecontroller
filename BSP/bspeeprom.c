#include "TypeDef.h"
#include "bspi2c.h"
#include "bspeeprom.h"

void InitEEPROMData(void);

void InitEEPROM(void)
{
	Init_I2cMaster();
	InitEEPROMData();
}



void InitEEPROMData(void)
{
	
}

uint8_t EEPROM24AA512ReadNByte(uint16_t Register, uint8_t *Data, uint8_t Num)
{
	if(I2C_ReadData(g_I2cHandleMaster,EEPROM24AA512_SLADDRESS,(uint16_t)Register,I2C_MEMADD_SIZE_16BIT, Data, Num) != HAL_OK)
	{
		 return 0; 
	}
	return 1;
}




uint8_t EEPROM24AA512WriteNByte(uint16_t Register, uint8_t *Buffer, uint8_t Length)
{
	uint8_t result = 1;
	
	if(I2C_WriteData(g_I2cHandleMaster, EEPROM24AA512_SLADDRESS, (uint16_t)Register, I2C_MEMADD_SIZE_16BIT, Buffer, Length) != HAL_OK)
		result = 0;
	
	return result;
}

