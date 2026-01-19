// seg7_display.c
#include "seg7_display.h"
#include "hardware_drivers.h"
#include <string.h>

// ���������� ���������� �������
static volatile uint32_t timer_counter = 0;
static volatile uint32_t timer_seconds = 0;
static volatile bool timer_running = 1;
static volatile bool timer_updated = 1;

// ���������� ��� ������������ ���������
static uint8_t current_digit = 0;
static uint8_t display_digits[4] = {0, 0, 0, 0};

// Буфер для отображения символов (используется в режиме замка)
char displayBuf[4] = {' ', ' ', ' ', ' '};

// Таблицы для отображения символов на 7-сегментном индикаторе
// Формат: {a, b, c, d, e, f, g, dp}
static const uint8_t digitTable[10][8] = {
    {1,1,1,1,1,1,0,0}, // 0
    {0,1,1,0,0,0,0,0}, // 1
    {1,1,0,1,1,0,1,0}, // 2
    {1,1,1,1,0,0,1,0}, // 3
    {0,1,1,0,0,1,1,0}, // 4
    {1,0,1,1,0,1,1,0}, // 5
    {1,0,1,1,1,1,1,0}, // 6
    {1,1,1,0,0,0,0,0}, // 7
    {1,1,1,1,1,1,1,0}, // 8
    {1,1,1,1,0,1,1,0}  // 9
};

static const uint8_t alphaTable[26][8] = {
    {1,1,1,0,1,1,1,0}, // A
    {0,0,1,1,1,1,1,0}, // B
    {1,0,0,1,1,1,0,0}, // C
    {0,1,1,1,1,0,1,0}, // D
    {1,0,0,1,1,1,1,0}, // E
    {1,0,0,0,1,1,1,0}, // F
    {1,0,1,1,1,1,1,0}, // G
    {0,1,1,0,1,1,1,0}, // H
    {0,0,0,0,1,1,0,0}, // I
    {0,1,1,1,1,0,0,0}, // J
    {0,0,0,0,0,0,0,0}, // K (не поддерживается)
    {0,0,0,1,1,1,0,0}, // L
    {0,0,0,0,0,0,0,0}, // M (не поддерживается)
    {0,0,1,0,1,0,1,0}, // n
    {1,1,1,1,1,1,0,0}, // O
    {1,1,0,0,1,1,1,0}, // P
    {0,0,0,0,0,0,0,0}, // Q (не поддерживается)
    {0,0,0,0,1,0,1,0}, // r
    {1,0,1,1,0,1,1,0}, // S (как 5)
    {0,0,0,1,1,1,1,0}, // t (не поддерживается)
    {0,0,0,0,0,0,0,0}, // U (не поддерживается)
    {0,0,0,0,0,0,0,0}, // V (не поддерживается)
    {0,0,0,0,0,0,0,0}, // W (не поддерживается)
    {0,0,0,0,0,0,0,0}, // X (не поддерживается)
    {0,0,0,0,0,0,0,0}, // Y (не поддерживается)
    {1,1,0,1,1,0,1,0}  // Z (как 2)
};

static const uint8_t specialTable[2][8] = {
    {0,0,0,0,0,0,1,0}, // -
    {0,0,0,1,0,0,0,0}  // _
};

// Функция для установки сегментов по таблице данных
static void set_segments_from_table(const uint8_t* data) {
    // Сброс всех сегментов
    PORT_ResetBits(MDR_PORTA, PORT_Pin_7 | PORT_Pin_4 | PORT_Pin_5); // a, b, c
    PORT_ResetBits(MDR_PORTB, PORT_Pin_0 | PORT_Pin_1 | PORT_Pin_2 | PORT_Pin_5 | PORT_Pin_6); // d, e, f, g, dp
    
    // Установка сегментов согласно таблице
    // a = PORT_Pin_7 (PORTA)
    if (data[0]) PORT_SetBits(MDR_PORTA, PORT_Pin_7);
    // b = PORT_Pin_4 (PORTA)
    if (data[1]) PORT_SetBits(MDR_PORTA, PORT_Pin_4);
    // c = PORT_Pin_5 (PORTA)
    if (data[2]) PORT_SetBits(MDR_PORTA, PORT_Pin_5);
    // d = PORT_Pin_0 (PORTB)
    if (data[3]) PORT_SetBits(MDR_PORTB, PORT_Pin_0);
    // e = PORT_Pin_2 (PORTB)
    if (data[4]) PORT_SetBits(MDR_PORTB, PORT_Pin_2);
    // f = PORT_Pin_1 (PORTB)
    if (data[5]) PORT_SetBits(MDR_PORTB, PORT_Pin_1);
    // g = PORT_Pin_5 (PORTB)
    if (data[6]) PORT_SetBits(MDR_PORTB, PORT_Pin_5);
    // dp = PORT_Pin_6 (PORTB)
    if (data[7]) PORT_SetBits(MDR_PORTB, PORT_Pin_6);
}

