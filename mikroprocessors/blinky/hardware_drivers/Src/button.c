/**
  * @file    buttons.c
  * @brief   Button handling implementation
  * @version 1.0
  */

#include "button.h"
#include "hardware_drivers.h"
#include <string.h>

/* Button instances */
static Button_TypeDef button_start_stop;
static Button_TypeDef button_lap_reset;
static Button_TypeDef button_mode;

/* Constants */
#define DEBOUNCE_TIME_MS     20
#define HOLD_TIME_MS        1000

/**
  * @brief  Initialize buttons
  */
void Buttons_Init(void)
{
    PORT_InitTypeDef port_init;
    
    /* Enable clock for ports */
    RST_CLK_PCLKcmd(RST_CLK_PCLK_PORTA, ENABLE);
    RST_CLK_PCLKcmd(RST_CLK_PCLK_PORTB, ENABLE);
    
    /* Configure buttons as inputs with pull-up */
    PORT_StructInit(&port_init);
    port_init.PORT_Pin = BUTTON_START_STOP_PIN;
    port_init.PORT_OE = PORT_OE_IN;
    port_init.PORT_FUNC = PORT_FUNC_PORT;
    port_init.PORT_MODE = PORT_MODE_DIGITAL;
    port_init.PORT_PULL_UP = PORT_PULL_UP_ON;
    port_init.PORT_PULL_DOWN = PORT_PULL_DOWN_OFF;
    PORT_Init(BUTTON_START_STOP_PORT, &port_init);
    
    PORT_StructInit(&port_init);
    port_init.PORT_Pin = BUTTON_LAP_RESET_PIN | BUTTON_MODE_PIN;
    port_init.PORT_OE = PORT_OE_IN;
    port_init.PORT_FUNC = PORT_FUNC_PORT;
    port_init.PORT_MODE = PORT_MODE_DIGITAL;
    port_init.PORT_PULL_UP = PORT_PULL_UP_ON;
    port_init.PORT_PULL_DOWN = PORT_PULL_DOWN_OFF;
    PORT_Init(BUTTON_LAP_RESET_PORT, &port_init);
    
    /* Initialize button structures */
    memset(&button_start_stop, 0, sizeof(button_start_stop));
    memset(&button_lap_reset, 0, sizeof(button_lap_reset));
    memset(&button_mode, 0, sizeof(button_mode));
    
    button_start_stop.debounce_time = HD_GetTick();
    button_lap_reset.debounce_time = HD_GetTick();
    button_mode.debounce_time = HD_GetTick();
}

/**
  * @brief  Process button states - call periodically
  */
void Buttons_Process(void)
{
    uint32_t current_time = HD_GetTick();
    
    /* Process Start/Stop button */
    ProcessButton(&button_start_stop, 
                  BUTTON_START_STOP_PORT, 
                  BUTTON_START_STOP_PIN, 
                  current_time);
    
    /* Process Lap/Reset button */
    ProcessButton(&button_lap_reset, 
                  BUTTON_LAP_RESET_PORT, 
                  BUTTON_LAP_RESET_PIN, 
                  current_time);
    
    /* Process Mode button */
    ProcessButton(&button_mode, 
                  BUTTON_MODE_PORT, 
                  BUTTON_MODE_PIN, 
                  current_time);
}

/**
  * @brief  Process individual button state
  */
static void ProcessButton(Button_TypeDef* button, MDR_PORT_TypeDef* port, uint32_t pin, uint32_t current_time)
{
    bool current_state = (PORT_ReadInputDataBit(port, pin) == RESET); /* Active low */
    
    /* Debounce logic */
    if (current_state != button->last_physical_state) {
        button->debounce_time = current_time;
        button->last_physical_state = current_state;
        return;
    }
    
    if ((current_time - button->debounce_time) < DEBOUNCE_TIME_MS) {
        return;
    }
    
    /* State machine */
    switch (button->state) {
        case BUTTON_RELEASED:
            if (current_state) {
                button->state = BUTTON_PRESSED;
                button->press_time = current_time;
                button->event = BUTTON_EVENT_PRESS;
            } else {
                button->event = BUTTON_EVENT_NONE;
            }
            break;
            
        case BUTTON_PRESSED:
            if (!current_state) {
                button->state = BUTTON_RELEASED;
                button->event = BUTTON_EVENT_RELEASE;
            } else if ((current_time - button->press_time) >= HOLD_TIME_MS) {
                button->state = BUTTON_HELD;
                button->event = BUTTON_EVENT_HOLD;
            } else {
                button->event = BUTTON_EVENT_NONE;
            }
            break;
            
        case BUTTON_HELD:
            if (!current_state) {
                button->state = BUTTON_RELEASED;
                button->event = BUTTON_EVENT_RELEASE;
            } else {
                button->event = BUTTON_EVENT_NONE;
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

/**
  * @brief  Get Lap/Reset button event
  */
Button_EventTypeDef Buttons_GetLapResetEvent(void)
{
    Button_EventTypeDef event = button_lap_reset.event;
    button_lap_reset.event = BUTTON_EVENT_NONE;
    return event;
}

/**
  * @brief  Get Mode button event
  */
Button_EventTypeDef Buttons_GetModeEvent(void)
{
    Button_EventTypeDef event = button_mode.event;
    button_mode.event = BUTTON_EVENT_NONE;
    return event;
}