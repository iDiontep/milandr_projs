/**
  * @file    seg7_display.c
  * @brief   7-Segment Display Controller Implementation
  * @version 1.0
  */

#include "seg7_display.h"
#include <string.h>
#include "leds.h"
#include "hardware_drivers.h"

/* Global display structure */
static SEG7_DisplayTypeDef seg7_display;

/* Segment patterns for digits 0-9 (Common Cathode) */
static const uint8_t seg7_digit_patterns[10] = {
    0b0111111, /* 0 */
    0b0000110, /* 1 */
    0b1011011, /* 2 */
    0b1001111, /* 3 */
    0b1100110, /* 4 */
    0b1101101, /* 5 */
    0b1111101, /* 6 */
    0b0000111, /* 7 */
    0b1111111, /* 8 */
    0b1101111  /* 9 */
};

/**
  * @brief  Initialize 7-segment display module
  */
void SEG7_Init(void)
{
    /* Initialize GPIO pins */
    SEG7_GPIO_Init();
    
    /* Initialize display structure */
    memset(&seg7_display, 0, sizeof(seg7_display));
    
    /* Set default values */
    seg7_display.refresh_interval = 1000 / (SEG7_REFRESH_RATE_HZ * SEG7_NUM_DIGITS);
    seg7_display.brightness = SEG7_DEFAULT_BRIGHTNESS;
    seg7_display.mode = SEG7_MODE_NORMAL;
    seg7_display.blink_interval = 500;
    seg7_display.blink_state = true;
    
    /* Clear display */
    SEG7_Clear();
    
    /* Set initial refresh time */
    seg7_display.last_refresh_time = HD_GetTick();
    seg7_display.last_blink_time = HD_GetTick();
}

/**
  * @brief  Initialize GPIO pins for 7-segment display
  */
static void SEG7_GPIO_Init(void)
{
    PORT_InitTypeDef port_init;
    
    /* Enable clock for all used ports */
    RST_CLK_PCLKcmd(RST_CLK_PCLK_PORTA, ENABLE);
    RST_CLK_PCLKcmd(RST_CLK_PCLK_PORTB, ENABLE);
    RST_CLK_PCLKcmd(RST_CLK_PCLK_PORTC, ENABLE);
    RST_CLK_PCLKcmd(RST_CLK_PCLK_PORTE, ENABLE);
    RST_CLK_PCLKcmd(RST_CLK_PCLK_PORTF, ENABLE);

    /* Configure segment pins as outputs */
    PORT_StructInit(&port_init);
    port_init.PORT_Pin = SEG7_SEG_A_PIN | SEG7_SEG_B_PIN | SEG7_SEG_C_PIN;
    port_init.PORT_OE = PORT_OE_OUT;
    port_init.PORT_FUNC = PORT_FUNC_PORT;
    port_init.PORT_MODE = PORT_MODE_DIGITAL;
    port_init.PORT_SPEED = PORT_SPEED_FAST;
    port_init.PORT_PD = PORT_PD_DRIVER;
    PORT_Init(SEG7_SEG_A_PORT, &port_init);

    PORT_StructInit(&port_init);
    port_init.PORT_Pin = SEG7_SEG_D_PIN | SEG7_SEG_E_PIN | SEG7_SEG_F_PIN | 
                         SEG7_SEG_G_PIN | SEG7_SEG_DP_PIN;
    port_init.PORT_OE = PORT_OE_OUT;
    port_init.PORT_FUNC = PORT_FUNC_PORT;
    port_init.PORT_MODE = PORT_MODE_DIGITAL;
    port_init.PORT_SPEED = PORT_SPEED_FAST;
    port_init.PORT_PD = PORT_PD_DRIVER;
    PORT_Init(SEG7_SEG_D_PORT, &port_init);

    /* Configure digit selection pins as outputs */
    PORT_StructInit(&port_init);
    port_init.PORT_Pin = SEG7_DIGIT_1_PIN | SEG7_DIGIT_2_PIN;
    port_init.PORT_OE = PORT_OE_OUT;
    port_init.PORT_FUNC = PORT_FUNC_PORT;
    port_init.PORT_MODE = PORT_MODE_DIGITAL;
    port_init.PORT_SPEED = PORT_SPEED_FAST;
    port_init.PORT_PD = PORT_PD_DRIVER;
    PORT_Init(SEG7_DIGIT_1_PORT, &port_init);

    PORT_StructInit(&port_init);
    port_init.PORT_Pin = SEG7_DIGIT_3_PIN;
    port_init.PORT_OE = PORT_OE_OUT;
    port_init.PORT_FUNC = PORT_FUNC_PORT;
    port_init.PORT_MODE = PORT_MODE_DIGITAL;
    port_init.PORT_SPEED = PORT_SPEED_FAST;
    port_init.PORT_PD = PORT_PD_DRIVER;
    PORT_Init(SEG7_DIGIT_3_PORT, &port_init);

    PORT_StructInit(&port_init);
    port_init.PORT_Pin = SEG7_DIGIT_4_PIN;
    port_init.PORT_OE = PORT_OE_OUT;
    port_init.PORT_FUNC = PORT_FUNC_PORT;
    port_init.PORT_MODE = PORT_MODE_DIGITAL;
    port_init.PORT_SPEED = PORT_SPEED_FAST;
    port_init.PORT_PD = PORT_PD_DRIVER;
    PORT_Init(SEG7_DIGIT_4_PORT, &port_init);
}

