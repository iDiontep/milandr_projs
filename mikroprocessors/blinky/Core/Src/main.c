#include "main.h"
#include "leds.h"
#include "hardware_drivers.h"
#include "app.h"

button_data user_button;

int main(void) {
    /* Initialize application */
    app_init();
    
    /* Start application timers */
    app_start(); // ĞÀÑÊÎÌÌÅÍÒÈĞÎÂÀÒÜ ıòó ñòğîêó

    /* Main loop - everything is handled in interrupts */
    while(1) {
        /* All processing is done in app_process() called from timer interrupt */
        /* You can add low-priority tasks here if needed */
        __NOP();
    }
}