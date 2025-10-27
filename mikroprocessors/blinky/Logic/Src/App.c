#include "app.h"
#include "main.h"
#include "hardware_drivers.h"

/* Application global variables */
static App_State_TypeDef app_state = APP_STATE_INIT;
static App_Mode_TypeDef app_mode = APP_MODE_PWM;
extern button_data user_button; 

/* Private function prototypes */
static void app_handle_button_events(uint32_t current_time);
static void app_update_indicators(void);

/**
  * @brief  Initialize application
  */
void app_init(void) {
    /* Initialize hardware drivers */
    HD_System_Init();
    
    /* Initialize TIMER1 for 1kHz interrupts */
    HD_Timer1_Init();
    
    /* Initialize LEDs */
    LED_Init();
    
    /* Initialize button on PA0 (active low - pressed = 0) */
    button_init(&user_button, MDR_PORTA, 0, 0);
    
    /* Visual indication of initialization */
    LED_AllOn();
    HD_Delay_ms_blocking(200);
    LED_AllOff();
    HD_Delay_ms_blocking(200);
    LED_AllOn();
    HD_Delay_ms_blocking(200);
    LED_AllOff();
    
    app_state = APP_STATE_RUNNING;
}

/**
  * @brief  Start application (start timers)
  */
void app_start(void) {
    /* Timer1 is already started in HD_System_Init() */
    /* Additional timers can be started here if needed */
    
    
}

/**
  * @brief  Main application process (called from timer interrupt)
  */
void app_process(void) {
    static uint32_t last_button_process = 0;
    static uint32_t last_sequence_update = 0;
    uint32_t current_time = HD_GetTick();
    
    /* Process button every 10ms to reduce CPU load */
    if (current_time - last_button_process >= 10) {
        read_button(&user_button, current_time);
        app_handle_button_events(current_time);
        last_button_process = current_time;
    }
    
    /* Update LED sequence every 100ms */
    if (current_time - last_sequence_update >= 100) {
        static uint8_t current_led = 0;
        
        // Turn off all LEDs
        LED_AllOff();
        
        // Turn on current LED
        LED_On((LED_TypeDef)current_led);
        
        // Move to next LED
        current_led = (current_led + 1) % LED_COUNT;
        
        last_sequence_update = current_time;
    }
}

/**
  * @brief  Handle button events
  */
static void app_handle_button_events(uint32_t current_time) {
    static uint32_t last_click_time = 0;
    static uint8_t button_processed = 0;
    
    // Process button state changes
    switch(user_button.state) {
        case BTN_PRESSED:
            if (!button_processed) {
                // Visual feedback for button press
                LED_AllOn();
                HD_Delay_ms_blocking(100);
                LED_AllOff();
                button_processed = 1;
            }
            break;
            
        case BTN_RELEASED:
            button_processed = 0; // Reset for next press
            break;
            
        case BTN_HOLD:
            // Optional: Add hold functionality if needed
            break;
    }
    
    // Reset clicks after timeout
#ifdef RST_BTN_AFTER_TIME
    if (user_button.clicks > 0 && (current_time - user_button.last_click_time) > BTN_CLICK_RESET_TIME) {
        button_reset_clicks(&user_button);
    }
#endif
}

/**
  * @brief  Update status indicators
  */
static void app_update_indicators(void) {
    static uint32_t last_blink_time = 0;
    uint32_t current_time = HD_GetTick();
    
    /* Blink LED1 when process is disabled */
    if (!LED_ProcessIsEnabled()) {
        if (current_time - last_blink_time >= 500) {
            LED_Toggle(LED1);
            last_blink_time = current_time;
        }
    } else {
        /* LED1 indicates current mode when process is enabled */
        switch(app_mode) {
            case APP_MODE_PWM:
                LED_On(LED1);  // Solid for PWM mode
                break;
            case APP_MODE_SEQUENCE:
                /* Blink slowly for sequence mode */
                if (current_time - last_blink_time >= 300) {
                    LED_Toggle(LED1);
                    last_blink_time = current_time;
                }
                break;
            case APP_MODE_STATIC:
                LED_Off(LED1);  // Off for static mode
                break;
        }
    }
}

/**
  * @brief  Set application mode
  */
void app_set_mode(App_Mode_TypeDef mode) {
    app_mode = mode;
    
    /* Stop current activities */
    LED_StopPWMWave();
    LED_SequenceStop();
    LED_AllOff();
    
    /* Start new mode */
    switch(mode) {
        case APP_MODE_PWM:
            LED_StartPWMWave();
            LED_SetWaveSpeed(1);
            LED_SetPWMPeriod(1500);
            break;
            
        case APP_MODE_SEQUENCE:
            LED_Sequence(200);  // 200ms per LED
            break;
            
        case APP_MODE_STATIC:
            /* Static mode - all LEDs off by default */
            LED_AllOff();
            break;
    }
    
    /* Visual feedback for mode change */
    for (int i = 0; i <= mode; i++) {
        LED_On((LED_TypeDef)i);
    }
    HD_Delay_ms_blocking(300);
    for (int i = 0; i < LED_COUNT; i++) {
        LED_Off((LED_TypeDef)i);
    }
}

/**
  * @brief  Get current application mode
  */
App_Mode_TypeDef app_get_mode(void) {
    return app_mode;
}

/**
  * @brief  Get application state
  */
App_State_TypeDef app_get_state(void) {
    return app_state;
}

/**
  * @brief  Toggle LED process on/off
  */
void app_toggle_process(void) {
    LED_ProcessEnable(!LED_ProcessIsEnabled());
}