#ifndef SEVSEG_H
#define SEVSEG_H

#include "GPIO_Union.h"

void sevseg_GPIO_Init(void);
void sevseg_display_digit(uint8_t digit);
void sevseg_enable_digit(uint8_t digit_pos);
void sevseg_disable_all_digits(void);

#endif