// seg7_display.c
#include "seg7_display.h"
#include "hardware_drivers.h"
#include <string.h>

// Глобальные переменные таймера
static volatile uint32_t timer_counter = 0;
static volatile uint32_t timer_seconds = 0;
static volatile bool timer_running = false;
static volatile bool timer_updated = false;

// Переменные для динамической индикации
static uint8_t current_digit = 0;
static uint8_t display_digits[4] = {0, 0, 0, 0};

void set_segments(uint8_t digit) {
    PORT_ResetBits(MDR_PORTA, PORT_Pin_7 | PORT_Pin_4 | PORT_Pin_5); // a, b, c
    PORT_ResetBits(MDR_PORTB, PORT_Pin_0 | PORT_Pin_1 | PORT_Pin_2 | PORT_Pin_5); // d, e, f, g
    if (digit > 9) digit = 0;

    switch (digit) {
        case 0:
            PORT_SetBits(MDR_PORTA, PORT_Pin_7 | PORT_Pin_4 | PORT_Pin_5); // a,b,c
            PORT_SetBits(MDR_PORTB, PORT_Pin_0 | PORT_Pin_1 | PORT_Pin_2); // d,e,f
            break;
        case 1:
            PORT_SetBits(MDR_PORTA, PORT_Pin_4 | PORT_Pin_5); // b,c
            break;
        case 2:
            PORT_SetBits(MDR_PORTA, PORT_Pin_7 | PORT_Pin_4); // a,b
            PORT_SetBits(MDR_PORTB, PORT_Pin_0 | PORT_Pin_2 | PORT_Pin_5); // d,e,g
            break;
        case 3:
            PORT_SetBits(MDR_PORTA, PORT_Pin_7 | PORT_Pin_4 | PORT_Pin_5); // a,b,c
            PORT_SetBits(MDR_PORTB, PORT_Pin_0 | PORT_Pin_5); // d,g
            break;
        case 4:
            PORT_SetBits(MDR_PORTA, PORT_Pin_4 | PORT_Pin_5); // b,c
            PORT_SetBits(MDR_PORTB, PORT_Pin_1 | PORT_Pin_5); // f,g
            break;
        case 5:
            PORT_SetBits(MDR_PORTA, PORT_Pin_7 | PORT_Pin_5); // a,c
            PORT_SetBits(MDR_PORTB, PORT_Pin_0 | PORT_Pin_1 | PORT_Pin_5); // d,f,g
            break;
        case 6:
            PORT_SetBits(MDR_PORTA, PORT_Pin_7 | PORT_Pin_5); // a,c
            PORT_SetBits(MDR_PORTB, PORT_Pin_0 | PORT_Pin_1 | PORT_Pin_2 | PORT_Pin_5); // d,e,f,g
            break;
        case 7:
            PORT_SetBits(MDR_PORTA, PORT_Pin_7 | PORT_Pin_4 | PORT_Pin_5); // a,b,c
            break;
        case 8:
            PORT_SetBits(MDR_PORTA, PORT_Pin_7 | PORT_Pin_4 | PORT_Pin_5); // a,b,c
            PORT_SetBits(MDR_PORTB, PORT_Pin_0 | PORT_Pin_1 | PORT_Pin_2 | PORT_Pin_5); // d,e,f,g
            break;
        case 9:
            PORT_SetBits(MDR_PORTA, PORT_Pin_7 | PORT_Pin_4 | PORT_Pin_5); // a,b,c
            PORT_SetBits(MDR_PORTB, PORT_Pin_0 | PORT_Pin_1 | PORT_Pin_5); // d,f,g
            break;
    }
}

void select_digit(uint8_t n) {
    PORT_ResetBits(MDR_PORTC, PORT_Pin_0 | PORT_Pin_1); // HG1, HG2
    PORT_ResetBits(MDR_PORTE, PORT_Pin_0);              // HG3
    PORT_ResetBits(MDR_PORTF, PORT_Pin_1);              // HG4
    switch (n) {
        case 0: PORT_SetBits(MDR_PORTC, PORT_Pin_0); break; // HG1
        case 1: PORT_SetBits(MDR_PORTC, PORT_Pin_1); break; // HG2
        case 2: PORT_SetBits(MDR_PORTE, PORT_Pin_0); break; // HG3
        case 3: PORT_SetBits(MDR_PORTF, PORT_Pin_1); break; // HG4
    }
}

