/**
  * @file    buttons.h
  * @brief   Button handling for MDR32F9Q2I
  * @version 1.0
  */

#ifndef BUTTONS_H
#define BUTTONS_H

#include <stdint.h>
#include <stdbool.h>
#include "MDR32F9Q2I.h"
#include "MDR32FxQI_rst_clk.h"
#include "MDR32FxQI_port.h"

/* Button pins configuration */
#define BUTTON_START_STOP_PORT   MDR_PORTA
#define BUTTON_START_STOP_PIN    PORT_Pin_0

/* Button states */
typedef enum {
    BUTTON_RELEASED = 0,
    BUTTON_PRESSED = 1,
    BUTTON_HELD = 2
} Button_StateTypeDef;

/* Button events */
typedef enum {
    BUTTON_EVENT_NONE = 0,
    BUTTON_EVENT_PRESS,
    BUTTON_EVENT_RELEASE,
    BUTTON_EVENT_HOLD
} Button_EventTypeDef;

/*-------------------- S T R U C T U R E S --------------------*/

typedef struct {
    Button_StateTypeDef state;
    Button_EventTypeDef event;
    uint32_t press_time;
    bool last_physical_state;
    uint32_t debounce_time;
} Button_TypeDef;

/* Function prototypes */
void Buttons_Init(void);
void Buttons_Process(void);
Button_EventTypeDef Buttons_GetStartStopEvent(void);

#endif /* BUTTONS_H */