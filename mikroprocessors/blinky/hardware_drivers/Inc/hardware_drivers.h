#ifndef HARDWARE_DRIVERS_H
#define HARDWARE_DRIVERS_H

#include <stdint.h>
#include <stdbool.h>
#include <MDR32FxQI_rst_clk.h>
#include <MDR32FxQI_timer.h>

/* Error codes */
typedef enum {
    HD_OK      = 0,
    HD_ERROR   = 1,
    HD_BUSY    = 2,
    HD_TIMEOUT = 3
} HD_StatusTypeDef;

/* Timer interrupt handlers */
void Timer1_IRQHandler(void);

/* Delay functions */
void HD_Delay_Init(void);
HD_StatusTypeDef HD_Delay_ms(uint32_t ms);
HD_StatusTypeDef HD_Delay_us(uint32_t us);
void HD_Delay_ms_blocking(uint32_t ms);
void HD_Delay_us_blocking(uint32_t us);
uint32_t HD_GetTick(void);
void HD_IncrementTick(void);

/* Timer functions */
void HD_Timer1_Init(void);
void TIMER1_IRQHandler(void);

/* System functions */
void HD_System_Init(void);
uint32_t HD_GetSystemClock(void);

/* Utility functions */
void HD_AssertFailed(const char* file, uint32_t line);

/* Assert macro */
#ifdef DEBUG
#define HD_ASSERT(expr) ((expr) ? (void)0 : HD_AssertFailed(__FILE__, __LINE__))
#else
#define HD_ASSERT(expr) ((void)0)
#endif

#endif /* HARDWARE_DRIVERS_H */
