#ifndef KEYBOARD_H
#define KEYBOARD_H

#include "GPIO_Union.h"

// Конфигурация клавиатуры 4x4
#define KEYPAD_ROWS 4
#define KEYPAD_COLS 4

// Пины для строк (ROW) - выходы
#define ROW1_PORT MDR_UNION_PORTD
#define ROW1_PIN  6
#define ROW2_PORT MDR_UNION_PORTD  
#define ROW2_PIN  5
#define ROW3_PORT MDR_UNION_PORTD
#define ROW3_PIN  3
#define ROW4_PORT MDR_UNION_PORTD
#define ROW4_PIN  2

// Пины для столбцов (COL) - входы
#define COL1_PORT MDR_UNION_PORTB
#define COL1_PIN  0
#define COL2_PORT MDR_UNION_PORTB
#define COL2_PIN  1  
#define COL3_PORT MDR_UNION_PORTB
#define COL3_PIN  2
#define COL4_PORT MDR_UNION_PORTB
#define COL4_PIN  5

// Карта клавиш
extern const char keymap[4][4];

void Keyboard_Init(void);
void Keyboard_SetRow(uint8_t row, uint8_t state);
uint8_t Keyboard_ReadColumn(uint8_t col);
char Keyboard_Scan(void);
uint8_t Keyboard_ReadRow(uint8_t row);

#endif