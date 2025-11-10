/**
  * @file    seg7_display.h
  * @brief   7-Segment Display Controller for MDR32F9Q2I
  * @version 1.0
  *
  * This module provides control for 4-digit 7-segment displays
  * with multiplexing support and efficient refresh management.
  */

#ifndef SEG7_DISPLAY_H
#define SEG7_DISPLAY_H

#include <stdint.h>
#include <stdbool.h>
#include "leds.h"
#include "MDR32F9Q2I.h"
#include "MDR32FxQI_rst_clk.h"
#include "MDR32FxQI_port.h"
#include "hardware_drivers.h"
#include "bit_utils.h"

/* Display configuration */
#define SEG7_NUM_DIGITS          4
#define SEG7_REFRESH_RATE_HZ     250     /* 25Hz refresh for flicker-free display */
#define SEG7_DEFAULT_BRIGHTNESS  100     /* Default brightness (0-100) */

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

/* Display modes */
typedef enum {
    SEG7_MODE_NORMAL = 0,    /* Normal display operation */
    SEG7_MODE_BLINK,         /* Blink entire display */
    SEG7_MODE_SCROLL,        /* Scroll text (future feature) */
    SEG7_MODE_OFF            /* Display off */
} SEG7_ModeTypeDef;

/* Display control structure */
typedef struct {
    uint8_t digits[SEG7_NUM_DIGITS];     /* Current digit values (0-9) */
    bool decimal_points[SEG7_NUM_DIGITS]; /* Decimal point states */
    uint8_t current_digit;               /* Currently active digit for multiplexing */
    uint32_t last_refresh_time;          /* Last refresh timestamp */
    uint32_t refresh_interval;           /* Refresh interval in ticks */
    uint8_t brightness;                  /* Brightness level (0-100) */
    SEG7_ModeTypeDef mode;               /* Current display mode */
    bool blink_state;                    /* Current blink state */
    uint32_t blink_interval;             /* Blink interval in ticks */
    uint32_t last_blink_time;            /* Last blink toggle timestamp */
} SEG7_DisplayTypeDef;

/* Function prototypes */
void SEG7_Init(void);
void SEG7_SetNumber(uint16_t number);
void SEG7_SetFloat(float number, uint8_t decimal_places);
void SEG7_SetDigit(uint8_t digit_index, uint8_t value, bool decimal_point);
void SEG7_SetBrightness(uint8_t brightness);
void SEG7_SetMode(SEG7_ModeTypeDef mode);
void SEG7_Clear(void);
void SEG7_Process(void);
void SEG7_TestPattern(void);

/* Internal functions */
static void SEG7_GPIO_Init(void);
static void SEG7_WriteDigit(uint8_t digit, uint8_t value, bool decimal_point);
static void SEG7_SelectDigit(uint8_t digit_index);
static void SEG7_AllDigitsOff(void);
static uint8_t SEG7_GetSegmentPattern(uint8_t digit);

#endif /* SEG7_DISPLAY_H */