#include "main.h"
#include "leds.h"
#include "hardware_drivers.h"
#include "seg7_display.h"

/* Global variables */
static uint16_t display_value = 1488;
static uint32_t last_update_time = 0;
static bool demo_mode = true;
static uint32_t demo_counter = 0;

/**
  * @brief  Main program
  * @param  None
  * @retval int
  */
int main(void) {
    /* Initialize system and peripherals */
    HD_System_Init();
    LED_Init();
    SEG7_Init();
    
    /* Start PWM wave for LEDs */
    LED_StartPWMWave();
    LED_SetWaveSpeed(1);    // Medium speed
    LED_SetPWMPeriod(1500); // Wave period
    
    /* Configure 7-segment display */
    SEG7_SetBrightness(80); // 80% brightness
    SEG7_SetNumber(display_value);
    
    /* Main program loop */
    while(1) {
        uint32_t current_time = HD_GetTick();
        
        /* Process LED effects in timer interrupt */
        // LED_Process() is called from TIMER1 interrupt
        
        /* Process 7-segment display multiplexing */
        SEG7_Process();
        
        /* Demo mode: cycle through different displays */
        if (demo_mode && (current_time - last_update_time) >= 100) {
            last_update_time = current_time;
            demo_counter++;
            
            switch (demo_counter % 6) {
                case 0:
                    SEG7_SetNumber(1488);
                    break;
                case 1:
                    SEG7_SetFloat(3.141f, 3); // Show Pi
                    break;
                case 2:
                    SEG7_SetNumber(8888); // All segments test
                    break;
                case 3:
                    SEG7_SetFloat(12.34f, 2); // Show decimal
                    break;
                case 4:
                    SEG7_TestPattern(); // Running pattern
                    break;
                case 5:
                    SEG7_SetMode(SEG7_MODE_BLINK); // Blink mode
                    HD_Delay_ms_blocking(2000);
                    SEG7_SetMode(SEG7_MODE_NORMAL);
                    break;
            }
        }
        
        /* Optional: Add small delay to reduce CPU usage */
        // HD_Delay_ms_blocking(1);
    }
}