/**
  * @brief  Display a number on the 7-segment display
  */
void SEG7_SetNumber(uint16_t number)
{
    uint8_t digits[SEG7_NUM_DIGITS];
    
    /* Extract individual digits */
    digits[0] = number / 1000;
    digits[1] = (number % 1000) / 100;
    digits[2] = (number % 100) / 10;
    digits[3] = number % 10;
    
    /* Update display digits */
    for (uint8_t i = 0; i < SEG7_NUM_DIGITS; i++) {
        seg7_display.digits[i] = digits[i];
        seg7_display.decimal_points[i] = false;
    }
}

/**
  * @brief  Display a floating point number
  */
void SEG7_SetFloat(float number, uint8_t decimal_places)
{
    uint16_t integer_part;
    uint16_t fractional_part;
    uint8_t digits[SEG7_NUM_DIGITS];
    
    /* Clamp decimal places */
    if (decimal_places > 3) {
        decimal_places = 3;
    }
    
    /* Scale the number based on decimal places */
    float scale_factor = 1.0f;
    for (uint8_t i = 0; i < decimal_places; i++) {
        scale_factor *= 10.0f;
    }
    
    uint16_t scaled_number = (uint16_t)(number * scale_factor + 0.5f);
    
    /* Extract digits */
    for (int8_t i = SEG7_NUM_DIGITS - 1; i >= 0; i--) {
        digits[i] = scaled_number % 10;
        scaled_number /= 10;
    }
    
    /* Update display digits and decimal points */
    for (uint8_t i = 0; i < SEG7_NUM_DIGITS; i++) {
        seg7_display.digits[i] = digits[i];
        seg7_display.decimal_points[i] = (i == (SEG7_NUM_DIGITS - 1 - decimal_places));
    }
}

/**
  * @brief  Set individual digit value and decimal point
  */
void SEG7_SetDigit(uint8_t digit_index, uint8_t value, bool decimal_point)
{
    if (digit_index < SEG7_NUM_DIGITS && value <= 9) {
        seg7_display.digits[digit_index] = value;
        seg7_display.decimal_points[digit_index] = decimal_point;
    }
}

/**
  * @brief  Set display brightness
  */
void SEG7_SetBrightness(uint8_t brightness)
{
    if (brightness > 100) {
        brightness = 100;
    }
    seg7_display.brightness = brightness;
    
    /* Adjust refresh interval based on brightness */
    uint32_t base_interval = 1000 / (SEG7_REFRESH_RATE_HZ * SEG7_NUM_DIGITS);
    seg7_display.refresh_interval = (base_interval * brightness) / 100;
}

/**
  * @brief  Set display mode
  */
void SEG7_SetMode(SEG7_ModeTypeDef mode)
{
    seg7_display.mode = mode;
    seg7_display.blink_state = true;
    seg7_display.last_blink_time = HD_GetTick();
}

/**
  * @brief  Clear all digits on the display
  */
void SEG7_Clear(void)
{
    for (uint8_t i = 0; i < SEG7_NUM_DIGITS; i++) {
        seg7_display.digits[i] = 0;
        seg7_display.decimal_points[i] = false;
    }
}

/**
  * @brief  Main display processing function - call periodically
  */
void SEG7_Process(void)
{
    uint32_t current_time = HD_GetTick();
    
    /* Handle blinking mode */
    if (seg7_display.mode == SEG7_MODE_BLINK) {
        if ((current_time - seg7_display.last_blink_time) >= seg7_display.blink_interval) {
            seg7_display.blink_state = !seg7_display.blink_state;
            seg7_display.last_blink_time = current_time;
        }
        
        /* If blinking off state, turn off all digits */
        if (!seg7_display.blink_state) {
            SEG7_AllDigitsOff();
            return;
        }
    }
    
    /* Handle display off mode */
    if (seg7_display.mode == SEG7_MODE_OFF) {
        SEG7_AllDigitsOff();
        return;
    }
    
    /* Check if it's time to refresh the display */
    if ((current_time - seg7_display.last_refresh_time) >= seg7_display.refresh_interval) {
        seg7_display.last_refresh_time = current_time;
        
        /* Turn off all digits first to prevent ghosting */
        SEG7_AllDigitsOff();
        
        /* Write segment pattern for current digit */
        SEG7_WriteDigit(seg7_display.current_digit, 
                       seg7_display.digits[seg7_display.current_digit],
                       seg7_display.decimal_points[seg7_display.current_digit]);
        
        /* Select current digit */
        SEG7_SelectDigit(seg7_display.current_digit);
        
        /* Move to next digit */
        seg7_display.current_digit = (seg7_display.current_digit + 1) % SEG7_NUM_DIGITS;
    }
}

