#ifndef APP_H
#define APP_H

#include <stdint.h>
#include "leds.h"
#include "button.h"

/* Application states */
typedef enum {
    APP_STATE_INIT = 0,
    APP_STATE_RUNNING = 1,
    APP_STATE_PAUSED = 2,
    APP_STATE_ERROR = 3
} App_State_TypeDef;

/* Application modes */
typedef enum {
    APP_MODE_PWM = 0,
    APP_MODE_SEQUENCE = 1,
    APP_MODE_STATIC = 2
} App_Mode_TypeDef;

/* Function prototypes */
void app_init(void);
void app_start(void);
void app_process(void);
void app_set_mode(App_Mode_TypeDef mode);
App_Mode_TypeDef app_get_mode(void);
App_State_TypeDef app_get_state(void);
void app_toggle_process(void);

#endif /* APP_H */
