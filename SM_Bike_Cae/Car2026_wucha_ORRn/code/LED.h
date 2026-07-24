/*
 * LED.h
 *
 *  Created on: 2025年11月13日
 *      Author: glc
 */

#ifndef CODE_LED_H_
#define CODE_LED_H_

#include "zf_common_typedef.h"

typedef enum {
    BUZZER_STATE_IDLE,      // 空闲状态
    BUZZER_STATE_BEEPING    // 发声中状态
} Buzzer_State;

void LED_Init(void);
void LED_Shine(void);
void Buzzer_Beep_Once(void);
void Buzzer_Process(void);

#endif /* CODE_LED_H_ */