/**
  * @brief  Write segment pattern for a specific digit
  */
static void SEG7_WriteDigit(uint8_t digit, uint8_t value, bool decimal_point)
{
    uint8_t pattern = SEG7_GetSegmentPattern(digit);
    
    /* Write segment pattern using PORT driver functions */
    (pattern & 0x40) ? PORT_SetBits(SEG7_SEG_A_PORT, SEG7_SEG_A_PIN) : 
                       PORT_ResetBits(SEG7_SEG_A_PORT, SEG7_SEG_A_PIN);
    
    (pattern & 0x20) ? PORT_SetBits(SEG7_SEG_B_PORT, SEG7_SEG_B_PIN) : 
                       PORT_ResetBits(SEG7_SEG_B_PORT, SEG7_SEG_B_PIN);
    
    (pattern & 0x10) ? PORT_SetBits(SEG7_SEG_C_PORT, SEG7_SEG_C_PIN) : 
                       PORT_ResetBits(SEG7_SEG_C_PORT, SEG7_SEG_C_PIN);
    
    (pattern & 0x08) ? PORT_SetBits(SEG7_SEG_D_PORT, SEG7_SEG_D_PIN) : 
                       PORT_ResetBits(SEG7_SEG_D_PORT, SEG7_SEG_D_PIN);
    
    (pattern & 0x04) ? PORT_SetBits(SEG7_SEG_E_PORT, SEG7_SEG_E_PIN) : 
                       PORT_ResetBits(SEG7_SEG_E_PORT, SEG7_SEG_E_PIN);
    
    (pattern & 0x02) ? PORT_SetBits(SEG7_SEG_F_PORT, SEG7_SEG_F_PIN) : 
                       PORT_ResetBits(SEG7_SEG_F_PORT, SEG7_SEG_F_PIN);
    
    (pattern & 0x01) ? PORT_SetBits(SEG7_SEG_G_PORT, SEG7_SEG_G_PIN) : 
                       PORT_ResetBits(SEG7_SEG_G_PORT, SEG7_SEG_G_PIN);
    
    /* Handle decimal point */
    decimal_point ? PORT_SetBits(SEG7_SEG_DP_PORT, SEG7_SEG_DP_PIN) : 
                    PORT_ResetBits(SEG7_SEG_DP_PORT, SEG7_SEG_DP_PIN);
}

/**
  * @brief  Select active digit for multiplexing
  */
static void SEG7_SelectDigit(uint8_t digit_index)
{
    /* Turn off all digits first */
    SEG7_AllDigitsOff();
    
    /* Select the active digit (active low with transistor drivers) */
    switch (digit_index) {
        case 0:
            PORT_ResetBits(SEG7_DIGIT_1_PORT, SEG7_DIGIT_1_PIN);
            break;
        case 1:
            PORT_ResetBits(SEG7_DIGIT_2_PORT, SEG7_DIGIT_2_PIN);
            break;
        case 2:
            PORT_ResetBits(SEG7_DIGIT_3_PORT, SEG7_DIGIT_3_PIN);
            break;
        case 3:
            PORT_ResetBits(SEG7_DIGIT_4_PORT, SEG7_DIGIT_4_PIN);
            break;
        default:
            break;
    }
}

/**
  * @brief  Turn off all digits
  */
static void SEG7_AllDigitsOff(void)
{
    /* Turn off all digit selection pins (active low) */
    PORT_SetBits(SEG7_DIGIT_1_PORT, SEG7_DIGIT_1_PIN);
    PORT_SetBits(SEG7_DIGIT_2_PORT, SEG7_DIGIT_2_PIN);
    PORT_SetBits(SEG7_DIGIT_3_PORT, SEG7_DIGIT_3_PIN);
    PORT_SetBits(SEG7_DIGIT_4_PORT, SEG7_DIGIT_4_PIN);
}

/**
  * @brief  Get segment pattern for a digit
  */
static uint8_t SEG7_GetSegmentPattern(uint8_t digit)
{
    if (digit > 9) {
        return 0; /* Blank for invalid digits */
    }
    return seg7_digit_patterns[digit];
}

/**
  * @brief  Display test pattern
  */
void SEG7_TestPattern(void)
{
    static uint8_t test_counter = 0;
    
    for (uint8_t i = 0; i < SEG7_NUM_DIGITS; i++) {
        seg7_display.digits[i] = (test_counter + i) % 10;
        seg7_display.decimal_points[i] = (i == 1);
    }
    
    test_counter = (test_counter + 1) % 10;
}