// Функция для отображения символа
void SEG7_ShowSymbol(char sym) {
    const uint8_t* data = NULL;
    
    if (sym >= '0' && sym <= '9') {
        data = digitTable[sym - '0'];
    } else if (sym >= 'A' && sym <= 'Z') {
        data = alphaTable[sym - 'A'];
    } else if (sym >= 'a' && sym <= 'z') {
        data = alphaTable[sym - 'a'];
    } else if (sym == '*') {
        data = specialTable[0]; // '-'
    } else if (sym == '#') {
        data = specialTable[1]; // '_'
    } else if (sym == ' ') {
        // Пробел - все сегменты выключены
        PORT_ResetBits(MDR_PORTA, PORT_Pin_7 | PORT_Pin_4 | PORT_Pin_5);
        PORT_ResetBits(MDR_PORTB, PORT_Pin_0 | PORT_Pin_1 | PORT_Pin_2 | PORT_Pin_5 | PORT_Pin_6);
        return;
    } else {
        // Неизвестный символ - пробел
        PORT_ResetBits(MDR_PORTA, PORT_Pin_7 | PORT_Pin_4 | PORT_Pin_5);
        PORT_ResetBits(MDR_PORTB, PORT_Pin_0 | PORT_Pin_1 | PORT_Pin_2 | PORT_Pin_5 | PORT_Pin_6);
        return;
    }
    
    if (data) {
        set_segments_from_table(data);
    }
}

// Старая функция для совместимости (только цифры)
void set_segments(uint8_t digit) {
    if (digit > 9) digit = 0;
    const uint8_t* data = digitTable[digit];
    set_segments_from_table(data);
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
    
    /* Enable clock for all used ports */
    RST_CLK_PCLKcmd(RST_CLK_PCLK_PORTA, ENABLE);
    RST_CLK_PCLKcmd(RST_CLK_PCLK_PORTB, ENABLE);
    RST_CLK_PCLKcmd(RST_CLK_PCLK_PORTC, ENABLE);
    RST_CLK_PCLKcmd(RST_CLK_PCLK_PORTE, ENABLE);
    RST_CLK_PCLKcmd(RST_CLK_PCLK_PORTF, ENABLE);

    /* Configure segment pins as outputs */
    PORT_StructInit(&port_init);
    port_init.PORT_Pin = SEG7_SEG_A_PIN | SEG7_SEG_B_PIN | SEG7_SEG_C_PIN;
    port_init.PORT_OE = PORT_OE_OUT;
    port_init.PORT_FUNC = PORT_FUNC_PORT;
    port_init.PORT_MODE = PORT_MODE_DIGITAL;
    port_init.PORT_SPEED = PORT_SPEED_FAST;
    port_init.PORT_PD = PORT_PD_DRIVER;
    PORT_Init(SEG7_SEG_A_PORT, &port_init);

    PORT_StructInit(&port_init);
    port_init.PORT_Pin = SEG7_SEG_D_PIN | SEG7_SEG_E_PIN | SEG7_SEG_F_PIN | 
                         SEG7_SEG_G_PIN | SEG7_SEG_DP_PIN;
    port_init.PORT_OE = PORT_OE_OUT;
    port_init.PORT_FUNC = PORT_FUNC_PORT;
    port_init.PORT_MODE = PORT_MODE_DIGITAL;
    port_init.PORT_SPEED = PORT_SPEED_FAST;
    port_init.PORT_PD = PORT_PD_DRIVER;
    PORT_Init(SEG7_SEG_D_PORT, &port_init);

    /* Configure digit selection pins as outputs */
    PORT_StructInit(&port_init);
    port_init.PORT_Pin = SEG7_DIGIT_1_PIN | SEG7_DIGIT_2_PIN;
    port_init.PORT_OE = PORT_OE_OUT;
    port_init.PORT_FUNC = PORT_FUNC_PORT;
    port_init.PORT_MODE = PORT_MODE_DIGITAL;
    port_init.PORT_SPEED = PORT_SPEED_FAST;
    port_init.PORT_PD = PORT_PD_DRIVER;
    PORT_Init(SEG7_DIGIT_1_PORT, &port_init);

    PORT_StructInit(&port_init);
    port_init.PORT_Pin = SEG7_DIGIT_3_PIN;
    port_init.PORT_OE = PORT_OE_OUT;
    port_init.PORT_FUNC = PORT_FUNC_PORT;
    port_init.PORT_MODE = PORT_MODE_DIGITAL;
    port_init.PORT_SPEED = PORT_SPEED_FAST;
    port_init.PORT_PD = PORT_PD_DRIVER;
    PORT_Init(SEG7_DIGIT_3_PORT, &port_init);

    PORT_StructInit(&port_init);
    port_init.PORT_Pin = SEG7_DIGIT_4_PIN;
    port_init.PORT_OE = PORT_OE_OUT;
    port_init.PORT_FUNC = PORT_FUNC_PORT;
    port_init.PORT_MODE = PORT_MODE_DIGITAL;
    port_init.PORT_SPEED = PORT_SPEED_FAST;
    port_init.PORT_PD = PORT_PD_DRIVER;
    PORT_Init(SEG7_DIGIT_4_PORT, &port_init);
    
    // ������������� ���������� �������
    memset(display_digits, 0, sizeof(display_digits));
    current_digit = 0;
}

