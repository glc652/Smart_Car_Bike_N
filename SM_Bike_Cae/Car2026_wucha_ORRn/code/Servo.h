/*
 * Servo.h
 *
 *  Created on: 2025锟斤拷12锟斤拷11锟斤拷
 *      Author: 19740
 */

#ifndef CODE_SERVO_H_
#define CODE_SERVO_H_
#include "zf_common_typedef.h"

void Servo_Init(void);
void Servo_Set(int32 angle);
void Servo_Free(void);  // 閲婃斁鑸垫満淇濇寔鍔涳紝鎵嬫帹鏃朵娇鐢�

void Servo_PID_init(void);
void Servo_turn_loop(float target_angle, float current_angle, float current_rate); // 锟角讹拷-锟斤拷锟劫度伙拷

#endif /* CODE_SERVO_H_ */
