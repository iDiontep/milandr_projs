/* USER CODE BEGIN Header */

/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/

/* USER CODE BEGIN Includes */
#include "main.h"
#include "leds.h"
#include <math.h>

/* USER CODE END Includes */

// ���������� ��� ���������� ��� ������
static uint32_t pwm_counter = 0;
static uint32_t pwm_period = 500; // ������ ��� (���������� �����)
static uint32_t wave_speed = 3;   // �������� ����� 
static uint8_t wave_active = 1;   // ���� ���������� �����

// ������ ��� ������������� �������� ���������� ���
static uint32_t last_pwm_update = 0;
static const uint32_t pwm_update_interval = 0; // ���������� ������ 10��

// ������� ��� ���������� ��� �������� ��� ������� �����
uint32_t calculate_pwm_value(uint8_t led_index, uint32_t counter) {
    // ����� ���� ��� ������� ����������
    uint32_t phase_shift = (pwm_period / LED_COUNT) * led_index;
    
    // ������� ������� � ����� ��� ������� ����������
    uint32_t position = (counter + phase_shift) % pwm_period;
    
    // �������������� ����� ��� �������� ��������� �������
    float angle = (2.0f * 3.14f * position) / pwm_period;
    float sine_value = (sinf(angle) + 1.0f) / 2.0f; // ����������� �� 0 �� 1
    
    // ����������� � �������� ��� (0-100)
    return (uint32_t)(sine_value * 100);
}

// ������� ��� ��������� ��� �� ����������
void set_led_pwm(LED_TypeDef led, uint32_t pwm_value) {
    static uint32_t pwm_step = 0;
    pwm_step = (pwm_step + 1) % 100;
    
    // ��������� ����������� � ����������� �� �������� ���
    if (pwm_step < pwm_value) {
        LED_On(led);
    } else {
        LED_Off(led);
    }
}

// ����������� ������� ��������� ����������� ��� ��� �����
void LED_Process_PWM(void) {
    if (!wave_active) return;
    
    // ���������, ������ �� ���������� ������� � ���������� ����������
    uint32_t current_time = HD_GetTick();
    if ((current_time - last_pwm_update) < pwm_update_interval) {
        return; // ������� ���� ��� ����������
    }
    
    last_pwm_update = current_time;
    
    // ��������� ������� ��� ��������
    pwm_counter = (pwm_counter + wave_speed) % pwm_period;
    
    // ������������� ��� �������� ��� ������� ����������
    for (int i = 0; i < LED_COUNT; i++) {
        uint32_t pwm_value = calculate_pwm_value(i, pwm_counter);
        set_led_pwm((LED_TypeDef)i, pwm_value);
    }
}

// ������� ���������� ������
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
    HD_System_Init();  // ����������������� ��� ������������� ���������� �������
    LED_Init();
    
    /* Start PWM wave */
    start_pwm_wave();
    set_wave_speed(2);  // ��������� ��������
    set_pwm_period(3000); // ������ �����

    /* Main loop */
    while(1) {
        // ���������� PWM ���������
        LED_Process_PWM();
        
        // �������� �������� ��� ������������
        // ����� ��������� ��� ������, ��� ��� � ��� ������ ���� ��������� ��������
        //for(volatile int i = 0; i < 100; i++);
    }
}