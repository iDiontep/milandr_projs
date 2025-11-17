/**
  * @file    buttons.c
  * @brief   Button handling implementation
  * @version 1.0
  */

#include "button.h"
#include "hardware_drivers.h"
#include <string.h>

/* Button instance */
static Button_TypeDef button_start_stop;

/* Constants */
#define DEBOUNCE_TIME_MS     20
#define HOLD_TIME_MS        1000

/**
  * @brief  Initialize buttons
  */
void Buttons_Init(void)
{
    PORT_InitTypeDef port_init;
    
    /* Enable clock for port */
    RST_CLK_PCLKcmd(RST_CLK_PCLK_PORTA, ENABLE);
    
    /* Configure button as input with pull-up */
    PORT_StructInit(&port_init);
    port_init.PORT_Pin = BUTTON_START_STOP_PIN;
    port_init.PORT_OE = PORT_OE_IN;
    port_init.PORT_FUNC = PORT_FUNC_PORT;
    port_init.PORT_MODE = PORT_MODE_DIGITAL;
    port_init.PORT_PULL_UP = PORT_PULL_UP_ON;
    port_init.PORT_PULL_DOWN = PORT_PULL_DOWN_OFF;
    PORT_Init(BUTTON_START_STOP_PORT, &port_init);
    
    /* Initialize button structure */
    memset(&button_start_stop, 0, sizeof(button_start_stop));
    button_start_stop.debounce_time = HD_GetTick();
}

/**
  * @brief  Process button states - call periodically
  */
void Buttons_Process(void)
{
    uint32_t current_time = HD_GetTick();
    bool current_state = (PORT_ReadInputDataBit(BUTTON_START_STOP_PORT, 
                                               BUTTON_START_STOP_PIN) == RESET); /* Active low */
    
    /* Debounce logic */
    if (current_state != button_start_stop.last_physical_state) {
        button_start_stop.debounce_time = current_time;
        button_start_stop.last_physical_state = current_state;
        return;
    }
    
    if ((current_time - button_start_stop.debounce_time) < DEBOUNCE_TIME_MS) {
        return;
    }
    
    /* State machine */
    switch (button_start_stop.state) {
        case BUTTON_RELEASED:
            if (current_state) {
                button_start_stop.state = BUTTON_PRESSED;
                button_start_stop.press_time = current_time;
                button_start_stop.event = BUTTON_EVENT_PRESS;
            } else {
                button_start_stop.event = BUTTON_EVENT_NONE;
            }
            break;
            
        case BUTTON_PRESSED:
            if (!current_state) {
                button_start_stop.state = BUTTON_RELEASED;
                button_start_stop.event = BUTTON_EVENT_RELEASE;
            } else if ((current_time - button_start_stop.press_time) >= HOLD_TIME_MS) {
                button_start_stop.state = BUTTON_HELD;
                button_start_stop.event = BUTTON_EVENT_HOLD;
            } else {
                button_start_stop.event = BUTTON_EVENT_NONE;
            }
            break;
            
        case BUTTON_HELD:
            if (!current_state) {
                button_start_stop.state = BUTTON_RELEASED;
                button_start_stop.event = BUTTON_EVENT_RELEASE;
            } else {
                button_start_stop.event = BUTTON_EVENT_NONE;
            }
            break;
    }
}

/**
  * @brief  Get Start/Stop button event
  */
Button_EventTypeDef Buttons_GetStartStopEvent(void)
{
    Button_EventTypeDef event = button_start_stop.event;
    button_start_stop.event = BUTTON_EVENT_NONE;
    return event;
}