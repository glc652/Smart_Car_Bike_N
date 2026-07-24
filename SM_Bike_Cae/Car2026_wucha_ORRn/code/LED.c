/*
 * LED.c
 *
 *  Created on: 2025年11月13日
 *      Author: 19740
 */
//===================================================Include===================================================
#include "zf_driver_gpio.h"
#include "zf_driver_timer.h"
#include "LED.h"
//===================================================Define&Declare===================================================
#define LED1                    (P20_9)
#define BUZZER_PIN              (P33_10)
#define BUZZER_BEEP_DURATION  500   // 发声时长(ms)


static uint32_t buzzer_start_tick = 0; // 发声开始时间戳
static Buzzer_State buzzer_state = BUZZER_STATE_IDLE;
uint8 buzzer_flag = 0;
//===================================================内部调用===================================================
//===================================================外部调用===================================================
/*
 * LEDheBuzzer初始化
 */
void LED_Init(void){
    gpio_init(LED1, GPO, GPIO_LOW, GPO_PUSH_PULL);          // 初始化 LED1 输出 默认高电平 推挽输出模式
    gpio_init(BUZZER_PIN, GPO, GPIO_LOW, GPO_PUSH_PULL);
}

/*
 * LED闪烁
 */
void LED_Shine(void){
    gpio_toggle_level(LED1);
}

/**
 *   触发蜂鸣器叫一声
 */
void Buzzer_Beep_Once(void)
{
    // 避免重复触发（如果正在响，不处理）
    if (buzzer_state != BUZZER_STATE_IDLE) {
        return;
    }
    gpio_toggle_level(BUZZER_PIN);
    buzzer_start_tick = system_getval_ms();
    buzzer_state = BUZZER_STATE_BEEPING;
}
/**
 * 蜂鸣器状态处理
 */
void Buzzer_Process(void)
{
    // 仅在发声中时处理
    if (buzzer_state != BUZZER_STATE_BEEPING) {
        return;
    }
    if ((system_getval_ms() - buzzer_start_tick) >= BUZZER_BEEP_DURATION) {
        gpio_toggle_level(BUZZER_PIN);
        buzzer_state = BUZZER_STATE_IDLE;
    }
}

