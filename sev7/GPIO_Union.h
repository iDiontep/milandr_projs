#ifndef __GPIO_UNION_H
#define __GPIO_UNION_H

#include "MDR32F9Q2I.h" 

typedef struct
{
	uint32_t Pin_0  :1; 
	uint32_t Pin_1  :1; 
	uint32_t Pin_2  :1; 
	uint32_t Pin_3  :1; 
	uint32_t Pin_4  :1; 
	uint32_t Pin_5  :1; 
	uint32_t Pin_6  :1; 
	uint32_t Pin_7  :1; 
	uint32_t Pin_8  :1; 
	uint32_t Pin_9  :1; 
	uint32_t Pin_10 :1; 
	uint32_t Pin_11 :1; 
	uint32_t Pin_12 :1; 
	uint32_t Pin_13 :1; 
	uint32_t Pin_14 :1; 
	uint32_t Pin_15 :1; 
	
	uint32_t 				:16; // Reserved
	
}MDR_PORT_RXTX_RegisterBitMap_TypeDef;

typedef struct
{
	uint32_t Pin_0  :1; 
	uint32_t Pin_1  :1; 
	uint32_t Pin_2  :1; 
	uint32_t Pin_3  :1; 
	uint32_t Pin_4  :1; 
	uint32_t Pin_5  :1; 
	uint32_t Pin_6  :1; 
	uint32_t Pin_7  :1; 
	uint32_t Pin_8  :1; 
	uint32_t Pin_9  :1; 
	uint32_t Pin_10 :1; 
	uint32_t Pin_11 :1; 
	uint32_t Pin_12 :1; 
	uint32_t Pin_13 :1; 
	uint32_t Pin_14 :1; 
	uint32_t Pin_15 :1; 
	
	uint32_t 				:16; // Reserved
	
}MDR_PORT_OE_RegisterBitMap_TypeDef;

typedef struct
{
	uint32_t Pin_0  :2; 
	uint32_t Pin_1  :2; 
	uint32_t Pin_2  :2; 
	uint32_t Pin_3  :2; 
	uint32_t Pin_4  :2; 
	uint32_t Pin_5  :2; 
	uint32_t Pin_6  :2; 
	uint32_t Pin_7  :2; 
	uint32_t Pin_8  :2; 
	uint32_t Pin_9  :2; 
	uint32_t Pin_10 :2; 
	uint32_t Pin_11 :2; 
	uint32_t Pin_12 :2; 
	uint32_t Pin_13 :2; 
	uint32_t Pin_14 :2; 
	uint32_t Pin_15 :2; 
	
}MDR_PORT_FUNC_RegisterBitMap_TypeDef;


typedef struct
{
	uint32_t Pin_0  :1; 
	uint32_t Pin_1  :1; 
	uint32_t Pin_2  :1; 
	uint32_t Pin_3  :1; 
	uint32_t Pin_4  :1; 
	uint32_t Pin_5  :1; 
	uint32_t Pin_6  :1; 
	uint32_t Pin_7  :1; 
	uint32_t Pin_8  :1; 
	uint32_t Pin_9  :1; 
	uint32_t Pin_10 :1; 
	uint32_t Pin_11 :1; 
	uint32_t Pin_12 :1; 
	uint32_t Pin_13 :1; 
	uint32_t Pin_14 :1; 
	uint32_t Pin_15 :1; 
	
	uint32_t 				:16; // Reserved
	
}MDR_PORT_ANALOG_RegisterBitMap_TypeDef;


