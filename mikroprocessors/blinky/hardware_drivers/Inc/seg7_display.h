// seg7_display.h
#ifndef SEG7_DISPLAY_H
#define SEG7_DISPLAY_H

#include "MDR32F9Q2I.h"
#include "MDR32FxQI_port.h"

void SEG7_Init(void);
void SEG7_Process(void);
void app_Init(void);
void Timer_Process(void);
void Timer_StartStop(void);
void Timer_Reset(void);
void Display_Update(void);

void SEG7_DisplayTest(void);
void SEG7_DisplayNumber(uint16_t number);


/* Segment pin definitions - Common Cathode configuration */
/* Segment A-G mapping to GPIO pins */
#define SEG7_SEG_A_PORT      MDR_PORTA
#define SEG7_SEG_A_PIN       PORT_Pin_7
#define SEG7_SEG_A_MASK      (1UL << 7)

#define SEG7_SEG_B_PORT      MDR_PORTA  
#define SEG7_SEG_B_PIN       PORT_Pin_4
#define SEG7_SEG_B_MASK      (1UL << 4)

#define SEG7_SEG_C_PORT      MDR_PORTA
#define SEG7_SEG_C_PIN       PORT_Pin_5
#define SEG7_SEG_C_MASK      (1UL << 5)

#define SEG7_SEG_D_PORT      MDR_PORTB
#define SEG7_SEG_D_PIN       PORT_Pin_0
#define SEG7_SEG_D_MASK      (1UL << 0)

#define SEG7_SEG_E_PORT      MDR_PORTB
#define SEG7_SEG_E_PIN       PORT_Pin_1
#define SEG7_SEG_E_MASK      (1UL << 1)

#define SEG7_SEG_F_PORT      MDR_PORTB
#define SEG7_SEG_F_PIN       PORT_Pin_2
#define SEG7_SEG_F_MASK      (1UL << 2)

#define SEG7_SEG_G_PORT      MDR_PORTB
#define SEG7_SEG_G_PIN       PORT_Pin_5
#define SEG7_SEG_G_MASK      (1UL << 5)

#define SEG7_SEG_DP_PORT     MDR_PORTB
#define SEG7_SEG_DP_PIN      PORT_Pin_6
#define SEG7_SEG_DP_MASK     (1UL << 6)

/* Digit selection pins (Common Cathode - active low with transistor drivers) */
#define SEG7_DIGIT_1_PORT    MDR_PORTC
#define SEG7_DIGIT_1_PIN     PORT_Pin_0
#define SEG7_DIGIT_1_MASK    (1UL << 0)

#define SEG7_DIGIT_2_PORT    MDR_PORTC
#define SEG7_DIGIT_2_PIN     PORT_Pin_1
#define SEG7_DIGIT_2_MASK    (1UL << 1)

#define SEG7_DIGIT_3_PORT    MDR_PORTE
#define SEG7_DIGIT_3_PIN     PORT_Pin_0
#define SEG7_DIGIT_3_MASK    (1UL << 0)

#define SEG7_DIGIT_4_PORT    MDR_PORTF
#define SEG7_DIGIT_4_PIN     PORT_Pin_1
#define SEG7_DIGIT_4_MASK    (1UL << 1)


#endif