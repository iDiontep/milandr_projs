#include "keyboard.h"

// Карта символов на клавиатуре
const char keymap[4][4] = {
    {'1', '2', '3', 'A'},
    {'4', '5', '6', 'B'},
    {'7', '8', '9', 'C'},
    {'*', '0', '#', 'D'}
};

// Структура для хранения информации о пинах
typedef struct {
    #include "keyboard.h"

// Карта символов на клавиатуре
const char keymap[4][4];
keymap[4][4] = {{'1', '2', '3', 'A'},{'4', '5', '6', 'B'},{'7', '8', '9', 'C'}, {'*', '0', '#', 'D'}
};

// Структура для хранения информации о пинах
typedef struct {
    GPIO_TypeDef* port;
    uint32_t pin;
} PinConfig;

// Конфигурация строк
static const PinConfig row_pins[KEYPAD_ROWS] = {
    {ROW1_PORT, ROW1_PIN},
    {ROW2_PORT, ROW2_PIN},
    {ROW3_PORT, ROW3_PIN},
    {ROW4_PORT, ROW4_PIN}
};

// Конфигурация столбцов  
static const PinConfig col_pins[KEYPAD_COLS] = {
    {COL1_PORT, COL1_PIN},
    {COL2_PORT, COL2_PIN},
    {COL3_PORT, COL3_PIN},
    {COL4_PORT, COL4_PIN}
};

void Keyboard_Init(void){
    // Включение тактирования портов
    MDR_RST_CLK->PER_CLOCK |= (1 << PORTA_CLK_BIT_POS) | 
                              (1 << PORTB_CLK_BIT_POS) | 
                              (1 << PORTC_CLK_BIT_POS) |
                              (1 << PORTD_CLK_BIT_POS);
    
    // Настройка строк как выходов (активный низкий уровень)
    for (int i = 0; i < KEYPAD_ROWS; i++) {
        // Включаем выход
        MDR_UNION(row_pins[i].port)->OE_Bit.Pin_n = 1; // где n = pin
        
        // Устанавливаем в 1 (неактивное состояние)
        MDR_UNION(row_pins[i].port)->RXTX_Bit.Pin_n = 1;
        
        // Настройка режима
        MDR_UNION(row_pins[i].port)->FUNC_Bit.Pin_n = 0; // Port mode
        MDR_UNION(row_pins[i].port)->ANALOG_Bit.Pin_n = 1; // Digital
        MDR_UNION(row_pins[i].port)->PWR_Bit.Pin_n = 3; // Full speed
    }
    
    // Настройка столбцов как входов с подтяжкой к питанию
    for (int i = 0; i < KEYPAD_COLS; i++) {
        // Отключаем выход
        MDR_UNION(col_pins[i].port)->OE_Bit.Pin_n = 0;
        
        // Включаем подтяжку к питанию
        MDR_UNION(col_pins[i].port)->PULL_Bit.PULL_UP_Pin_n = 1;
        MDR_UNION(col_pins[i].port)->PULL_Bit.PULL_DOWN_Pin_n = 0;
        
        // Настройка режима
        MDR_UNION(col_pins[i].port)->FUNC_Bit.Pin_n = 0; // Port mode
        MDR_UNION(col_pins[i].port)->ANALOG_Bit.Pin_n = 1; // Digital
    }
}

void Keyboard_SetRow(uint8_t row, uint8_t state) {
    if (row >= KEYPAD_ROWS) return;
    
    if (state) {
        MDR_UNION(row_pins[row].port)->RXTX_Bit.Pin_n = 1;
    } else {
        MDR_UNION(row_pins[row].port)->RXTX_Bit.Pin_n = 0;
    }
}

uint8_t Keyboard_ReadColumn(uint8_t col) {
    if (col >= KEYPAD_COLS) return 1;
    
    // Читаем состояние столбца (0 = нажато, 1 = не нажато)
    return MDR_UNION(col_pins[col].port)->RXTX_Bit.Pin_n;
}

uint8_t Keyboard_ReadRow(uint8_t row) {
    if (row >= KEYPAD_ROWS) return 0;
    
    // Активируем строку (устанавливаем в 0)
    Keyboard_SetRow(row, 0);
    
    // Ждем стабилизации
    for (volatile int i = 0; i < 10; i++);
    
    // Читаем столбцы
    uint8_t result = 0;
    for (int col = 0; col < KEYPAD_COLS; col++) {
        // Если столбец в 0 (нажата кнопка)
        if (Keyboard_ReadColumn(col) == 0) {
            result |= (1 << col);
        }
    }
    
    // Деактивируем строку
    Keyboard_SetRow(row, 1);
    
    return result;
}

char Keyboard_Scan(void) {
    for (int row = 0; row < KEYPAD_ROWS; row++) {
        uint8_t col_data = Keyboard_ReadRow(row);
        
        if (col_data != 0) {
            for (int col = 0; col < KEYPAD_COLS; col++) {
                if (col_data & (1 << col)) {
                    // Антидребезг
                    for (volatile int i = 0; i < 1000; i++);
                    
                    // Проверяем, что кнопка все еще нажата
                    if (Keyboard_ReadRow(row) & (1 << col)) {
                        return keymap[row][col];
                    }
                }
            }
        }
    }
    
    return 0; // Ничего не нажато
}#include "keyboard.h"

// Карта символов на клавиатуре
const char keymap[4][4] = {
    {'1', '2', '3', 'A'},
    {'4', '5', '6', 'B'},
    {'7', '8', '9', 'C'},
    {'*', '0', '#', 'D'}
};

// Структура для хранения информации о пинах
typedef struct {
    GPIO_TypeDef* port;
    uint32_t pin;
} PinConfig;

// Конфигурация строк
static const PinConfig row_pins[KEYPAD_ROWS] = {
    {ROW1_PORT, ROW1_PIN},
    {ROW2_PORT, ROW2_PIN},
    {ROW3_PORT, ROW3_PIN},
    {ROW4_PORT, ROW4_PIN}
};

// Конфигурация столбцов  
static const PinConfig col_pins[KEYPAD_COLS] = {
    {COL1_PORT, COL1_PIN},
    {COL2_PORT, COL2_PIN},
    {COL3_PORT, COL3_PIN},
    {COL4_PORT, COL4_PIN}
};

void Keyboard_Init(void) {
    // Включение тактирования портов
    MDR_RST_CLK->PER_CLOCK |= (1 << PORTA_CLK_BIT_POS) | 
                              (1 << PORTB_CLK_BIT_POS) | 
                              (1 << PORTC_CLK_BIT_POS) |
                              (1 << PORTD_CLK_BIT_POS);
    
    // Настройка строк как выходов (активный низкий уровень)
    for (int i = 0; i < KEYPAD_ROWS; i++) {
        // Включаем выход
        MDR_UNION(row_pins[i].port)->OE_Bit.Pin_n = 1; // где n = pin
        
        // Устанавливаем в 1 (неактивное состояние)
        MDR_UNION(row_pins[i].port)->RXTX_Bit.Pin_n = 1;
        
        // Настройка режима
        MDR_UNION(row_pins[i].port)->FUNC_Bit.Pin_n = 0; // Port mode
        MDR_UNION(row_pins[i].port)->ANALOG_Bit.Pin_n = 1; // Digital
        MDR_UNION(row_pins[i].port)->PWR_Bit.Pin_n = 3; // Full speed
    }
    
    // Настройка столбцов как входов с подтяжкой к питанию
    for (int i = 0; i < KEYPAD_COLS; i++) {
        // Отключаем выход
        MDR_UNION(col_pins[i].port)->OE_Bit.Pin_n = 0;
        
        // Включаем подтяжку к питанию
        MDR_UNION(col_pins[i].port)->PULL_Bit.PULL_UP_Pin_n = 1;
        MDR_UNION(col_pins[i].port)->PULL_Bit.PULL_DOWN_Pin_n = 0;
        
        // Настройка режима
        MDR_UNION(col_pins[i].port)->FUNC_Bit.Pin_n = 0; // Port mode
        MDR_UNION(col_pins[i].port)->ANALOG_Bit.Pin_n = 1; // Digital
    }
}

void Keyboard_SetRow(uint8_t row, uint8_t state) {
    if (row >= KEYPAD_ROWS) return;
    
    if (state) {
        MDR_UNION(row_pins[row].port)->RXTX_Bit.Pin_n = 1;
    } else {
        MDR_UNION(row_pins[row].port)->RXTX_Bit.Pin_n = 0;
    }
}

uint8_t Keyboard_ReadColumn(uint8_t col) {
    if (col >= KEYPAD_COLS) return 1;
    
    // Читаем состояние столбца (0 = нажато, 1 = не нажато)
    return MDR_UNION(col_pins[col].port)->RXTX_Bit.Pin_n;
}

uint8_t Keyboard_ReadRow(uint8_t row) {
    if (row >= KEYPAD_ROWS) return 0;
    
    // Активируем строку (устанавливаем в 0)
    Keyboard_SetRow(row, 0);
    
    // Ждем стабилизации
    for (volatile int i = 0; i < 10; i++);
    
    // Читаем столбцы
    uint8_t result = 0;
    for (int col = 0; col < KEYPAD_COLS; col++) {
        // Если столбец в 0 (нажата кнопка)
        if (Keyboard_ReadColumn(col) == 0) {
            result |= (1 << col);
        }
    }
    
    // Деактивируем строку
    Keyboard_SetRow(row, 1);
    
    return result;
}

char Keyboard_Scan(void) {
    for (int row = 0; row < KEYPAD_ROWS; row++) {
        uint8_t col_data = Keyboard_ReadRow(row);
        
        if (col_data != 0) {
            for (int col = 0; col < KEYPAD_COLS; col++) {
                if (col_data & (1 << col)) {
                    // Антидребезг
                    for (volatile int i = 0; i < 1000; i++);
                    
                    // Проверяем, что кнопка все еще нажата
                    if (Keyboard_ReadRow(row) & (1 << col)) {
                        return keymap[row][col];
                    }
                }
            }
        }
    }
    
    return 0; // Ничего не нажато
}* port;
    uint32_t pin;
} PinConfig;