typedef struct
{
	uint32_t PULL_DOWN_Pin_0  :1; 
	uint32_t PULL_DOWN_Pin_1  :1; 
	uint32_t PULL_DOWN_Pin_2  :1; 
	uint32_t PULL_DOWN_Pin_3  :1; 
	uint32_t PULL_DOWN_Pin_4  :1; 
	uint32_t PULL_DOWN_Pin_5  :1; 
	uint32_t PULL_DOWN_Pin_6  :1; 
	uint32_t PULL_DOWN_Pin_7  :1; 
	uint32_t PULL_DOWN_Pin_8  :1; 
	uint32_t PULL_DOWN_Pin_9  :1; 
	uint32_t PULL_DOWN_Pin_10 :1; 
	uint32_t PULL_DOWN_Pin_11 :1; 
	uint32_t PULL_DOWN_Pin_12 :1; 
	uint32_t PULL_DOWN_Pin_13 :1; 
	uint32_t PULL_DOWN_Pin_14 :1; 
	uint32_t PULL_DOWN_Pin_15 :1; 
	
	uint32_t PULL_UP_Pin_0  	:1; 
	uint32_t PULL_UP_Pin_1  	:1; 
	uint32_t PULL_UP_Pin_2  	:1; 
	uint32_t PULL_UP_Pin_3  	:1; 
	uint32_t PULL_UP_Pin_4  	:1; 
	uint32_t PULL_UP_Pin_5  	:1; 
	uint32_t PULL_UP_Pin_6  	:1; 
	uint32_t PULL_UP_Pin_7  	:1; 
	uint32_t PULL_UP_Pin_8  	:1; 
	uint32_t PULL_UP_Pin_9  	:1; 
	uint32_t PULL_UP_Pin_10 	:1; 
	uint32_t PULL_UP_Pin_11 	:1; 
	uint32_t PULL_UP_Pin_12 	:1; 
	uint32_t PULL_UP_Pin_13 	:1; 
	uint32_t PULL_UP_Pin_14 	:1; 
	uint32_t PULL_UP_Pin_15 	:1; 
	
}MDR_PORT_PULL_RegisterBitMap_TypeDef;


typedef struct
{
	uint32_t DriverMode_Pin_0  :1; 
	uint32_t DriverMode_Pin_1  :1; 
	uint32_t DriverMode_Pin_2  :1; 
	uint32_t DriverMode_Pin_3  :1; 
	uint32_t DriverMode_Pin_4  :1; 
	uint32_t DriverMode_Pin_5  :1; 
	uint32_t DriverMode_Pin_6  :1; 
	uint32_t DriverMode_Pin_7  :1; 
	uint32_t DriverMode_Pin_8  :1; 
	uint32_t DriverMode_Pin_9  :1; 
	uint32_t DriverMode_Pin_10 :1; 
	uint32_t DriverMode_Pin_11 :1; 
	uint32_t DriverMode_Pin_12 :1; 
	uint32_t DriverMode_Pin_13 :1; 
	uint32_t DriverMode_Pin_14 :1; 
	uint32_t DriverMode_Pin_15 :1; 
	
	uint32_t SHM_Pin_0  	:1; 
	uint32_t SHM_Pin_1  	:1; 
	uint32_t SHM_Pin_2  	:1; 
	uint32_t SHM_Pin_3  	:1; 
	uint32_t SHM_Pin_4  	:1; 
	uint32_t SHM_Pin_5  	:1; 
	uint32_t SHM_Pin_6  	:1; 
	uint32_t SHM_Pin_7  	:1; 
	uint32_t SHM_Pin_8  	:1; 
	uint32_t SHM_Pin_9  	:1; 
	uint32_t SHM_Pin_10 	:1; 
	uint32_t SHM_Pin_11 	:1; 
	uint32_t SHM_Pin_12 	:1; 
	uint32_t SHM_Pin_13 	:1; 
	uint32_t SHM_Pin_14 	:1; 
	uint32_t SHM_Pin_15 	:1; 
	
}MDR_PORT_PD_RegisterBitMap_TypeDef;


