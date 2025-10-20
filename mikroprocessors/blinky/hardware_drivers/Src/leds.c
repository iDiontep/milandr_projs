#include "leds.h"
#include "main.h"
#include <math.h>

/* LED configuration structure with direct register access */
typedef struct {
    __IO uint32_t* reg;     // Pointer to RXTX register
    uint32_t mask;          // Bit mask for this LED
    uint8_t state;
    uint32_t start_time;
} LED_ConfigTypeDef;

/* LED configuration array with direct register access */
static LED_ConfigTypeDef LED_Config[LED_COUNT] = {
    {&LED1_PORT->RXTX, LED1_MASK, 0, 0},
    {&LED2_PORT->RXTX, LED2_MASK, 0, 0},
    {&LED3_PORT->RXTX, LED3_MASK, 0, 0},
    {&LED4_PORT->RXTX, LED4_MASK, 0, 0}
};

// Sequence control variables
static uint8_t sequence_active = 0;
static uint8_t current_led = 0;
static uint32_t sequence_start_time = 0;
static uint32_t led_on_time = 0;

// PWM Wave control variables
static uint32_t pwm_counter = 0;
static uint32_t pwm_period = DEFAULT_PWM_PERIOD;
static uint32_t wave_speed = DEFAULT_WAVE_SPEED;
static uint8_t wave_active = 0;
static uint32_t last_pwm_update = 0;
static const uint32_t pwm_update_interval = 0; // 10ms update interval


void LED_Init(void)
{
    PORT_InitTypeDef Port_InitStructure;
    
    /* Enable clocks for PORTA and PORTC */
    RST_CLK_PCLKcmd(RST_CLK_PCLK_PORTA, ENABLE);
    Port_InitStructure.PORT_Pin = PORT_Pin_1 | PORT_Pin_3 | PORT_Pin_5;
    Port_InitStructure.PORT_OE = PORT_OE_OUT;
    Port_InitStructure.PORT_MODE = PORT_MODE_DIGITAL;
    Port_InitStructure.PORT_SPEED = PORT_SPEED_FAST;
    PORT_Init(MDR_PORTA, &Port_InitStructure);
    
    RST_CLK_PCLKcmd(RST_CLK_PCLK_PORTC, ENABLE);
    Port_InitStructure.PORT_Pin = PORT_Pin_2;
    Port_InitStructure.PORT_OE = PORT_OE_OUT;
    Port_InitStructure.PORT_MODE = PORT_MODE_DIGITAL;
    Port_InitStructure.PORT_SPEED = PORT_SPEED_FAST;
    PORT_Init(MDR_PORTC, &Port_InitStructure);

    /* Turn off all LEDs initially using bit operations */
    LED_AllOff();
}

/**
  * @brief  Turns on specified LED using direct register access
  */
void LED_On(LED_TypeDef led)
{
    uint32_t idx = (uint32_t)led & (LED_COUNT - 1);  // Bitwise bounds check
    BIT_SET(*LED_Config[idx].reg, LED_Config[idx].mask);
    LED_Config[idx].state = 1;
    LED_Config[idx].start_time = HD_GetTick();
}

/**
  * @brief  Turns on specified LED for specified time (non-blocking)
  */
void LED_On_ms(LED_TypeDef led, uint32_t ms) {
    LED_On(led);
    // Timing handled in LED_Process
}

/**
  * @brief  Process LED timing (called from timer interrupt)
  */


/**
  * @brief  Turns off specified LED using direct register access
  */
void LED_Off(LED_TypeDef led)
{
    uint32_t idx = (uint32_t)led & (LED_COUNT - 1);  // Bitwise bounds check
    BIT_CLR(*LED_Config[idx].reg, LED_Config[idx].mask);
    LED_Config[idx].state = 0;
    LED_Config[idx].start_time = 0;
}

/**
  * @brief  Toggles specified LED using direct register access
  */
void LED_Toggle(LED_TypeDef led)
{
    uint32_t idx = (uint32_t)led & (LED_COUNT - 1);  // Bitwise bounds check
    BIT_TGL(*LED_Config[idx].reg, LED_Config[idx].mask);
    LED_Config[idx].state ^= 1U;  // Toggle state using XOR
    LED_Config[idx].start_time = HD_GetTick() & -(LED_Config[idx].state == 1); // Smart conditional update
}