// Конфигурация строк
static const PinConfig row_pins[KEYPAD_ROWS] = {
    {ROW1_PORT, ROW1_PIN},
    {ROW2_PORT, ROW2_PIN},
    {ROW3_PORT, ROW3_PIN},
    {ROW4_PORT, ROW4_PIN}
};

// Конфигурация столбцов  
static const PinConfig col_pins[KEYPAD_COLS] = {
    {COL1_PORT, COL1_PIN},
    {COL2_PORT, COL2_PIN},
    {COL3_PORT, COL3_PIN},
    {COL4_PORT, COL4_PIN}
};

void Keyboard_Init(void) {
    // Включение тактирования портов
    MDR_RST_CLK->PER_CLOCK |= (1 << PORTA_CLK_BIT_POS) | 
                              (1 << PORTB_CLK_BIT_POS) | 
                              (1 << PORTC_CLK_BIT_POS) |
                              (1 << PORTD_CLK_BIT_POS);
    
    // Настройка строк как выходов (активный низкий уровень)
    for (int i = 0; i < KEYPAD_ROWS; i++) {
        // Включаем выход
        MDR_UNION(row_pins[i].port)->OE_Bit.Pin_n = 1; // где n = pin
        
        // Устанавливаем в 1 (неактивное состояние)
        MDR_UNION(row_pins[i].port)->RXTX_Bit.Pin_n = 1;
        
        // Настройка режима
        MDR_UNION(row_pins[i].port)->FUNC_Bit.Pin_n = 0; // Port mode
        MDR_UNION(row_pins[i].port)->ANALOG_Bit.Pin_n = 1; // Digital
        MDR_UNION(row_pins[i].port)->PWR_Bit.Pin_n = 3; // Full speed
    }
    
    // Настройка столбцов как входов с подтяжкой к питанию
    for (int i = 0; i < KEYPAD_COLS; i++) {
        // Отключаем выход
        MDR_UNION(col_pins[i].port)->OE_Bit.Pin_n = 0;
        
        // Включаем подтяжку к питанию
        MDR_UNION(col_pins[i].port)->PULL_Bit.PULL_UP_Pin_n = 1;
        MDR_UNION(col_pins[i].port)->PULL_Bit.PULL_DOWN_Pin_n = 0;
        
        // Настройка режима
        MDR_UNION(col_pins[i].port)->FUNC_Bit.Pin_n = 0; // Port mode
        MDR_UNION(col_pins[i].port)->ANALOG_Bit.Pin_n = 1; // Digital
    }
}