typedef struct
{
	uint32_t Pin_0  :2; 
	uint32_t Pin_1  :2; 
	uint32_t Pin_2  :2; 
	uint32_t Pin_3  :2; 
	uint32_t Pin_4  :2; 
	uint32_t Pin_5  :2; 
	uint32_t Pin_6  :2; 
	uint32_t Pin_7  :2; 
	uint32_t Pin_8  :2; 
	uint32_t Pin_9  :2; 
	uint32_t Pin_10 :2; 
	uint32_t Pin_11 :2; 
	uint32_t Pin_12 :2; 
	uint32_t Pin_13 :2; 
	uint32_t Pin_14 :2; 
	uint32_t Pin_15 :2; 
	
}MDR_PORT_PWR_RegisterBitMap_TypeDef;


typedef struct
{
	uint32_t Pin_0  :1; 
	uint32_t Pin_1  :1; 
	uint32_t Pin_2  :1; 
	uint32_t Pin_3  :1; 
	uint32_t Pin_4  :1; 
	uint32_t Pin_5  :1; 
	uint32_t Pin_6  :1; 
	uint32_t Pin_7  :1; 
	uint32_t Pin_8  :1; 
	uint32_t Pin_9  :1; 
	uint32_t Pin_10 :1; 
	uint32_t Pin_11 :1; 
	uint32_t Pin_12 :1; 
	uint32_t Pin_13 :1; 
	uint32_t Pin_14 :1; 
	uint32_t Pin_15 :1; 
	
	uint32_t 				:16; // Reserved
	
}MDR_PORT_GFEN_RegisterBitMap_TypeDef;



typedef struct
{
	union
	{
		__IO uint32_t RXTX;
		MDR_PORT_RXTX_RegisterBitMap_TypeDef RXTX_Bit;
	};
	
	union
	{
		__IO uint32_t OE;
		MDR_PORT_OE_RegisterBitMap_TypeDef OE_Bit;
	};
	
	union
	{
		__IO uint32_t FUNC;
		MDR_PORT_FUNC_RegisterBitMap_TypeDef FUNC_Bit;
	};
	
	union
	{
		__IO uint32_t ANALOG;
		MDR_PORT_ANALOG_RegisterBitMap_TypeDef ANALOG_Bit;
	};
	
	union
	{
		__IO uint32_t PULL;
		MDR_PORT_PULL_RegisterBitMap_TypeDef PULL_Bit;
	};
	
	union
	{
		__IO uint32_t PD;
		MDR_PORT_PD_RegisterBitMap_TypeDef PD_Bit;
	};
	
	union
	{
		__IO uint32_t PWR;
		MDR_PORT_PWR_RegisterBitMap_TypeDef PWR_Bit;
	};
	
	union
	{
		__IO uint32_t GFEN;
		MDR_PORT_GFEN_RegisterBitMap_TypeDef GFEN_Bit;
	};
	
}MDR_PORT_UnionTypeDef;

#define MDR_UNION_PORTA           ((MDR_PORT_UnionTypeDef    *) MDR_PORTA_BASE)
#define MDR_UNION_PORTB           ((MDR_PORT_UnionTypeDef    *) MDR_PORTB_BASE)
#define MDR_UNION_PORTC           ((MDR_PORT_UnionTypeDef    *) MDR_PORTC_BASE)
#define MDR_UNION_PORTD           ((MDR_PORT_UnionTypeDef    *) MDR_PORTD_BASE)
#define MDR_UNION_PORTE           ((MDR_PORT_UnionTypeDef    *) MDR_PORTE_BASE)
#define MDR_UNION_PORTF           ((MDR_PORT_UnionTypeDef    *) MDR_PORTF_BASE)

#define ENABLE_BIT															1
#define ENABLE_DOUBLE_BIT												3

#define DISABLE_BIT															0
#define DISABLE_DOUBLE_BIT											0

#define PORTA_CLK_BIT_POS												21
#define PORTB_CLK_BIT_POS												22
#define PORTC_CLK_BIT_POS												23
#define PORTD_CLK_BIT_POS												24
#define PORTE_CLK_BIT_POS												25
#define PORTF_CLK_BIT_POS												29

void GPIO_Led_Init( void );
void GPIO_Button_Init( void );
void Previous_code_from_while( void );

#endif 
