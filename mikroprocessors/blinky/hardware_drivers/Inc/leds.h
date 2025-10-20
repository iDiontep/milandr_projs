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

/* LED pins configuration with bit masks */
#define LED1_PIN    PORT_Pin_2
#define LED1_PORT   MDR_PORTC
#define LED1_MASK   (1UL << 2)

#define LED2_PIN    PORT_Pin_1
#define LED2_PORT   MDR_PORTA  
#define LED2_MASK   (1UL << 1)

#define LED3_PIN    PORT_Pin_5
#define LED3_PORT   MDR_PORTA
#define LED3_MASK   (1UL << 5)

#define LED4_PIN    PORT_Pin_3
#define LED4_PORT   MDR_PORTA
#define LED4_MASK   (1UL << 3)

/* Smart bit manipulation macros */
#define BIT_SET(reg, mask)      ((reg) |= (mask))
#define BIT_CLR(reg, mask)      ((reg) &= ~(mask)) 
#define BIT_TGL(reg, mask)      ((reg) ^= (mask))
#define BIT_GET(reg, mask)      (((reg) & (mask)) != 0)

/* LED direct register operations */
#define LED_REG_ON(port)        BIT_SET((port)->RXTX, mask)
#define LED_REG_OFF(port)       BIT_CLR((port)->RXTX, mask)
#define LED_REG_TOGGLE(port)    BIT_TGL((port)->RXTX, mask)

/* PWM Wave parameters */
#define DEFAULT_PWM_PERIOD  1500
#define DEFAULT_WAVE_SPEED  1


/* Function prototypes - Basic LED control */
void LED_Init(void);
void LED_On(LED_TypeDef led);
void LED_On_ms(LED_TypeDef led, uint32_t ms);
void LED_Off(LED_TypeDef led);
void LED_Toggle(LED_TypeDef led);
void LED_SetState(LED_TypeDef led, uint8_t state);
uint8_t LED_GetState(LED_TypeDef led);
void LED_AllOn(void);
void LED_AllOff(void);
void LED_Process(void);

/* Function prototypes - Sequence control */
void LED_Sequence(uint32_t delay_time);
void LED_SequenceStop(void);
uint8_t LED_SequenceIsActive(void);

/* Function prototypes - PWM Wave control */
void LED_StartPWMWave(void);
void LED_StopPWMWave(void);
void LED_SetWaveSpeed(uint32_t speed);
void LED_SetPWMPeriod(uint32_t period);
uint8_t LED_PWMWaveIsActive(void);
void LED_ProcessPWM(void);



#endif /* LEDS_H */