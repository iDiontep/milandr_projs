#include "button.h"
#include "seg7_display.h"
#include "hardware_drivers.h"
#include "leds.h"

int main(void) {
    // Инициализация системы
    HD_System_Init();
    Buttons_Init();
    app_Init();
    SEG7_Init();
    LED_Init();
    
    // Включаем глобальные прерывания
    __enable_irq();
	
    while(1) {
        Buttons_Process();
        Timer_Process();
        
        // Обработка кнопок
        Button_EventTypeDef event = Buttons_GetStartStopEvent();
        if (event == BUTTON_EVENT_PRESS) {
            Timer_StartStop();
        } else if (event == BUTTON_EVENT_HOLD) {
            Timer_Reset();
        }
        
        // Обновление дисплея
        Display_Update();
        
        // Динамическая индикация обрабатывается в прерывании TIMER1
        // SEG7_Process() вызывается автоматически из TIMER1_IRQHandler
        SEG7_DisplayTest();
        // Небольшая задержка для стабильности
        HD_Delay_ms_blocking(10);
    }
}