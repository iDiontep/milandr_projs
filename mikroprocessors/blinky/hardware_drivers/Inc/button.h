#ifndef BUTTON_H_
#define BUTTON_H_

/*-------------------- I N C L U D E S --------------------*/
#include "stdint.h"
#include "main.h"
#include "MDR32FxQI_port.h"

/*-------------------- D E F I N E S --------------------*/
// resets clicks count after BTN_MAX_CLICK_TIME automaticaly
#define RST_BTN_AFTER_TIME

#define BTN_DEBOUNCE_TIME        (25)
#define BTN_CLICK_RESET_TIME     (2000)
#define BTN_CLICKS_SERIES_INTERVAL (5000)
#define BTN_HOLD_TIME            (2000)

#define BTN_MAX_CLICKS_COUNT     (3)

// Button on PA0
#define BUTTON_PORT              MDR_PORTA
#define BUTTON_PIN               (0)

/*-------------------- E N U M S --------------------*/
typedef enum {
    BTN_RELEASED = 0, // State when button not pressed or released after click and long press
    BTN_PRESSED = 1,  // State when button pressed < BTN_CLICK_TIME after that was released
    BTN_HOLD = 2      // State when button pressed > BTN_HOLD_TIME
} BTN_States_TypeDef;

/*-------------------- S T R U C T U R E S --------------------*/
typedef struct {
    MDR_PORT_TypeDef *port;      // PORT
    uint16_t pin;                // Pin number
    uint32_t change_time;        // Time when button state changed
    uint32_t last_click_time;    // Time after last click
    uint32_t hold_time;          // Time duration after button realized it is long press
    uint8_t val;                 // Pin logic value (1/0)
    uint8_t prev_val;            // Previous pin logic value (1/0)
    uint8_t state;               // Current state from enum
    uint8_t clicks;              // Clicks counter
    uint8_t press_val;           // Voltage (0/1) on GPIO Pin when button is pressed
} button_data;

/*-------------------- F U N C T I O N S --------------------*/

// Set initial parameters for button's structure
void button_init(button_data *btn, MDR_PORT_TypeDef *port, uint16_t pin, uint8_t press_val);

// Updates button's structure: state, time stamps for press & release, click count
void read_button(button_data *btn, uint32_t cur_time);

// Read button state directly from GPIO
uint8_t button_read_gpio(MDR_PORT_TypeDef *port, uint16_t pin);

// Get button state
uint8_t button_get_state(button_data *btn);

// Get click count
uint8_t button_get_clicks(button_data *btn);

/**
 * @brief Reset click count
 * @param btn button structure
 */
void button_reset_clicks(button_data *btn);

#endif /* BUTTON_H_ */
