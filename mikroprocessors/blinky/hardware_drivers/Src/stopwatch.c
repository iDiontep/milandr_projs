/**
  * @file    stopwatch.c
  * @brief   Stopwatch module implementation
  * @version 1.0
  */

#include "stopwatch.h"
#include "hardware_drivers.h"
#include <string.h>

/* Global stopwatch instance */
static Stopwatch_TypeDef stopwatch;

/* Display modes */
#define DISPLAY_NORMAL      0
#define DISPLAY_LAP         1
#define DISPLAY_MILLIS      2

/**
  * @brief  Initialize stopwatch module
  */
void Stopwatch_Init(void)
{
    memset(&stopwatch, 0, sizeof(stopwatch));
    stopwatch.mode = STOPWATCH_RESET;
    stopwatch.last_update = HD_GetTick();
    stopwatch.display_blink = false;
    stopwatch.lap_count = 0;
    stopwatch.show_milliseconds = false;
    
    /* Initialize display */
    SEG7_Init();
    SEG7_SetBrightness(80);
    Stopwatch_UpdateDisplay();
}

/**
  * @brief  Start the stopwatch
  */
void Stopwatch_Start(void)
{
    if (stopwatch.mode == STOPWATCH_RESET) {
        /* Reset time when starting from reset state */
        memset(&stopwatch.current_time, 0, sizeof(stopwatch.current_time));
        memset(&stopwatch.lap_time, 0, sizeof(stopwatch.lap_time));
    }
    
    stopwatch.mode = STOPWATCH_RUNNING;
    stopwatch.last_update = HD_GetTick();
    stopwatch.display_blink = false;
    SEG7_SetMode(SEG7_MODE_NORMAL);
}

/**
  * @brief  Stop/pause the stopwatch
  */
void Stopwatch_Stop(void)
{
    if (stopwatch.mode == STOPWATCH_RUNNING) {
        stopwatch.mode = STOPWATCH_PAUSED;
        stopwatch.display_blink = true;
        stopwatch.last_blink = HD_GetTick();
    }
}

/**
  * @brief  Reset the stopwatch
  */
void Stopwatch_Reset(void)
{
    stopwatch.mode = STOPWATCH_RESET;
    memset(&stopwatch.current_time, 0, sizeof(stopwatch.current_time));
    memset(&stopwatch.lap_time, 0, sizeof(stopwatch.lap_time));
    stopwatch.lap_count = 0;
    stopwatch.display_blink = false;
    stopwatch.show_milliseconds = false;
    SEG7_SetMode(SEG7_MODE_NORMAL);
    Stopwatch_UpdateDisplay();
}

/**
  * @brief  Record lap time
  */
void Stopwatch_Lap(void)
{
    if (stopwatch.mode == STOPWATCH_RUNNING) {
        /* Save current time as lap time */
        memcpy(&stopwatch.lap_time, &stopwatch.current_time, sizeof(stopwatch.current_time));
        stopwatch.lap_count++;
        stopwatch.mode = STOPWATCH_LAP;
        
        /* Show lap time for 3 seconds */
        Stopwatch_UpdateDisplay();
    } else if (stopwatch.mode == STOPWATCH_LAP) {
        /* Return to normal display */
        stopwatch.mode = STOPWATCH_RUNNING;
    }
}

/**
  * @brief  Toggle milliseconds display
  */
void Stopwatch_ToggleMilliseconds(void)
{
    stopwatch.show_milliseconds = !stopwatch.show_milliseconds;
    Stopwatch_UpdateDisplay();
}

/**
  * @brief  Main stopwatch processing function - call periodically
  */
void Stopwatch_Process(void)
{
    uint32_t current_time = HD_GetTick();
    
    /* Handle time update when running */
    if (stopwatch.mode == STOPWATCH_RUNNING || stopwatch.mode == STOPWATCH_LAP) {
        if ((current_time - stopwatch.last_update) >= 10) { /* Update every 10ms */
            stopwatch.last_update = current_time;
            
            /* Update milliseconds */
            stopwatch.current_time.milliseconds += 10;
            
            /* Handle time rollover */
            if (stopwatch.current_time.milliseconds >= 1000) {
                stopwatch.current_time.milliseconds = 0;
                stopwatch.current_time.seconds++;
                
                if (stopwatch.current_time.seconds >= 60) {
                    stopwatch.current_time.seconds = 0;
                    stopwatch.current_time.minutes++;
                    
                    if (stopwatch.current_time.minutes >= 60) {
                        stopwatch.current_time.minutes = 0;
                        stopwatch.current_time.hours++;
                        
                        /* Stop at 99:59:59.999 */
                        if (stopwatch.current_time.hours >= 100) {
                            stopwatch.current_time.hours = 99;
                            stopwatch.current_time.minutes = 59;
                            stopwatch.current_time.seconds = 59;
                            stopwatch.current_time.milliseconds = 999;
                            stopwatch.mode = STOPWATCH_PAUSED;
                        }
                    }
                }
            }
            
            /* Update display */
            Stopwatch_UpdateDisplay();
        }
    }
    
    /* Handle blinking in paused mode */
    if (stopwatch.mode == STOPWATCH_PAUSED && stopwatch.display_blink) {
        if ((current_time - stopwatch.last_blink) >= 500) { /* 500ms blink interval */
            stopwatch.last_blink = current_time;
            static bool blink_state = false;
            blink_state = !blink_state;
            
            if (blink_state) {
                SEG7_SetMode(SEG7_MODE_OFF);
            } else {
                SEG7_SetMode(SEG7_MODE_NORMAL);
                Stopwatch_UpdateDisplay();
            }
        }
    }
    
    /* Process 7-segment display */
    SEG7_Process();
}

