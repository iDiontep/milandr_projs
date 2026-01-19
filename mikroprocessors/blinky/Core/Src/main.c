#include "MDR32F9Q2I.h"
#include "MDR32FxQI_rst_clk.h"
#include "MDR32FxQI_port.h"
#include "MDR32FxQI_timer.h"
#include "hardware_drivers.h"
#include "leds.h"
#include "button.h"
#include "keyboard_driver.h"
#include "seg7_display.h"
#include <string.h>

/* Максимальная длина кода */
#define CODE_LENGTH 6
#define MAX_INPUT_BUFFER 20
#define MAX_WRONG_ATTEMPTS 3
#define KEYPAD_LOCK_TIMEOUT_MS 60000  // 1 минута
#define LONG_PRESS_MS 1000
#define SHORT_PRESS_MS 50

/* Состояния системы */
typedef enum {
    STATE_LOCKED = 0,           // Замок закрыт, ожидание ввода кода
    STATE_UNLOCKED = 1,         // Замок открыт
    STATE_PROGRAMMING = 2,      // Режим программирования кода
    STATE_KEYPAD_LOCKED = 3,    // Клавиатура заблокирована (после 20 символов)
    STATE_TIMEOUT_LOCKED = 4    // Клавиатура заблокирована (после 3 ошибок)
} SystemState_t;

/* Глобальные переменные */
static char master_code[CODE_LENGTH + 1] = "112233";  // Код по умолчанию
static char input_buffer[CODE_LENGTH + 1] = "";
static char full_input_buffer[MAX_INPUT_BUFFER + 1] = "";
static uint8_t input_index = 0;
static uint8_t full_buffer_index = 0;
static SystemState_t current_state = STATE_LOCKED;
static uint32_t wrong_attempts = 0;
static uint32_t lock_timer = 0;
static uint8_t is_keypad_locked = 0;
static uint32_t keypad_lock_start = 0;

/* Прототипы функций */
void process_keypad(char key);
void process_button(Button_EventTypeDef event);
void update_leds(void);
void check_code(void);
void clear_input_buffer(void);
void lock_keypad_temporary(void);
void unlock_keypad(void);
void reset_wrong_attempts(void);
void init_locker_system(void);
char convert_key_to_display(char key);

/* Преобразование символов для отображения */
char convert_key_to_display(char key) {
    if (key >= '0' && key <= '9') return key;
    if (key >= 'A' && key <= 'F') return key;
    if (key == '*') return '-';
    if (key == '#') return '_';
    return ' ';
}

/* Инициализация системы */
void init_locker_system(void) {
    // Отключение прерываний на время инициализации
    __disable_irq();
    
    // Инициализация периферии
    HD_System_Init();
    HD_Delay_Init();
    HD_Timer1_Init();
    
    // Инициализация компонентов
    LED_Init();
    Buttons_Init();
    Keyboard_Init();
    SEG7_Init();
    
    // Включение прерываний после завершения инициализации
    __enable_irq();
    
    // Начальное состояние
    current_state = STATE_LOCKED;
    clear_input_buffer();
    reset_wrong_attempts();
    is_keypad_locked = 0;
    
    // Инициализация дисплея
    SEG7_ClearBuffer();
    
    // Включаем светодиоды по состоянию
    update_leds();
}

