#include "hardware_drivers.h"
#include "main.h"
#include "leds.h"
#include "MDR32FxQI_rst_clk.h"
#include "MDR32FxQI_port.h"
#include "MDR32FxQI_timer.h"

/* Private variables */
static volatile uint32_t tick_counter = 0;
static uint32_t system_clock = 8000000; /* Default 8 MHz */

/* SysTick interrupt handler */
void SysTick_Handler(void)
{
    HD_IncrementTick();
}

/* TIMER1 interrupt handler for LED processing */
void Timer1_IRQHandler(void)
{
    if (TIMER_GetITStatus(MDR_TIMER1, TIMER_STATUS_CNT_ARR)) {
        TIMER_ClearITPendingBit(MDR_TIMER1, TIMER_STATUS_CNT_ARR);
        
        /* Call LED process function */
        LED_Process();
    }
}

/**
  * @brief  Initialize TIMER1 for LED processing
  * @param  None
  * @retval None
  */
void HD_Timer1_Init(void)
{
    TIMER_CntInitTypeDef timerInit;
    
    /* Enable clock for TIMER1 */
    RST_CLK_PCLKcmd(RST_CLK_PCLK_TIMER1, ENABLE);
    
    /* Configure timer for 1kHz frequency (1ms period) */
    TIMER_CntStructInit(&timerInit);
    timerInit.TIMER_IniCounter = 0;
    timerInit.TIMER_Period = (system_clock / 1000) - 1; /* 1ms period */
    timerInit.TIMER_Prescaler = 0;
    timerInit.TIMER_CounterMode = TIMER_CntDir_Up;
    timerInit.TIMER_CounterDirection = TIMER_CntDir_Up;
    timerInit.TIMER_EventSource = 0;
    timerInit.TIMER_FilterSampling = TIMER_FDTS_TIMER_CLK_div_1;
    timerInit.TIMER_ARR_UpdateMode = TIMER_ARR_Update_Immediately;
    timerInit.TIMER_ETR_FilterConf = TIMER_Filter_1FF_at_TIMER_CLK;
    timerInit.TIMER_ETR_Prescaler = TIMER_ETR_Prescaler_None;
    timerInit.TIMER_ETR_Polarity = TIMER_BRKPolarity_NonInverted;
    timerInit.TIMER_BRK_Polarity = TIMER_BRKPolarity_NonInverted;
    
    TIMER_CntInit(MDR_TIMER1, &timerInit);
    
    /* Enable TIMER1 interrupt */
    TIMER_ITConfig(MDR_TIMER1, TIMER_STATUS_CNT_ARR, ENABLE);
    
    /* Configure NVIC for TIMER1 */
    NVIC_EnableIRQ(Timer1_IRQn);
    NVIC_SetPriority(Timer1_IRQn, 1);
    
    /* Start TIMER1 */
    TIMER_Cmd(MDR_TIMER1, ENABLE);
}

/**
  * @brief  Initialize delay functionality using SysTick timer
  * @param  None
  * @retval HD_StatusTypeDef
  */
void HD_Delay_Init(void)
{
    /* Configure SysTick to generate interrupt every 1ms */
    if (SysTick_Config(system_clock / 1000)) {
        /* Capture error */
        while (1);
    }
    
    /* Set SysTick priority */
    NVIC_SetPriority(SysTick_IRQn, 0);
}

/**
  * @brief  Non-blocking delay in milliseconds
  * @param  ms: delay in milliseconds
  * @retval HD_StatusTypeDef
  */
HD_StatusTypeDef HD_Delay_ms(uint32_t ms)
{
    static uint32_t start_tick = 0;
    static bool delay_running = false;
    
    if (!delay_running) {
        start_tick = HD_GetTick();
        delay_running = true;
        return HD_BUSY;
    }
    
    if ((HD_GetTick() - start_tick) >= ms) {
        delay_running = false;
        return HD_OK;
    }
    
    return HD_BUSY;
}

/**
  * @brief  Non-blocking delay in microseconds
  * @param  us: delay in microseconds
  * @retval HD_StatusTypeDef
  */
HD_StatusTypeDef HD_Delay_us(uint32_t us)
{
    static uint32_t start_tick = 0;
    static bool delay_running = false;
    uint32_t ticks_needed = (us * (system_clock / 1000000)) / 1000;
    
    if (!delay_running) {
        start_tick = HD_GetTick();
        delay_running = true;
        return HD_BUSY;
    }
    
    if ((HD_GetTick() - start_tick) >= ticks_needed) {
        delay_running = false;
        return HD_OK;
    }
    
    return HD_BUSY;
}

/**
  * @brief  Blocking delay in milliseconds
  * @param  ms: delay in milliseconds
  * @retval None
  */
void HD_Delay_ms_blocking(uint32_t ms)
{
    uint32_t start_tick = HD_GetTick();
    
    while ((HD_GetTick() - start_tick) < ms) {
        /* Wait */
    }
}

/**
  * @brief  Blocking delay in microseconds
  * @param  us: delay in microseconds
  * @retval None
  */
void HD_Delay_us_blocking(uint32_t us)
{
    /* Calculate number of cycles for approximate delay */
    uint32_t cycles = (us * (system_clock / 1000000)) / 3;
    
    for (volatile uint32_t i = 0; i < cycles; i++) {
        __NOP();
    }
}

/**
  * @brief  Get current tick count
  * @param  None
  * @retval Current tick value
  */
uint32_t HD_GetTick(void)
{
    return tick_counter;
}

/**
  * @brief  Increment tick counter (called from SysTick interrupt)
  * @param  None
  * @retval None
  */
void HD_IncrementTick(void)
{
    tick_counter++;
}

/**
  * @brief  Initialize system hardware
  * @param  None
  * @retval None
  */
void HD_System_Init(void)
{
		/* Register init */
		MDR_RST_CLK->PER_CLOCK |= (0x01 <<23);
		MDR_PORTC->RXTX &= ~(0x01 <<2);
		MDR_PORTC->OE |= (0x01 <<2);
		MDR_PORTC-> FUNC &= ~(0x03 << 2*2);
		MDR_PORTC->ANALOG |= ~(0x01 <<2);
		MDR_PORTC-> PULL &= ~(0x01 << (2));
		MDR_PORTC-> PULL &= ~(0x01 << (2+16));
		MDR_PORTC-> PD &= ~(0x01 << (2));
		MDR_PORTC-> PD &= ~(0x01 << (2+16));
		MDR_PORTC-> PWR  |= (0x01 << 2*2);
		MDR_PORTC-> GFEN  &= ~(0x01 << (2));
    /* Initialize delay system */
    HD_Delay_Init();
    
    /* Initialize TIMER1 for LED processing */
    HD_Timer1_Init();
    
    /* Additional hardware initialization can be added here */
}

/**
  * @brief  Get system clock frequency
  * @param  None
  * @retval System clock frequency in Hz
  */
uint32_t HD_GetSystemClock(void)
{
    return system_clock;
}

/**
  * @brief  Assert failure handler
  * @param  file: Source file name where assert failed
  * @param  line: Line number where assert failed
  * @retval None
  */
void HD_AssertFailed(const char* file, uint32_t line)
{
    /* User can add custom assert handling here */
    /* For example: turn on error LED, log error, etc. */
    
    /* Infinite loop for debugging */
    while (1) {
        /* Add breakpoint here for debugging */
        __NOP();
    }
}