/**
  * @brief  Update 7-segment display with current time
  */
void Stopwatch_UpdateDisplay(void)
{
    uint8_t digits[4] = {0};
    bool decimal_points[4] = {false};
    
    if (stopwatch.mode == STOPWATCH_LAP) {
        /* Display lap time */
        if (stopwatch.show_milliseconds) {
            /* Display MM.SS or M.SS.m based on available digits */
            if (stopwatch.lap_time.minutes >= 10) {
                digits[0] = stopwatch.lap_time.minutes / 10;
                digits[1] = stopwatch.lap_time.minutes % 10;
                decimal_points[1] = true;
                digits[2] = stopwatch.lap_time.seconds / 10;
                digits[3] = stopwatch.lap_time.seconds % 10;
            } else {
                digits[0] = stopwatch.lap_time.minutes;
                decimal_points[0] = true;
                digits[1] = stopwatch.lap_time.seconds / 10;
                digits[2] = stopwatch.lap_time.seconds % 10;
                digits[3] = stopwatch.lap_time.milliseconds / 100;
            }
        } else {
            /* Display minutes and seconds */
            if (stopwatch.lap_time.minutes >= 10) {
                digits[0] = stopwatch.lap_time.minutes / 10;
                digits[1] = stopwatch.lap_time.minutes % 10;
            } else {
                digits[1] = stopwatch.lap_time.minutes;
            }
            digits[2] = stopwatch.lap_time.seconds / 10;
            digits[3] = stopwatch.lap_time.seconds % 10;
            decimal_points[1] = true; /* Colon between minutes and seconds */
        }
    } else {
        /* Display current time */
        if (stopwatch.show_milliseconds) {
            /* Display with milliseconds */
            if (stopwatch.current_time.minutes > 0) {
                if (stopwatch.current_time.minutes >= 10) {
                    digits[0] = stopwatch.current_time.minutes / 10;
                    digits[1] = stopwatch.current_time.minutes % 10;
                    decimal_points[1] = true;
                    digits[2] = stopwatch.current_time.seconds / 10;
                    digits[3] = stopwatch.current_time.seconds % 10;
                } else {
                    digits[0] = stopwatch.current_time.minutes;
                    decimal_points[0] = true;
                    digits[1] = stopwatch.current_time.seconds / 10;
                    digits[2] = stopwatch.current_time.seconds % 10;
                    digits[3] = stopwatch.current_time.milliseconds / 100;
                }
            } else {
                /* Display seconds and milliseconds */
                digits[0] = stopwatch.current_time.seconds / 10;
                digits[1] = stopwatch.current_time.seconds % 10;
                decimal_points[1] = true;
                digits[2] = stopwatch.current_time.milliseconds / 100;
                digits[3] = (stopwatch.current_time.milliseconds % 100) / 10;
            }
        } else {
            /* Display minutes and seconds */
            if (stopwatch.current_time.hours > 0) {
                /* Display hours and minutes if hours > 0 */
                if (stopwatch.current_time.hours >= 10) {
                    digits[0] = stopwatch.current_time.hours / 10;
                    digits[1] = stopwatch.current_time.hours % 10;
                } else {
                    digits[1] = stopwatch.current_time.hours;
                }
                digits[2] = stopwatch.current_time.minutes / 10;
                digits[3] = stopwatch.current_time.minutes % 10;
                decimal_points[1] = true; /* Colon */
            } else {
                /* Display minutes and seconds */
                if (stopwatch.current_time.minutes >= 10) {
                    digits[0] = stopwatch.current_time.minutes / 10;
                    digits[1] = stopwatch.current_time.minutes % 10;
                } else {
                    digits[1] = stopwatch.current_time.minutes;
                }
                digits[2] = stopwatch.current_time.seconds / 10;
                digits[3] = stopwatch.current_time.seconds % 10;
                decimal_points[1] = true; /* Colon between minutes and seconds */
            }
        }
    }
    
    /* Update display digits */
    for (uint8_t i = 0; i < 4; i++) {
        SEG7_SetDigit(i, digits[i], decimal_points[i]);
    }
}

/**
  * @brief  Get current stopwatch mode
  */
Stopwatch_ModeTypeDef Stopwatch_GetMode(void)
{
    return stopwatch.mode;
}

/**
  * @brief  Get current time
  */
Stopwatch_TimeTypeDef Stopwatch_GetCurrentTime(void)
{
    return stopwatch.current_time;
}

/**
  * @brief  Get lap time
  */
Stopwatch_TimeTypeDef Stopwatch_GetLapTime(void)
{
    return stopwatch.lap_time;
}