void Keyboard_SetRow(uint8_t row, uint8_t state) {
    if (row >= KEYPAD_ROWS) return;
    
    if (state) {
        MDR_UNION(row_pins[row].port)->RXTX_Bit.Pin_n = 1;
    } else {
        MDR_UNION(row_pins[row].port)->RXTX_Bit.Pin_n = 0;
    }
}

uint8_t Keyboard_ReadColumn(uint8_t col) {
    if (col >= KEYPAD_COLS) return 1;
    
    // Читаем состояние столбца (0 = нажато, 1 = не нажато)
    return MDR_UNION(col_pins[col].port)->RXTX_Bit.Pin_n;
}

uint8_t Keyboard_ReadRow(uint8_t row) {
    if (row >= KEYPAD_ROWS) return 0;
    
    // Активируем строку (устанавливаем в 0)
    Keyboard_SetRow(row, 0);
    
    // Ждем стабилизации
    for (volatile int i = 0; i < 10; i++);
    
    // Читаем столбцы
    uint8_t result = 0;
    for (int col = 0; col < KEYPAD_COLS; col++) {
        // Если столбец в 0 (нажата кнопка)
        if (Keyboard_ReadColumn(col) == 0) {
            result |= (1 << col);
        }
    }
    
    // Деактивируем строку
    Keyboard_SetRow(row, 1);
    
    return result;
}

char Keyboard_Scan(void) {
    for (int row = 0; row < KEYPAD_ROWS; row++) {
        uint8_t col_data = Keyboard_ReadRow(row);
        
        if (col_data != 0) {
            for (int col = 0; col < KEYPAD_COLS; col++) {
                if (col_data & (1 << col)) {
                    // Антидребезг
                    for (volatile int i = 0; i < 1000; i++);
                    
                    // Проверяем, что кнопка все еще нажата
                    if (Keyboard_ReadRow(row) & (1 << col)) {
                        return keymap[row][col];
                    }
                }
            }
        }
    }
    
    return 0; // Ничего не нажато
}