void SEG7_Process(void) {
    static uint32_t last_update = 0;
    uint32_t current_time = HD_GetTick();
    
    // Обновление каждые 5мс для мультиплексирования
    if ((current_time - last_update) >= 5) {
        last_update = current_time;
        
        // Отображение символа из буфера
        SEG7_ShowSymbol(displayBuf[current_digit]);
        
        // Выбор текущей цифры
        select_digit(current_digit);
        
        // Переход к следующей цифре
        current_digit = (current_digit + 1) % 4;
    }
}

void app_Init(void) {
    timer_counter = 0;
    timer_seconds = 0;
    timer_running = true;
    timer_updated = true;
	

}

void Timer_Process(void) {
    static uint32_t last_tick = 0;
    uint32_t current_tick = HD_GetTick();
    
    if (timer_running) {
        if ((current_tick - last_tick) >= 100) { // 100 ��
            timer_counter++;
            last_tick = current_tick;
            
            if (timer_counter >= 10) { // 1 ������� (10 * 100�� = 1000��)
                timer_counter = 0;
                timer_seconds++;
                timer_updated = true;
								Display_Update();
                
                // ����������� 99:59
                if (timer_seconds >= 6000) { // 100 ����� * 60 ������
                    timer_seconds = 0;
                }
            }
        }
    } else {
        // ���� ������ �� ��������, ��������� last_tick ��� ����������� �������
        last_tick = current_tick;
    }
}

void Timer_StartStop(void) {
    timer_running = !timer_running;
    timer_updated = true; // �������������� ���������� ������� ��� ��������� ���������
}

void Timer_Reset(void) {
    timer_seconds = 0;
    timer_counter = 0;
    timer_running = false;
    timer_updated = true;
}

void Display_Update(void) {
    if (timer_updated) {
        uint32_t minutes = timer_seconds / 60;
        uint32_t seconds = timer_seconds % 60;
        
        // ����������� ����� � ����� ��� �����������
        display_digits[0] = minutes / 10;        // ������� �����
        display_digits[1] = minutes % 10;        // ������� �����
        display_digits[2] = seconds / 10;        // ������� ������
        display_digits[3] = seconds % 10;        // ������� ������
        
        timer_updated = false;
    }
}

void SEG7_DisplayTest(void) {
    // ���������� 1 �� ������ �������
    select_digit(0);
    set_segments(display_digits[0]);
    HD_Delay_ms_blocking(1);
    
    // ���������� 2 �� ������ �������
    select_digit(1);
    set_segments(display_digits[1]);
    HD_Delay_ms_blocking(1);
    
    // ���������� 3 �� ������� �������
    select_digit(2);
    set_segments(display_digits[2]);
    HD_Delay_ms_blocking(1);
    
    // ���������� 4 �� ��������� �������
    select_digit(3);
    set_segments(display_digits[3]);
    HD_Delay_ms_blocking(1);
}

void SEG7_DisplayNumber(uint16_t number) {
    static uint8_t current_digit = 0;
    static uint8_t digits[4] = {1, 2, 3, 4}; // �� ��������� 1234
    
    // ��������� ����� �� �����, ���� �����
    if (number <= 9999) {
        digits[0] = number / 1000;
        digits[1] = (number % 1000) / 100;
        digits[2] = (number % 100) / 10;
        digits[3] = number % 10;
    }
   
    
    // ������������� �������� ��� ������� �����
    set_segments(digits[current_digit]);
    
    // �������� ������� ������
    select_digit(current_digit);
    
    // ��������� � ���������� �������
    current_digit = (current_digit + 1) % 4;
		
		HD_Delay_ms_blocking(1);
}

// Функция для добавления символа в буфер дисплея (сдвиг)
void SEG7_PushSymbol(char sym) {
    for (int i = 3; i > 0; i--) {
        displayBuf[i] = displayBuf[i - 1];
    }
    displayBuf[0] = sym;
}

// Функция для установки всего буфера
void SEG7_SetBuffer(const char* str) {
    for (int i = 0; i < 4; i++) {
        if (str[i] != '\0') {
            displayBuf[i] = str[i];
        } else {
            displayBuf[i] = ' ';
        }
    }
}

// Функция для очистки буфера
void SEG7_ClearBuffer(void) {
    for (int i = 0; i < 4; i++) {
        displayBuf[i] = ' ';
    }
}