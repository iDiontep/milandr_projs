#ifndef KEYBOARD_DRIVER_H
#define KEYBOARD_DRIVER_H

#include <stdint.h>

/* Function prototypes */
void Keyboard_Init(void);
void Keyboard_Process(void);  // Обработка debounce - вызывать периодически
char keypadRead(void);        // Получить стабильное значение клавиши

#endif /* KEYBOARD_DRIVER_H */