/* Обработка нажатий клавиатуры */
void process_keypad(char key) {
    // Проверка блокировки клавиатуры
    if (is_keypad_locked) {
        if ((HD_GetTick() - keypad_lock_start) >= KEYPAD_LOCK_TIMEOUT_MS) {
            unlock_keypad();
        } else {
            return; // Клавиатура заблокирована
        }
    }
    
    // Блокировка по таймауту
    if (current_state == STATE_TIMEOUT_LOCKED) {
        if ((HD_GetTick() - lock_timer) >= KEYPAD_LOCK_TIMEOUT_MS) {
            current_state = STATE_LOCKED;
            reset_wrong_attempts();
            update_leds();
            SEG7_ClearBuffer();
        }
        return;
    }
    
    // Обработка в зависимости от состояния
    switch (current_state) {
        case STATE_LOCKED:
            // Ввод кода для разблокировки
            if ((key >= '0' && key <= '9') || (key >= 'A' && key <= 'F')) {
                if (input_index < CODE_LENGTH) {
                    input_buffer[input_index++] = key;
                    input_buffer[input_index] = '\0';
                    
                    // Добавляем в полный буфер для проверки блокировки
                    if (full_buffer_index < MAX_INPUT_BUFFER) {
                        full_input_buffer[full_buffer_index++] = key;
                        full_input_buffer[full_buffer_index] = '\0';
                    }
                    
                    // Проверка на блокировку после 20 символов
                    if (full_buffer_index >= MAX_INPUT_BUFFER) {
                        lock_keypad_temporary();
                        return;
                    }
                    
                    // Обновление дисплея
                    SEG7_PushSymbol(convert_key_to_display(key));
                    
                    // Проверка кода при вводе 6 символов
                    if (input_index == CODE_LENGTH) {
                        check_code();
                        clear_input_buffer();
                    }
                }
            }
            break;
            
        case STATE_UNLOCKED:
            // В открытом состоянии клавиатура не активна
            break;
            
        case STATE_PROGRAMMING:
            // Программирование нового кода
            if ((key >= '0' && key <= '9') || (key >= 'A' && key <= 'F')) {
                if (input_index < CODE_LENGTH) {
                    input_buffer[input_index++] = key;
                    input_buffer[input_index] = '\0';
                    
                    // Обновление дисплея
                    SEG7_PushSymbol(convert_key_to_display(key));
                    
                    // Сохранение кода при вводе 6 символов
                    if (input_index == CODE_LENGTH) {
                        memcpy(master_code, input_buffer, CODE_LENGTH);
                        master_code[CODE_LENGTH] = '\0';
                        
                        // Выход из режима программирования
                        current_state = STATE_LOCKED;
                        clear_input_buffer();
                        
                        // Мигание светодиодами подтверждения
                        LED_AllOn();
                        HD_Delay_ms_blocking(500);
                        LED_AllOff();
                        update_leds();
                        
                        // Очистка дисплея
                        SEG7_ClearBuffer();
                    }
                }
            }
            break;
            
        default:
            break;
    }
}

/* Обработка кнопки */
void process_button(Button_EventTypeDef event) {
    switch (event) {
        case BUTTON_EVENT_HOLD:  // Длинное нажатие
            if (current_state == STATE_LOCKED) {
                // Вход в режим программирования
                current_state = STATE_PROGRAMMING;
                clear_input_buffer();
                update_leds();
                
                // Отображение "PrOG" на дисплее
                SEG7_SetBuffer("PrOG");
            } else if (current_state == STATE_PROGRAMMING) {
                // Выход из режима программирования
                current_state = STATE_LOCKED;
                clear_input_buffer();
                update_leds();
                
                // Очистка дисплея
                SEG7_ClearBuffer();
            }
            break;
            
        case BUTTON_EVENT_PRESS:  // Короткое нажатие
            if (current_state == STATE_UNLOCKED) {
                // Закрытие замка
                current_state = STATE_LOCKED;
                clear_input_buffer();
                update_leds();
                reset_wrong_attempts();
                
                // Очистка дисплея
                SEG7_ClearBuffer();
            }
            break;
            
        default:
            break;
    }
}

