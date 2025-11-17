#include "button.h"
#include "seg7_display.h"
#include "hardware_drivers.h"

int main(void) {
    // Инициализация системы
    HD_System_Init();
    HD_Delay_Init();
    SEG7_Init();
    
    // Простой тест - статическое отображение 1234
    while(1) {
        // Вариант 1: Статическое отображение (поочередно включаем разряды)
        SEG7_DisplayTest();
        
        // Или вариант 2: Используем динамическую индикацию
        // SEG7_DisplayNumber(1234);
        // HD_Delay_ms_blocking(1);
    }
}
	
    /*
	int main(void) {
    // Инициализация системы
    HD_System_Init();
    Buttons_Init();
    Timer_Init();
    SEG7_Init();
    HD_Timer1_Init(); // Для динамической индикации
	
	
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
        
        // Небольшая задержка для стабильности
        HD_Delay_ms_blocking(10);
    }
		}
*/		

