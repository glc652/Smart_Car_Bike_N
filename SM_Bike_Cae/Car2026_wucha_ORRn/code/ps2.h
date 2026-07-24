/*
 * ps2.h
 * PS2无线手柄驱动头文件
 */

#ifndef CODE_PS2_H_
#define CODE_PS2_H_

#include "stdint.h"

// PS2手柄数据结构体
typedef struct
{
    uint8_t mode;        // 手柄工作模式

    uint8_t btn1;        // 按键1
                        // Bit0:SELECT  Bit1:L3  Bit2:R3  Bit3:START
                        // Bit4:上      Bit5:右  Bit6:下  Bit7:左

    uint8_t btn2;        // 按键2
                        // Bit0:L2  Bit1:R2  Bit2:L1  Bit3:R1
                        // Bit4:△  Bit5:○  Bit6:×  Bit7:□

    uint8_t RJoy_LR;     // 右摇杆左右  0x00=左  0xFF=右
    uint8_t RJoy_UD;     // 右摇杆上下  0x00=上  0xFF=下
    uint8_t LJoy_LR;     // 左摇杆左右  0x00=左  0xFF=右
    uint8_t LJoy_UD;     // 左摇杆上下  0x00=上  0xFF=下

} JOYSTICK_TypeDef;

//============================== 按键位定义 btn1 ==============================
#define PS2_SELECT       (1<<0)   // SELECT键
#define PS2_L3           (1<<1)   // 左摇杆按下
#define PS2_R3           (1<<2)   // 右摇杆按下
#define PS2_START        (1<<3)   // START键
#define PS2_UP           (1<<4)   // 方向键 上
#define PS2_RIGHT        (1<<5)   // 方向键 右
#define PS2_DOWN         (1<<6)   // 方向键 下
#define PS2_LEFT         (1<<7)   // 方向键 左

//============================== 按键位定义 btn2 ==============================
#define PS2_L2           (1<<0)   // L2
#define PS2_R2           (1<<1)   // R2
#define PS2_L1           (1<<2)   // L1
#define PS2_R1           (1<<3)   // R1
#define PS2_TRIANGLE     (1<<4)   // 三角 △
#define PS2_CIRCLE       (1<<5)   // 圆形 ○
#define PS2_CROSS        (1<<6)   // 叉号 ×
#define PS2_SQUARE       (1<<7)   // 方形 □

//============================== 外部函数声明 ==============================
void AX_PS2_Init(void);                                  // PS2初始化
void AX_PS2_ScanKey(JOYSTICK_TypeDef* JoystickStruct);   // 读取手柄数据

JOYSTICK_TypeDef* PS2_GetCurrent(void);                  // 获取当前手柄状态
JOYSTICK_TypeDef* PS2_GetStable(void);                   // 获取消抖后的稳定状态
uint8_t PS2_IsKeyDown(uint8_t btn_byte, uint8_t btn_bit);// 判断按键是否按下
uint8_t PS2_IsKeyPressed_Edge(uint8_t btn_byte, uint8_t last_btn_byte, uint8_t btn_bit); // 判断按键刚按下
int8_t PS2_GetJoyValue(uint8_t joy_value);               // 摇杆值映射(-100~100)

#endif /* CODE_PS2_H_ */
