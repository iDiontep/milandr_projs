#include "sevseg.h"

// Паттерны для цифр 0-9 (сегменты A-H)
const uint8_t digit_patterns[10] = {
    0b00111111, // 0: ABCDEF
    0b00000110, // 1: BC
    0b01011011, // 2: ABDEG
    0b01001111, // 3: ABCDG
    0b01100110, // 4: BCFG
    0b01101101, // 5: ACDFG
    0b01111101, // 6: ACDEFG
    0b00000111, // 7: ABC
    0b01111111, // 8: ABCDEFG
    0b01101111  // 9: ABCDFG
};

void sevseg_display_digit(uint8_t digit) {
    if (digit > 9) return;
    
    uint8_t pattern = digit_patterns[digit];
    
    // Устанавливаем сегменты A-H
    MDR_UNION_PORTA->RXTX_Bit.Pin_7 = (pattern & 0x01) ? ENABLE_BIT : DISABLE_BIT; // A
    MDR_UNION_PORTA->RXTX_Bit.Pin_4 = (pattern & 0x02) ? ENABLE_BIT : DISABLE_BIT; // B
    MDR_UNION_PORTA->RXTX_Bit.Pin_5 = (pattern & 0x04) ? ENABLE_BIT : DISABLE_BIT; // C
    MDR_UNION_PORTB->RXTX_Bit.Pin_0 = (pattern & 0x08) ? ENABLE_BIT : DISABLE_BIT; // D
    MDR_UNION_PORTB->RXTX_Bit.Pin_2 = (pattern & 0x10) ? ENABLE_BIT : DISABLE_BIT; // E
    MDR_UNION_PORTB->RXTX_Bit.Pin_1 = (pattern & 0x20) ? ENABLE_BIT : DISABLE_BIT; // F
    MDR_UNION_PORTB->RXTX_Bit.Pin_5 = (pattern & 0x40) ? ENABLE_BIT : DISABLE_BIT; // G
    MDR_UNION_PORTB->RXTX_Bit.Pin_6 = DISABLE_BIT; // H (точка)
}

void sevseg_enable_digit(uint8_t digit_pos) {
    // Выключаем все разряды
    sevseg_disable_all_digits();
    
    // Включаем нужный разряд
    switch(digit_pos) {
        case 0: MDR_UNION_PORTF->RXTX_Bit.Pin_1 = ENABLE_BIT; break; // DIGIT 0
        case 1: MDR_UNION_PORTE->RXTX_Bit.Pin_0 = ENABLE_BIT; break; // DIGIT 1
        case 2: MDR_UNION_PORTC->RXTX_Bit.Pin_1 = ENABLE_BIT; break; // DIGIT 2
        case 3: MDR_UNION_PORTC->RXTX_Bit.Pin_0 = ENABLE_BIT; break; // DIGIT 3
    }
}

void sevseg_disable_all_digits(void) {
    MDR_UNION_PORTF->RXTX_Bit.Pin_1 = DISABLE_BIT; // DIGIT 0
    MDR_UNION_PORTE->RXTX_Bit.Pin_0 = DISABLE_BIT; // DIGIT 1
    MDR_UNION_PORTC->RXTX_Bit.Pin_1 = DISABLE_BIT; // DIGIT 2
    MDR_UNION_PORTC->RXTX_Bit.Pin_0 = DISABLE_BIT; // DIGIT 3
}