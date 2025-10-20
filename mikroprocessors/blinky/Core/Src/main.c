/* USER CODE BEGIN Header */

/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/

/* USER CODE BEGIN Includes */
#include "main.h"
#include "leds.h"
#include <math.h>

/* USER CODE END Includes */

// Переменные для управления ШИМ волной
static uint32_t pwm_counter = 0;
static uint32_t pwm_period = 500; // Период ШИМ (количество шагов)
static uint32_t wave_speed = 3;   // Скорость волны 
static uint8_t wave_active = 1;   // Флаг активности волны

// Таймер для регулирования скорости обновления ШИМ
static uint32_t last_pwm_update = 0;
static const uint32_t pwm_update_interval = 0; // Обновление каждые 10мс

// Функция для вычисления ШИМ значения для бегущей волны
uint32_t calculate_pwm_value(uint8_t led_index, uint32_t counter) {
    // Сдвиг фазы для каждого светодиода
    uint32_t phase_shift = (pwm_period / LED_COUNT) * led_index;
    
    // Текущая позиция в волне для данного светодиода
    uint32_t position = (counter + phase_shift) % pwm_period;
    
    // Синусоидальная волна для плавного изменения яркости
    float angle = (2.0f * 3.14f * position) / pwm_period;
    float sine_value = (sinf(angle) + 1.0f) / 2.0f; // Нормализуем от 0 до 1
    
    // Преобразуем в значение ШИМ (0-100)
    return (uint32_t)(sine_value * 100);
}

// Функция для установки ШИМ на светодиоде
void set_led_pwm(LED_TypeDef led, uint32_t pwm_value) {
    static uint32_t pwm_step = 0;
    pwm_step = (pwm_step + 1) % 100;
    
    // Управляем светодиодом в зависимости от значения ШИМ
    if (pwm_step < pwm_value) {
        LED_On(led);
    } else {
        LED_Off(led);
    }
}

// Обновленная функция обработки светодиодов для ШИМ волны
void LED_Process_PWM(void) {
    if (!wave_active) return;
    
    // Проверяем, прошло ли достаточно времени с последнего обновления
    uint32_t current_time = HD_GetTick();
    if ((current_time - last_pwm_update) < pwm_update_interval) {
        return; // Слишком рано для обновления
    }
    
    last_pwm_update = current_time;
    
    // Обновляем счетчик для анимации
    pwm_counter = (pwm_counter + wave_speed) % pwm_period;
    
    // Устанавливаем ШИМ значения для каждого светодиода
    for (int i = 0; i < LED_COUNT; i++) {
        uint32_t pwm_value = calculate_pwm_value(i, pwm_counter);
        set_led_pwm((LED_TypeDef)i, pwm_value);
    }
}

// Функции управления волной
void start_pwm_wave(void) {
    wave_active = 1;
    pwm_counter = 0;
    last_pwm_update = HD_GetTick();
}

void stop_pwm_wave(void) {
    wave_active = 0;
    LED_AllOff();
}

void set_wave_speed(uint32_t speed) {
    wave_speed = speed;
}

void set_pwm_period(uint32_t period) {
    pwm_period = period;
}

int main(void) {
    /* Initialize system and LEDs */
    HD_System_Init();  // Раскомментировать для инициализации системного таймера
    LED_Init();
    
    /* Start PWM wave */
    start_pwm_wave();
    set_wave_speed(2);  // Медленная скорость
    set_pwm_period(3000); // Период волны

    /* Main loop */
    while(1) {
        // Используем PWM обработку
        LED_Process_PWM();
        
        // Короткая задержка для стабильности
        // Можно уменьшить или убрать, так как у нас теперь есть временной контроль
        //for(volatile int i = 0; i < 100; i++);
    }
}