void SEG7_Init(void) {
    PORT_InitTypeDef port_init;
    
    // Включаем тактирование портов
    RST_CLK_PCLKcmd(RST_CLK_PCLK_PORTA, ENABLE);
    RST_CLK_PCLKcmd(RST_CLK_PCLK_PORTB, ENABLE);
    RST_CLK_PCLKcmd(RST_CLK_PCLK_PORTC, ENABLE);
    RST_CLK_PCLKcmd(RST_CLK_PCLK_PORTE, ENABLE);
    RST_CLK_PCLKcmd(RST_CLK_PCLK_PORTF, ENABLE);
    
    // Настройка сегментов (порты A и B)
    PORT_StructInit(&port_init);
    port_init.PORT_Pin = PORT_Pin_7 | PORT_Pin_4 | PORT_Pin_5;
    port_init.PORT_OE = PORT_OE_OUT;
    port_init.PORT_FUNC = PORT_FUNC_PORT;
    port_init.PORT_MODE = PORT_MODE_DIGITAL;
    port_init.PORT_PULL_UP = PORT_PULL_UP_OFF;
    port_init.PORT_PULL_DOWN = PORT_PULL_DOWN_OFF;
    PORT_Init(MDR_PORTA, &port_init);
    
    port_init.PORT_Pin = PORT_Pin_0 | PORT_Pin_1 | PORT_Pin_2 | PORT_Pin_5;
    PORT_Init(MDR_PORTB, &port_init);
    
    // Настройка разрядов (порты C, E, F)
    port_init.PORT_Pin = PORT_Pin_0 | PORT_Pin_1;
    PORT_Init(MDR_PORTC, &port_init);
    
    port_init.PORT_Pin = PORT_Pin_0;
    PORT_Init(MDR_PORTE, &port_init);
    
    port_init.PORT_Pin = PORT_Pin_1;
    PORT_Init(MDR_PORTF, &port_init);
    
    // Инициализация переменных дисплея
    memset(display_digits, 0, sizeof(display_digits));
    current_digit = 0;
}

void SEG7_Process(void) {
    // Гасим предыдущий разряд
    select_digit(4); // Несуществующий разряд - гасим все
    
    // Устанавливаем сегменты для текущего разряда
    set_segments(display_digits[current_digit]);
    
    // Включаем текущий разряд
    select_digit(current_digit);
    
    // Переходим к следующему разряду
    current_digit = (current_digit + 1) % 4;
}

void Timer_Init(void) {
    timer_counter = 0;
    timer_seconds = 0;
    timer_running = false;
    timer_updated = false;
}

void Timer_Process(void) {
    static uint32_t last_tick = 0;
    uint32_t current_tick = HD_GetTick();
    
    if (timer_running && (current_tick - last_tick >= 100)) { // 100 мс
        timer_counter++;
        last_tick = current_tick;
        
        if (timer_counter >= 10) { // 1 секунда
            timer_counter = 0;
            timer_seconds++;
            timer_updated = true;
            
            // Ограничение 99:59
            if (timer_seconds >= 6000) {
                timer_seconds = 0;
            }
        }
    }
}

void Timer_StartStop(void) {
    timer_running = !timer_running;
}

void Timer_Reset(void) {
    timer_seconds = 0;
    timer_counter = 0;
    timer_updated = true;
}

void Display_Update(void) {
    if (timer_updated) {
        uint32_t minutes = timer_seconds / 60;
        uint32_t seconds = timer_seconds % 60;
        
        // Преобразуем время в цифры для отображения
        display_digits[0] = minutes / 10;        // Десятки минут
        display_digits[1] = minutes % 10;        // Единицы минут
        display_digits[2] = seconds / 10;        // Десятки секунд
        display_digits[3] = seconds % 10;        // Единицы секунд
        
        timer_updated = false;
    }
}


void SEG7_DisplayTest(void) {
    // Отображаем 1 на первом разряде
    select_digit(0);
    set_segments(1);
    HD_Delay_ms_blocking(5);
    
    // Отображаем 2 на втором разряде
    select_digit(1);
    set_segments(2);
    HD_Delay_ms_blocking(5);
    
    // Отображаем 3 на третьем разряде
    select_digit(2);
    set_segments(3);
    HD_Delay_ms_blocking(5);
    
    // Отображаем 4 на четвертом разряде
    select_digit(3);
    set_segments(4);
    HD_Delay_ms_blocking(5);
}

void SEG7_DisplayNumber(uint16_t number) {
    static uint8_t current_digit = 0;
    static uint8_t digits[4] = {1, 2, 3, 4}; // По умолчанию 1234
    
    // Разбираем число на цифры, если нужно
    if (number <= 9999) {
        digits[0] = number / 1000;
        digits[1] = (number % 1000) / 100;
        digits[2] = (number % 100) / 10;
        digits[3] = number % 10;
    }
    
    // Гасим предыдущий разряд
    select_digit(4); // Несуществующий разряд
    
    // Устанавливаем сегменты для текущей цифры
    set_segments(digits[current_digit]);
    
    // Включаем текущий разряд
    select_digit(current_digit);
    
    // Переходим к следующему разряду
    current_digit = (current_digit + 1) % 4;
}