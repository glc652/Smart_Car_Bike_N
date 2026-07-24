/*
 * ps2_control.h
 *
 *  Created on: 2026年5月30日
 *      Author: 19740
 */

#ifndef CODE_PS2_CONTROL_H_
#define CODE_PS2_CONTROL_H_

#include "stdint.h"
#define star_speed (8.0f) //15
#define MAX_SPEED (10)
#define MIN_SPEED (8)
extern float target_yaw_Ps2;
extern float ps2_yaw_target;
extern float ps2_speed_target;
void PS2_Control_Init(void);  // PS2手柄控制初始化
void PS2_Control_Process(void);  // PS2手柄控制处理

float PS2_Get_Yaw_Target(void);  // 获取当前目标偏航角
float PS2_Get_Speed_Target(void);  // 获取当前目标速度
uint8_t PS2_Get_Control_Enabled(void);  // 获取手柄控制使能状态

void PS2_Set_Yaw_Target(float yaw);  // 手动设置目标偏航角
void PS2_Set_Speed_Target(float speed);  // 手动设置目标速度

#endif /* CODE_PS2_CONTROL_H_ */
