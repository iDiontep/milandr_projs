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
    /* Enable clock for TIMER1 */
    RST_CLK_PCLKcmd(RST_CLK_PCLK_TIMER1, ENABLE);
    
    /* Configure timer clock - аналогично первому коду */
    MDR_RST_CLK->TIM_CLOCK &= ~RST_CLK_TIM_CLOCK_TIM1_BRG_Msk;
    MDR_RST_CLK->TIM_CLOCK |= (0 << RST_CLK_TIM_CLOCK_TIM1_BRG_Pos) | RST_CLK_TIM_CLOCK_TIM1_CLK_EN;

    /* Прямая работа с регистрами по образцу первого кода */
    MDR_TIMER1->CNT = 0;
    MDR_TIMER1->PSG = 0;
    MDR_TIMER1->ARR = (system_clock / 1000) - 1; /* 1ms period */
    
    /* Настройка основного контроллера таймера */
    MDR_TIMER1->CNTRL = (0x0 << TIMER_CNTRL_FDTS_Pos) | TIMER_CNTRL_CNT_EN;
    
    /* Отключаем каналы, т.к. они не используются для обработки LED */
    MDR_TIMER1->CH1_CNTRL = 0;
    MDR_TIMER1->CH2_CNTRL = 0;
    MDR_TIMER1->CH3_CNTRL = 0;
    MDR_TIMER1->CH4_CNTRL = 0;
    
    /* Дополнительные регистры управления каналами */
    MDR_TIMER1->CH1_CNTRL1 = 0;
    MDR_TIMER1->CH2_CNTRL1 = 0;
    MDR_TIMER1->CH3_CNTRL1 = 0;
    MDR_TIMER1->CH4_CNTRL1 = 0;
    
    /* Регистры управления внешними событиями и brake */
    MDR_TIMER1->BRKETR_CNTRL = 0;
    MDR_TIMER1->DMA_RE = 0;

    /* Включаем прерывание по событию переполнения (ARR) */
    MDR_TIMER1->IE = TIMER_IE_BRK_EVENT_IE;


    /* Настройка прерывания */
    NVIC_SetPriority(Timer1_IRQn, 1);
    NVIC_EnableIRQ(Timer1_IRQn);
    
    /* Таймер уже запущен через установку TIMER_CNTRL_CNT_EN выше */
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
        __NOP();
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