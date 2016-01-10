#ifndef BSP_EEPROM_H
#define BSP_EEPROM_H

#define EEPROM24AA512_SLADDRESS 		0xA0


uint8_t EEPROM24AA512WriteNByte(uint16_t Register, uint8_t *Buffer, uint8_t Length);

uint8_t EEPROM24AA512ReadNByte(uint16_t Register, uint8_t *Data, uint8_t Num);

extern void InitEEPROM(void);


#endif

