#ifndef LEDS_H
#define LEDS_H

#include <stdint.h>
#include "hardware_drivers.h"

/* LED definitions */
typedef enum {
    LED1 = 0,  /* PORTA Pin 1 */
    LED2 = 1,  /* PORTA Pin 5 */
    LED3 = 2,  /* PORTA Pin 3 */
    LED4 = 3,  /* PORTC Pin 2 */
    LED_COUNT
} LED_TypeDef;

/* LED pins configuration */
#define LED1_PIN    PORT_Pin_1
#define LED1_PORT   MDR_PORTA

#define LED2_PIN    PORT_Pin_5
#define LED2_PORT   MDR_PORTA

#define LED3_PIN    PORT_Pin_3
#define LED3_PORT   MDR_PORTA

#define LED4_PIN    PORT_Pin_2
#define LED4_PORT   MDR_PORTC

/* Function prototypes */
void LED_Init(void);
void LED_On(LED_TypeDef led);
void LED_On_ms(LED_TypeDef led, uint32_t ms);
void LED_Off(LED_TypeDef led);
void LED_Toggle(LED_TypeDef led);
void LED_SetState(LED_TypeDef led, uint8_t state);
uint8_t LED_GetState(LED_TypeDef led);
void LED_Sequence(uint32_t delay_time);
void LED_SequenceStop(void);
uint8_t LED_SequenceIsActive(void);
void LED_AllOn(void);
void LED_AllOff(void);
void LED_Process(void);

#endif /* LEDS_H */