/* Проверка введенного кода */
void check_code(void) {
    if (strncmp(input_buffer, master_code, CODE_LENGTH) == 0) {
        // Код верный
        current_state = STATE_UNLOCKED;
        reset_wrong_attempts();
        full_buffer_index = 0; // Сброс счетчика блокировки
        
        // Все светодиоды
        LED_AllOn();
        
        // Отображение "oPEn" на дисплее
        SEG7_SetBuffer("oPEn");
    } else {
        // Неверный код
        wrong_attempts++;
        
        if (wrong_attempts >= MAX_WRONG_ATTEMPTS) {
            // Блокировка на 1 минуту
            current_state = STATE_TIMEOUT_LOCKED;
            lock_timer = HD_GetTick();
            
            // Мигание светодиодами ошибки
            for (int i = 0; i < 5; i++) {
                LED_AllOn();
                HD_Delay_ms_blocking(200);
                LED_AllOff();
                HD_Delay_ms_blocking(200);
            }
        } else {
            // Мигание ошибки
            LED_AllOff();
            HD_Delay_ms_blocking(200);
            update_leds();
            HD_Delay_ms_blocking(200);
        }
        
        // Отображение "Err" на дисплее
        SEG7_SetBuffer("Err ");
    }
}

/* Обновление светодиодов по состоянию */
void update_leds(void) {
    LED_AllOff();
    
    switch (current_state) {
        case STATE_LOCKED:
            LED_On(LED2);
            LED_On(LED3);
            break;
            
        case STATE_UNLOCKED:
            LED_AllOn();
            break;
            
        case STATE_PROGRAMMING:
            LED_On(LED1);
            LED_On(LED4);
            break;
            
        case STATE_KEYPAD_LOCKED:
        case STATE_TIMEOUT_LOCKED:
            // Мигание всех светодиодов
            LED_AllOn();
            HD_Delay_ms_blocking(100);
            LED_AllOff();
            break;
    }
}

/* Очистка буфера ввода */
void clear_input_buffer(void) {
    input_index = 0;
    memset(input_buffer, 0, sizeof(input_buffer));
}

/* Блокировка клавиатуры после 20 символов */
void lock_keypad_temporary(void) {
    is_keypad_locked = 1;
    keypad_lock_start = HD_GetTick();
    full_buffer_index = 0;
    
    // Отображение "LoC" на дисплее
    SEG7_SetBuffer("LoC ");
}

/* Разблокировка клавиатуры */
void unlock_keypad(void) {
    is_keypad_locked = 0;
    full_buffer_index = 0;
    
    // Очистка дисплея
    SEG7_ClearBuffer();
}

/* Сброс счетчика неверных попыток */
void reset_wrong_attempts(void) {
    wrong_attempts = 0;
}

/* Основной цикл */
int main(void) {
    init_locker_system();
    
    static char last_key = 0;
    static uint32_t last_key_time = 0;
    
    while (1) {
        // Обработка клавиатуры с защитой от повторных нажатий
        char key = keypadRead();
        if (key && (key != last_key || (HD_GetTick() - last_key_time) > 200)) {
            process_keypad(key);
            last_key = key;
            last_key_time = HD_GetTick();
        } else if (!key) {
            last_key = 0;
        }
        
        // Обработка кнопки
        Buttons_Process();
        Button_EventTypeDef btn_event = Buttons_GetStartStopEvent();
        if (btn_event != BUTTON_EVENT_NONE) {
            process_button(btn_event);
        }
        
        // Обновление светодиодов
        update_leds();
        
        // Обработка ШИМ для светодиодов (для плавного свечения)
        LED_Process();
        
        // Проверка таймаутов блокировки
        if (is_keypad_locked) {
            if ((HD_GetTick() - keypad_lock_start) >= KEYPAD_LOCK_TIMEOUT_MS) {
                unlock_keypad();
            }
        }
        
        if (current_state == STATE_TIMEOUT_LOCKED) {
            if ((HD_GetTick() - lock_timer) >= KEYPAD_LOCK_TIMEOUT_MS) {
                current_state = STATE_LOCKED;
                reset_wrong_attempts();
                update_leds();
                SEG7_ClearBuffer();
            }
        }
        
        // Небольшая задержка для стабильности
        HD_Delay_ms_blocking(10);
    }
}
