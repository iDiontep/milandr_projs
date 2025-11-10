/**
  * @file    stopwatch.h
  * @brief   Stopwatch module for MDR32F9Q2I
  * @version 1.0
  */

#ifndef STOPWATCH_H
#define STOPWATCH_H

#include <stdint.h>
#include <stdbool.h>
#include "seg7_display.h"

/* Stopwatch modes */
typedef enum {
    STOPWATCH_RESET = 0,    /* Reset state */
    STOPWATCH_RUNNING,      /* Running */
    STOPWATCH_PAUSED,       /* Paused */
    STOPWATCH_LAP           /* Lap mode */
} Stopwatch_ModeTypeDef;

/* Stopwatch time structure */
typedef struct {
    uint8_t hours;
    uint8_t minutes;
    uint8_t seconds;
    uint16_t milliseconds;
} Stopwatch_TimeTypeDef;

/* Stopwatch control structure */
typedef struct {
    Stopwatch_ModeTypeDef mode;
    Stopwatch_TimeTypeDef current_time;
    Stopwatch_TimeTypeDef lap_time;
    uint32_t last_update;
    bool display_blink;
    uint32_t last_blink;
    uint8_t lap_count;
    bool show_milliseconds;
} Stopwatch_TypeDef;

/* Function prototypes */
void Stopwatch_Init(void);
void Stopwatch_Start(void);
void Stopwatch_Stop(void);
void Stopwatch_Reset(void);
void Stopwatch_Lap(void);
void Stopwatch_ToggleMilliseconds(void);
void Stopwatch_Process(void);
void Stopwatch_UpdateDisplay(void);
Stopwatch_ModeTypeDef Stopwatch_GetMode(void);
Stopwatch_TimeTypeDef Stopwatch_GetCurrentTime(void);
Stopwatch_TimeTypeDef Stopwatch_GetLapTime(void);

#endif /* STOPWATCH_H */