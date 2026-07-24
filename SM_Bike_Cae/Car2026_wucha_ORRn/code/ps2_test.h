/*
 * ps2_test.h
 *
 *  Created on: 2026年5月30日
 *      Author: 19740
 */

#ifndef CODE_PS2_TEST_H_
#define CODE_PS2_TEST_H_

#include "stdint.h"

// 测试函数
void PS2_Test_Init(void);                    // 初始化测试
void PS2_Test_ReadData(void);                // 基础读取测试
void PS2_Test_DetailedData(void);            // 详细数据输出
void PS2_Test_KeyDetection(void);            // 按键检测测试
void PS2_Test_JoystickSensitivity(void);     // 摇杆灵敏度测试
void PS2_Test_Stability(void);               // 稳定性测试
void PS2_Test_All(void);                     // 完整测试

// 控制函数
uint8_t PS2_Test_IsEnabled(void);            // 获取测试状态
void PS2_Test_SetEnabled(uint8_t enable);    // 启用/禁用测试

#endif /* CODE_PS2_TEST_H_ */
