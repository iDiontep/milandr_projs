#include "main.h"
#include "button.h"
/**
  * @brief  Main program - Stopwatch application
  */
int main(void)
{
    /* Initialize system and peripherals */
    HD_System_Init();
    LED_Init();
    Buttons_Init();
    Stopwatch_Init();
    
    /* Start LED effects */
    LED_StartPWMWave();
    LED_SetWaveSpeed(1);
    LED_SetPWMPeriod(1500);
    
    /* Main program loop */
    while(1) {
        /* Process button states */
        Buttons_Process();
        
        /* Handle button events */
        HandleButtonEvents();
        
        /* Process stopwatch */
        Stopwatch_Process();
        
        /* Process LED effects (in interrupt) */
        // LED_Process() is called from TIMER1 interrupt
        
        /* Optional: Reduce CPU usage */
        // HD_Delay_ms_blocking(1);
    }
}

/**
  * @brief  Handle button events for stopwatch control
  */
static void HandleButtonEvents(void)
{
    /* Start/Stop button */
    Button_EventTypeDef start_stop_event = Buttons_GetStartStopEvent();
    if (start_stop_event == BUTTON_EVENT_PRESS) {
        Stopwatch_ModeTypeDef mode = Stopwatch_GetMode();
        if (mode == STOPWATCH_RUNNING || mode == STOPWATCH_LAP) {
            Stopwatch_Stop();
        } else {
            Stopwatch_Start();
        }
    }
    
    /* Lap/Reset button */
    Button_EventTypeDef lap_reset_event = Buttons_GetLapResetEvent();
    if (lap_reset_event == BUTTON_EVENT_PRESS) {
        Stopwatch_ModeTypeDef mode = Stopwatch_GetMode();
        if (mode == STOPWATCH_RUNNING) {
            Stopwatch_Lap();
        } else if (mode == STOPWATCH_LAP) {
            Stopwatch_ModeTypeDef current_mode = Stopwatch_GetMode();
            if (current_mode != STOPWATCH_RUNNING) {
                Stopwatch_Lap(); /* Return to running mode */
            }
        }
    } else if (lap_reset_event == BUTTON_EVENT_HOLD) {
        /* Long press for reset */
        Stopwatch_Reset();
    }
    
    /* Mode button */
    Button_EventTypeDef mode_event = Buttons_GetModeEvent();
    if (mode_event == BUTTON_EVENT_PRESS) {
        Stopwatch_ToggleMilliseconds();
    }
}