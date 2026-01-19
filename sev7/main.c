#include "MDR32F9Q2I.h"
#include "GPIO_Union.h"
#include "sevseg.h"
#include "keyboard.h"

#define DELAY_MS 5

void Delay(uint32_t ms) {
    for (uint32_t i = 0; i < ms * 1000; i++) {
        __NOP();
    }
}

int main(void) {
    // Инициализация
    sevseg_GPIO_Init();
    Keyboard_Init();
    
    // Начальное состояние - показ 0000
    uint8_t display_buffer[4] = {0, 0, 0, 0};
    uint8_t display_pos = 0;
    uint8_t key_count = 0;
    uint32_t timer = 0;
    
    while (1) {
        timer++;
        
        // Динамическая индикация (каждые 4 цикла переключаем разряд)
        if (timer % 4 == 0) {
            sevseg_display_digit(display_buffer[display_pos]);
            sevseg_enable_digit(display_pos);
            display_pos = (display_pos + 1) % 4;
        }
        
        // Опрос клавиатуры (по одной строке за цикл)
        uint8_t current_row = (timer / 4) % 4; // Синхронизируем с индикацией
        uint8_t col_data = Keyboard_ReadRow(current_row);
        
        // Если есть нажатие в текущей строке
        if (col_data != 0) {
            // Находим нажатую колонку
            for (int col = 0; col < 4; col++) {
                if (col_data & (1 << col)) {
                    // Преобразуем row/col в номер 0-15
                    uint8_t key_num = current_row * 4 + col;
                    
                    // Добавляем в буфер отображения
                    if (key_count < 4) {
                        display_buffer[key_count] = key_num;
                        key_count++;
                    } else {
                        // Сдвигаем буфер если заполнен
                        for (int i = 0; i < 3; i++) {
                            display_buffer[i] = display_buffer[i+1];
                        }
                        display_buffer[3] = key_num;
                    }
                    
                    // Ждем отпускания кнопки
                    while (Keyboard_ReadRow(current_row) & (1 << col)) {
                        Delay(1);
                    }
                    
                    break;
                }
            }
        }
        
        Delay(DELAY_MS);
    }
}