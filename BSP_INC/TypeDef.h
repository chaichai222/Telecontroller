#ifndef TYPE_DEF_H
#define TYPE_DEF_H

#include <stdint.h>
#include "stm32f1xx_hal.h"

typedef uint8_t 	u8;
typedef uint16_t 	u16;
typedef uint32_t 	u32;

typedef int8_t 	s8;
typedef int16_t s16;
typedef int32_t s32;

/* 有符号整型 */
typedef signed char int8_t;
typedef signed short int int16_t;
typedef signed int int32_t;

/* 无符号整型 */
typedef unsigned char uint8_t;
typedef unsigned short int uint16_t;
typedef unsigned int uint32_t;

typedef enum
{
	FALSE = 0,
	TRUE,
}BOOL;

typedef enum
{
	OFF = 0,
	ON,
}SWITCH;


typedef struct PIN_STRU
{
	GPIO_TypeDef* GPIOX;//10ms 
	u16	PIN;
}PIN_STRUCT;



#endif

