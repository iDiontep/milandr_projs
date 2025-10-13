/* USER CODE BEGIN Header */

/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/


/* USER CODE BEGIN Includes */
#include "main.h"
#include "leds.h"

/* USER CODE END Includes */

int main(void)
{
    /* Initialize system and LEDs */
    HD_System_Init();
    LED_Init();
    
    /* Start LED sequence */
    LED_Sequence(500);
    
    /* Main loop - now empty since everything is handled by interrupts */
    while(1)
    {
        /* CPU can enter low power mode here if needed */
        /* All LED processing is done in Timer1 interrupt */
    }
}
