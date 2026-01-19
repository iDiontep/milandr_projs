#include "MDR32F9Q2I.h"                 // Device header
#include "MDR32FxQI_rst_clk.h"          // Milandr::Drivers:RST_CLK
#include "MDR32FxQI_port.h"             // Milandr::Drivers:PORT
#include "MDR32FxQI_timer.h"
#include "keyboard_driver.h"
#include "hardware_drivers.h"

/* Debounce constants */
#define KEYBOARD_DEBOUNCE_TIME_MS   20   // Время стабилизации контакта (мс)

typedef struct {
    MDR_PORT_TypeDef* PORT;
    uint32_t PIN;
} Pin;

/* Keyboard debounce state */
static char last_raw_key = 0;           // Последняя прочитанная клавиша
static char debounced_key = 0;         // Стабильная клавиша после debounce
static uint32_t debounce_start_time = 0; // Время начала изменения состояния

static Pin row[4] = {
    {MDR_PORTA, PORT_Pin_6}, // R1 (phys 21) XP2
    {MDR_PORTA, PORT_Pin_3}, // R2 (phys 19)
    {MDR_PORTA, PORT_Pin_2}, // R3 (phys 17)
    {MDR_PORTA, PORT_Pin_1}  // R4 (phys 15)
};

static Pin col[4] = {
    {MDR_PORTD, PORT_Pin_6}, // C1 (phys 38) XP1
    {MDR_PORTD, PORT_Pin_5}, // C2 (phys 36)
    {MDR_PORTD, PORT_Pin_3}, // C3 (phys 34)
    {MDR_PORTD, PORT_Pin_2}  // C4 (phys 32)
};

void Keyboard_Init(void)
{
    RST_CLK_PCLKcmd(
        RST_CLK_PCLK_PORTA |
        RST_CLK_PCLK_PORTD,
        ENABLE
    );

    PORT_InitTypeDef port;
    
    /* Initialize structure directly (like in example) */
    port.PORT_OE = PORT_OE_OUT;
    port.PORT_FUNC = PORT_FUNC_PORT;
    port.PORT_MODE = PORT_MODE_DIGITAL;
    port.PORT_SPEED = PORT_SPEED_SLOW;
    port.PORT_PULL_UP = PORT_PULL_UP_OFF;
    port.PORT_PULL_DOWN = PORT_PULL_DOWN_OFF;

    // Initialize row pins as outputs
    for (int i = 0; i < 4; i++) {
        port.PORT_Pin = row[i].PIN;
        PORT_Init(row[i].PORT, &port);
        PORT_ResetBits(row[i].PORT, row[i].PIN);
    }

    // Initialize column pins as inputs with pull-down
    port.PORT_OE = PORT_OE_IN;
    port.PORT_PULL_DOWN = PORT_PULL_DOWN_ON;
    for (int i = 0; i < 4; i++) {
        port.PORT_Pin = col[i].PIN;
        PORT_Init(col[i].PORT, &port);
    }
    
    // Инициализация переменных debounce
    last_raw_key = 0;
    debounced_key = 0;
    debounce_start_time = HD_GetTick();
}


/**
  * @brief  Raw keypad read without debounce (internal function)
  * @retval Pressed key character or 0 if no key pressed
  */
static char keypadReadRaw(void)
{
    static const char keymap[4][4] =
    {
        {'1','2','3','A'},
        {'4','5','6','B'},
        {'7','8','9','C'},
        {'*','0','#','D'}
    };

    for (int r = 0; r < 4; r++)
    {
        PORT_SetBits(row[r].PORT, row[r].PIN);

        for (int c = 0; c < 4; c++)
        {
            if (PORT_ReadInputDataBit(col[c].PORT, col[c].PIN))
            {
                PORT_ResetBits(row[r].PORT, row[r].PIN);
                return keymap[r][c];
            }
        }

        PORT_ResetBits(row[r].PORT, row[r].PIN);
    }

    return 0;
}

/**
  * @brief  Process keyboard debounce - call periodically
  * @retval None
  */
void Keyboard_Process(void)
{
    uint32_t current_time = HD_GetTick();
    char current_raw_key = keypadReadRaw();
    
    // Если состояние изменилось, начинаем отсчет времени debounce
    if (current_raw_key != last_raw_key) {
        debounce_start_time = current_time;
        last_raw_key = current_raw_key;
        return;
    }
    
    // Проверяем, прошло ли достаточно времени для стабилизации
    uint32_t time_diff = (current_time >= debounce_start_time) ? 
                         (current_time - debounce_start_time) : 
                         (0xFFFFFFFF - debounce_start_time + current_time + 1);
    
    if (time_diff >= KEYBOARD_DEBOUNCE_TIME_MS) {
        // Состояние стабильно в течение времени debounce
        // Обновляем debounced_key только если оно изменилось
        if (debounced_key != current_raw_key) {
            debounced_key = current_raw_key;
        }
    }
}

/**
  * @brief  Get debounced keypad value
  * @retval Pressed key character or 0 if no key pressed
  */
char keypadRead(void)
{
    return debounced_key;
}