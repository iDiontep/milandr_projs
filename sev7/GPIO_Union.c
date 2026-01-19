#include "GPIO_Union.h"

/************************************************************** Defines *****************************************************************************/
// Program configuration 
// #define BUTTON_CONTINIOUSLY_PRESSED_BLINKING
#define BUTTON_PRESSED_INVERT_BIT

#if ( defined BUTTON_CONTINIOUSLY_PRESSED_BLINKING ) && ( defined BUTTON_PRESSED_INVERT_BIT )
	#error Must be chosen only one program mode!
#endif
void Delay( void );


void GPIO_Led_Init( void )
{
	/********************************************************** CLK PORTA enable*************************************************************************/
	MDR_RST_CLK -> PER_CLOCK |= ( ENABLE_BIT << PORTA_CLK_BIT_POS	);
		
	/******************************************************** PORTA Pin 5 LED configuration *************************************************************/	
	// Enable PORTA Pin 5 output mode
	MDR_UNION_PORTA -> OE_Bit.Pin_5 						= ENABLE_BIT;
	
	// Enable PORTA Pin 5 port mode 
	MDR_UNION_PORTA -> FUNC_Bit.Pin_5 					= DISABLE_DOUBLE_BIT;
	
	// Enable PORTA Pin 5 digital mode
	MDR_UNION_PORTA -> ANALOG_Bit.Pin_5 				= ENABLE_BIT;
	
	// Disable PORTA Pin 5 pull-up and pull-down
	MDR_UNION_PORTA -> PULL_Bit.PULL_DOWN_Pin_5 = DISABLE_BIT;
	MDR_UNION_PORTA -> PULL_Bit.PULL_UP_Pin_5 	= DISABLE_BIT;
	
	// Disable PORTA Pin 5 open-drain mode and Shmidt trigger
	MDR_UNION_PORTA -> PD_Bit.DriverMode_Pin_5 	= DISABLE_BIT;
	MDR_UNION_PORTA -> PD_Bit.SHM_Pin_5 				= DISABLE_BIT;
	
	// Enable PORTA Pin 5 full speed mode
	MDR_UNION_PORTA -> PWR_Bit.Pin_5 						= ENABLE_DOUBLE_BIT;
	
	// Disable PORTA Pin 5 input filter
	MDR_UNION_PORTA -> GFEN_Bit.Pin_5 					= DISABLE_BIT;
}

void GPIO_Button_Init( void )
{
	/********************************************************** CLK PORTA enable*************************************************************************/
	MDR_RST_CLK -> PER_CLOCK |= ( ENABLE_BIT << PORTA_CLK_BIT_POS	);
	
	/******************************************************** PORTA Pin 0 Button configuration ************************************************************/
	// Enable PORTA Pin 0 output mode
	MDR_UNION_PORTA -> OE_Bit.Pin_0 						= DISABLE_BIT;
	
	// Enable PORTA Pin 0 port mode 
	MDR_UNION_PORTA -> FUNC_Bit.Pin_0 					= DISABLE_DOUBLE_BIT;
	
	// Enable PORTA Pin 0 digital mode
	MDR_UNION_PORTA -> ANALOG_Bit.Pin_0 				= ENABLE_BIT;
	
	// Disable PORTA Pin 0 pull-up and pull-down
	MDR_UNION_PORTA -> PULL_Bit.PULL_DOWN_Pin_0 = DISABLE_BIT;
	MDR_UNION_PORTA -> PULL_Bit.PULL_UP_Pin_0 	= DISABLE_BIT;
	
	// Disable PORTA Pin 0 open-drain mode and Shmidt trigger
	MDR_UNION_PORTA -> PD_Bit.DriverMode_Pin_0 	= DISABLE_BIT;
	MDR_UNION_PORTA -> PD_Bit.SHM_Pin_0 				= DISABLE_BIT;
	
	// Enable PORTA Pin 0 full speed mode
	MDR_UNION_PORTA -> PWR_Bit.Pin_0 						= ENABLE_DOUBLE_BIT;
	
	// Disable PORTA Pin 0 input filter
	MDR_UNION_PORTA -> GFEN_Bit.Pin_0 					= DISABLE_BIT;
}


void Previous_code_from_while( void )
{
	#ifdef BUTTON_PRESSED_INVERT_BIT
	// Invert bit if button pressed 
	if(MDR_UNION_PORTA ->RXTX_Bit.Pin_0 == DISABLE_BIT )
	{
		// Invert bit
		MDR_UNION_PORTA -> RXTX_Bit.Pin_5 ^= 1;
		
		// Wait contact bounce time
		Delay();
	}
	#endif
	
	#ifdef BUTTON_CONTINIOUSLY_PRESSED_BLINKING
		// If button pressed - blink
		if( MDR_UNION_PORTA -> RXTX_Bit.Pin_0 == DISABLE_BIT )
		{
			// Enable light
			MDR_UNION_PORTA -> RXTX_Bit.Pin_5 = ENABLE_BIT;
			Delay();
			
			// Disable light
			MDR_UNION_PORTA -> RXTX_Bit.Pin_5 = DISABLE_BIT;
			Delay();
		}
	#endif
}
