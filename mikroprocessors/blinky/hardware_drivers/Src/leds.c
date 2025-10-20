#include "leds.h"
#include "main.h"

/* LED configuration structure */
typedef struct {
    MDR_PORT_TypeDef* port;
    uint32_t pin;
    uint8_t state;
    uint32_t start_time;  // A?aiy aee??aiey naaoiaeiaa
} LED_ConfigTypeDef;

/* LED configuration array */
static LED_ConfigTypeDef LED_Config[LED_COUNT] = {
    {LED1_PORT, LED1_PIN, 0, 0},
    {LED2_PORT, LED2_PIN, 0, 0},
    {LED3_PORT, LED3_PIN, 0, 0},
    {LED4_PORT, LED4_PIN, 0, 0}
};

// Noaoe?aneea ia?aiaiiua aey oi?aaeaiey iineaaiaaoaeuiinou?
static uint8_t sequence_active = 0;
static uint8_t current_led = 0;
static uint32_t sequence_start_time = 0;
static uint32_t led_on_time = 0;

void LED_Init(void)
{
    PORT_InitTypeDef Port_InitStructure;
    
    /* Enable clocks for PORTA and PORTC */
    RST_CLK_PCLKcmd(RST_CLK_PCLK_PORTA,ENABLE);
	Port_InitStructure.PORT_Pin = PORT_Pin_1|PORT_Pin_3|PORT_Pin_5;
	Port_InitStructure.PORT_OE = PORT_OE_OUT;
	Port_InitStructure.PORT_MODE = PORT_MODE_DIGITAL;
	Port_InitStructure.PORT_SPEED = PORT_SPEED_FAST;
	PORT_Init(MDR_PORTA, &Port_InitStructure);
	
	RST_CLK_PCLKcmd(RST_CLK_PCLK_PORTC,ENABLE);
	Port_InitStructure.PORT_Pin = PORT_Pin_2;
	Port_InitStructure.PORT_OE = PORT_OE_OUT;
	Port_InitStructure.PORT_MODE = PORT_MODE_DIGITAL;
  Port_InitStructure.PORT_SPEED = PORT_SPEED_FAST;
	PORT_Init(MDR_PORTC, &Port_InitStructure);

    /* Turn off all LEDs initially */
    LED_AllOff();
}

/**
  * @brief  Turns on specified LED
  * @param  led: LED to turn on (LED1, LED2, LED3, LED4)
  * @retval None
  */
void LED_On(LED_TypeDef led)
{
    if (led < LED_COUNT) {
        PORT_SetBits(LED_Config[led].port, LED_Config[led].pin);
        LED_Config[led].state = 1;
        LED_Config[led].start_time = HD_GetTick(); // Caiiieiaai a?aiy aee??aiey
    }
}

/**
  * @brief  Turns on specified LED for specified time (non-blocking)
  * @param  led: LED to turn on
  * @param  ms: time in milliseconds
  * @retval None
  */
void LED_On_ms(LED_TypeDef led, uint32_t ms) {
    LED_On(led);
    // A?aiy auee??aiey aoaao ia?aaioaii a LED_Process ooieoee
}

/**
  * @brief  Process LED timing (called from timer interrupt)
  * @param  None
  * @retval None
  */
void LED_Process(void)
{
    uint32_t current_time = HD_GetTick();
	for (int i = 0; i < LED_COUNT; i++) {	
		LED_Toggle(i);
	}
//    
//    // Ia?aaaouaaai iineaaiaaoaeuiinou naaoiaeiaia
//    if (sequence_active) {
//        uint32_t elapsed_time = current_time - sequence_start_time;
//        uint32_t step_duration = led_on_time;
//        
//        // Ii?aaaeyai, eaeie naaoiaeia aie?ai ai?aou a aaiiue iiiaio
//        uint8_t target_led = (elapsed_time / step_duration) % LED_COUNT;
//        
//        // Anee ia?aee??eeenu ia iiaue naaoiaeia
//        if (target_led != current_led) {
//            // Auee??aai i?aauaouee naaoiaeia
//            if (current_led < LED_COUNT) {
//                LED_Off((LED_TypeDef)current_led);
//            }
//            
//            // Aee??aai iiaue naaoiaeia
//            LED_On((LED_TypeDef)target_led);
//            current_led = target_led;
//        }
//        
//        // Aaoiiaoe?anee auee??aai oaeouee naaoiaeia, anee aai a?aiy enoaeei
//        if (current_led < LED_COUNT && LED_Config[current_led].state) {
//            uint32_t led_elapsed_time = current_time - LED_Config[current_led].start_time;
//            if (led_elapsed_time >= step_duration) {
//                LED_Off((LED_TypeDef)current_led);
//            }
//        }
//    }
}

/**
  * @brief  Turns off specified LED
  * @param  led: LED to turn off (LED1, LED2, LED3, LED4)
  * @retval None
  */
void LED_Off(LED_TypeDef led)
{
    if (led < LED_COUNT) {
        PORT_ResetBits(LED_Config[led].port, LED_Config[led].pin);
        LED_Config[led].state = 0;
        LED_Config[led].start_time = 0; // Na?anuaaai a?aiy aee??aiey
    }
}

/**
  * @brief  Toggles specified LED
  * @param  led: LED to toggle (LED1, LED2, LED3, LED4)
  * @retval None
  */
void LED_Toggle(LED_TypeDef led)
{
    if (led < LED_COUNT) {
        if (LED_Config[led].state) {
            LED_Off(led);
        } else {
            LED_On(led);
        }
    }
}

/**
  * @brief  Sets LED state (on/off)
  * @param  led: LED to set
  * @param  state: 0 = off, any other value = on
  * @retval None
  */
void LED_SetState(LED_TypeDef led, uint8_t state)
{
    if (state) {
        LED_On(led);
    } else {
        LED_Off(led);
    }
}

/**
  * @brief  Gets current LED state
  * @param  led: LED to check
  * @retval 1 if LED is on, 0 if off or invalid LED
  */
uint8_t LED_GetState(LED_TypeDef led)
{
    if (led < LED_COUNT) {
        return LED_Config[led].state;
    }
    return 0;
}

/**
  * @brief  Starts LED sequence (running light)
  * @param  delay_time: time each LED stays on in milliseconds
  * @retval None
  */
void LED_Sequence(uint32_t delay_time)
{
    sequence_active = 1;
    current_led = 0;
    sequence_start_time = HD_GetTick();
    led_on_time = delay_time;
    
    // Aee??aai ia?aue naaoiaeia
    LED_On((LED_TypeDef)current_led);
}

/**
  * @brief  Stops LED sequence
  * @param  None
  * @retval None
  */
void LED_SequenceStop(void)
{
    sequence_active = 0;
    LED_AllOff();
}

/**
  * @brief  Check if sequence is active
  * @param  None
  * @retval 1 if active, 0 if not
  */
uint8_t LED_SequenceIsActive(void)
{
    return sequence_active;
}

/**
  * @brief  Turns on all LEDs
  * @param  None
  * @retval None
  */
void LED_AllOn(void)
{
    for (int i = 0; i < LED_COUNT; i++) {
        LED_On((LED_TypeDef)i);
    }
}

/**
  * @brief  Turns off all LEDs
  * @param  None
  * @retval None
  */
void LED_AllOff(void)
{
    for (int i = 0; i < LED_COUNT; i++) {
        LED_Off((LED_TypeDef)i);
    }
}