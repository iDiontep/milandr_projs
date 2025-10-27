#include "button.h"
#include "main.h"
#include "hardware_drivers.h"

/*-------------------- F U N C T I O N S --------------------*/

/**
 * @brief Read button state directly from GPIO
 * @param port GPIO port
 * @param pin GPIO pin
 * @return Current button state (0 or 1)
 */
uint8_t button_read_gpio(MDR_PORT_TypeDef *port, uint16_t pin) {
    return (port->RXTX & (1UL << pin)) ? 1 : 0;
}

/**
 * @brief Set initial parameters for button's structure
 * @param btn structure for initialization
 * @param port PORT for this button
 * @param pin pin number for this button
 * @param press_val logic value then button is pressed
 */
void button_init(button_data *btn, MDR_PORT_TypeDef *port, uint16_t pin, uint8_t press_val) {
    btn->port = port;
    btn->pin = pin;
    btn->press_val = press_val;
    btn->val = button_read_gpio(port, pin);
    btn->prev_val = !btn->press_val;
    btn->clicks = 0;
    btn->state = BTN_RELEASED;
    btn->change_time = HD_GetTick();
    btn->last_click_time = HD_GetTick();
    btn->hold_time = 0;
}

/**
 * @brief Updates button's structure: state, time stamps for press & release, click count
 * not changes in this function:
 * @param btn->port PORT (sets in button_init)
 * @param btn->pin pin number (sets in button_init)
 * @param btn->change_time time when button value on gpio changed
 * @param btn->last_click_time time when last click ended
 * @param btn->hold_time time duration after button realized it is long press
 * @param btn->val logic value of gpio pin
 * @param btn->prev_val previous value of gpio pin
 * @param btn->state BTN_RELEASED/BTN_CLICKED/BTN_HOLD
 * @param btn->clicks Clicks counter resets after BTN_CLICK_CNTR_RST_TIME/long press/BTN_MAX_CLICKS_COUNT
 * @param btn->press_val logic value then button is pressed (sets in button_init)
 */
void read_button(button_data *btn, uint32_t cur_time) {
    // read button val from GPIO
    btn->val = button_read_gpio(btn->port, btn->pin);

    // compare val & prev_val
    if(btn->val != btn->prev_val) {
        // check if time > debounce time
        if(cur_time >= (btn->change_time + BTN_DEBOUNCE_TIME)) {

            // if button was pressed (click & hold)
            if(btn->val == btn->press_val) {
                btn->state = BTN_PRESSED;
                // reset hold time after next press
                btn->hold_time = 0;
            }

            // else button was released (click & release)
            else if(btn->val != btn->press_val) {
                // click: if button was pressed and released before max time for click & check for max clicks count (ternary operator)
                if((btn->prev_val == btn->press_val) && btn->state != BTN_HOLD) {
                    // if clicks in same series
                    if(((cur_time - btn->last_click_time) <= BTN_CLICKS_SERIES_INTERVAL) || (btn->clicks == 0)) {
                          btn->clicks = (btn->clicks < BTN_MAX_CLICKS_COUNT) ? btn->clicks + 1 : 0;
                    }
                    else if((cur_time - btn->last_click_time) > BTN_CLICKS_SERIES_INTERVAL) {
                        btn->clicks = 0;
                    }
                    btn->last_click_time = cur_time;
                }
                btn->state = BTN_RELEASED;
            }

            // update change time & prev val
            btn->change_time = cur_time;
            btn->prev_val = btn->val;
        }
    }

    // increment btn hold time
    else if((btn->val == btn->press_val) && ((cur_time - btn->change_time) > BTN_HOLD_TIME)) {
        btn->state = BTN_HOLD;
        btn->hold_time++;
    }
}

/**
 * @brief Get current button state
 * @param btn button structure
 * @return button state
 */
uint8_t button_get_state(button_data *btn) {
    return btn->state;
}

/**
 * @brief Get click count
 * @param btn button structure
 * @return number of clicks
 */
uint8_t button_get_clicks(button_data *btn) {
    return btn->clicks;
}

/**
 * @brief Reset click count
 * @param btn button structure
 */
void button_reset_clicks(button_data *btn) {
    btn->clicks = 0;
}