/**
  * @brief  Sets LED state using ternary operator
  */
void LED_SetState(LED_TypeDef led, uint8_t state)
{
    uint32_t idx = (uint32_t)led & (LED_COUNT - 1);
    
    // Single line with ternary operator
    state ? BIT_SET(*LED_Config[idx].reg, LED_Config[idx].mask) : 
            BIT_CLR(*LED_Config[idx].reg, LED_Config[idx].mask);
    
    LED_Config[idx].state = state;
    LED_Config[idx].start_time = HD_GetTick() & -(state == 1); // Update time only when turning on
}

/**
  * @brief  Gets current LED state
  */
uint8_t LED_GetState(LED_TypeDef led)
{
    uint32_t idx = (uint32_t)led & (LED_COUNT - 1);
    return LED_Config[idx].state;
}

/**
  * @brief  Starts LED sequence (running light)
  */
void LED_Sequence(uint32_t delay_time)
{
    sequence_active = 1;
    current_led = 0;
    sequence_start_time = HD_GetTick();
    led_on_time = delay_time;
    
    // Turn on first LED
    BIT_SET(*LED_Config[current_led].reg, LED_Config[current_led].mask);
    LED_Config[current_led].state = 1;
}

/**
  * @brief  Stops LED sequence
  */
void LED_SequenceStop(void)
{
    sequence_active = 0;
    LED_AllOff();
}

/**
  * @brief  Check if sequence is active
  */
uint8_t LED_SequenceIsActive(void)
{
    return sequence_active;
}

/**
  * @brief  Turns on all LEDs using bit operations
  */
void LED_AllOn(void)
{
    for (int i = 0; i < LED_COUNT; i++) {
        BIT_SET(*LED_Config[i].reg, LED_Config[i].mask);
        LED_Config[i].state = 1;
    }
}

/**
  * @brief  Turns off all LEDs using bit operations
  */
void LED_AllOff(void)
{
    for (int i = 0; i < LED_COUNT; i++) {
        BIT_CLR(*LED_Config[i].reg, LED_Config[i].mask);
        LED_Config[i].state = 0;
    }
}

/* PWM Wave functions */
static uint32_t calculate_pwm_value(uint8_t led_index, uint32_t counter) {
    uint32_t phase_shift = (pwm_period / LED_COUNT) * led_index;
    uint32_t position = (counter + phase_shift) % pwm_period;
    
    float angle = (2.0f * 3.14159f * position) / pwm_period;
    float sine_value = (sinf(angle) + 1.0f) / 2.0f;
    
    return (uint32_t)(sine_value * 100);
}

static void set_led_pwm(LED_TypeDef led, uint32_t pwm_value) {
    static uint32_t pwm_step = 0;
    pwm_step = (pwm_step + 1) % 100;
    
    if (pwm_step < pwm_value) {
        LED_On(led);
    } else {
        LED_Off(led);
    }
}

void LED_StartPWMWave(void) {
    wave_active = 1;
    pwm_counter = 0;
    last_pwm_update = HD_GetTick();
}

void LED_StopPWMWave(void) {
    wave_active = 0;
    LED_AllOff();
}

void LED_SetWaveSpeed(uint32_t speed) {
    wave_speed = speed;
}

void LED_SetPWMPeriod(uint32_t period) {
    pwm_period = period;
}

uint8_t LED_PWMWaveIsActive(void) {
    return wave_active;
}

/**
  * @brief  Main LED process function called from timer interrupt
  */
void LED_Process(void){
    uint32_t current_time = HD_GetTick();
    
    /* Process PWM Wave if active */
    if (wave_active) {
        if ((current_time - last_pwm_update) >= pwm_update_interval) {
            last_pwm_update = current_time;
            
            pwm_counter = (pwm_counter + wave_speed) % pwm_period;
            
            for (int i = 0; i < LED_COUNT; i++) {
                uint32_t pwm_value = calculate_pwm_value(i, pwm_counter);
                set_led_pwm((LED_TypeDef)i, pwm_value);
            }
        }
    }
    
    /* Process LED sequence if active */
    if (sequence_active) {
        if ((current_time - sequence_start_time) >= led_on_time) {
            LED_Off((LED_TypeDef)current_led);
            current_led = (current_led + 1) % LED_COUNT;
            LED_On((LED_TypeDef)current_led);
            sequence_start_time = current_time;
        }
    }
}