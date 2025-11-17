// seg7_display.h
#ifndef SEG7_DISPLAY_H
#define SEG7_DISPLAY_H

#include "MDR32F9Q2I.h"
#include "MDR32FxQI_port.h"

void SEG7_Init(void);
void SEG7_Process(void);
void Timer_Init(void);
void Timer_Process(void);
void Timer_StartStop(void);
void Timer_Reset(void);
void Display_Update(void);

void SEG7_DisplayTest(void);
void SEG7_DisplayNumber(uint16_t number);


#endif