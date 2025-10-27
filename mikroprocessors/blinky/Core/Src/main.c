#include "main.h"
#include "leds.h"
#include "hardware_drivers.h"

int main(void) {
    /* Initialize system and LEDs */
    HD_System_Init();
    LED_Init();
    
    /* Start PWM wave */
    LED_StartPWMWave();
    LED_SetWaveSpeed(1);    // Medium speed
    LED_SetPWMPeriod(1500);  // Wave period
    
    /* Alternative: Start LED sequence */
    // LED_Sequence(200);  // 200ms per LED

    /* Main loop - now empty since everything works on interrupts */
    while(1) {
        // All LED processing is done in timer interrupts
        // You can add other non-time-critical tasks here
        __NOP();
				//LED_Process();
        // Or add small delay to reduce CPU usage
        // HD_Delay_ms_blocking(1);
    }
}