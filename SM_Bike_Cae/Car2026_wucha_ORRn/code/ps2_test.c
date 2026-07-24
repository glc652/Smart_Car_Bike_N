/*
 * ps2_test.c
 *
 *  Created on: 2026年5月30日
 *      Author: 19740
 *  功能: PS2手柄通信测试
 *  note: 手杆往上退是值递减
 */
//===================================================Include===================================================
#include "ps2.h"
#include "zf_common_headfile.h"

//===================================================全局变量===================================================
static JOYSTICK_TypeDef ps2_test_data = {0};
static uint8_t ps2_test_enabled = 0;
static JOYSTICK_TypeDef ps2_last_stable = {0};
static uint8_t ps2_debounce_count = 0;
#define PS2_DEBOUNCE_TIME 3

//===================================================测试函数===================================================
/**
  * @brief  PS2手柄通信测试初始化
  * @param  无
  * @return 无
  */
void PS2_Test_Init(void)
{
    AX_PS2_Init();
    ps2_test_enabled = 1;
}

/**
  * @brief  PS2手柄通信测试 - 基础读取测试
  * @param  无
  * @return 无
  *
  * 功能: 读取手柄数据并通过串口输出
  * 用于检查:
  * 1. 手柄是否能被正确识别
  * 2. 按键数据是否正常
  * 3. 摇杆数据是否正常
  */
void PS2_Test_ReadData(void)
{
    if(!ps2_test_enabled)
        return;

    AX_PS2_ScanKey(&ps2_test_data);

    // 输出手柄模式
    printf("Mode: 0x%02X | ", ps2_test_data.mode);

    // 输出按键1状态 (btn1)
    printf("BTN1: 0x%02X (", ps2_test_data.btn1);
    if(!(ps2_test_data.btn1 & (1<<0))) printf("SEL ");
    if(!(ps2_test_data.btn1 & (1<<1))) printf("L3 ");
    if(!(ps2_test_data.btn1 & (1<<2))) printf("R3 ");
    if(!(ps2_test_data.btn1 & (1<<3))) printf("STR ");
    if(!(ps2_test_data.btn1 & (1<<4))) printf("UP ");
    if(!(ps2_test_data.btn1 & (1<<5))) printf("R ");
    if(!(ps2_test_data.btn1 & (1<<6))) printf("DN ");
    if(!(ps2_test_data.btn1 & (1<<7))) printf("L ");
    printf(") | ");

    // 输出按键2状态 (btn2)
    printf("BTN2: 0x%02X (", ps2_test_data.btn2);
    if(!(ps2_test_data.btn2 & (1<<0))) printf("L2 ");
    if(!(ps2_test_data.btn2 & (1<<1))) printf("R2 ");
    if(!(ps2_test_data.btn2 & (1<<2))) printf("L1 ");
    if(!(ps2_test_data.btn2 & (1<<3))) printf("R1 ");
    if(!(ps2_test_data.btn2 & (1<<4))) printf("TRI ");
    if(!(ps2_test_data.btn2 & (1<<5))) printf("CIR ");
    if(!(ps2_test_data.btn2 & (1<<6))) printf("CRO ");
    if(!(ps2_test_data.btn2 & (1<<7))) printf("SQU ");
    printf(")\n");

    // 输出摇杆数据
    printf("RJoy: LR=%3d UD=%3d | LJoy: LR=%3d UD=%3d\n",
           PS2_GetJoyValue(ps2_test_data.RJoy_LR),
           PS2_GetJoyValue(ps2_test_data.RJoy_UD),
           PS2_GetJoyValue(ps2_test_data.LJoy_LR),
           PS2_GetJoyValue(ps2_test_data.LJoy_UD));
}

/**
  * @brief  PS2手柄通信测试 - 遥杆数据输出
  * @param  无
  * @return 无
  *
  * 功能: 输出原始数据和解析后的数据，用于调试
  */
void PS2_Test_DetailedData(void)
{
    if(!ps2_test_enabled)
        return;

    AX_PS2_ScanKey(&ps2_test_data);

//    printf("\n========== PS2 Detailed Data ==========\n");
//    printf("Mode: 0x%02X\n", ps2_test_data.mode);
//    printf("BTN1: 0x%02X\n", ps2_test_data.btn1);
//    printf("BTN2: 0x%02X\n", ps2_test_data.btn2);
    printf("RJoy_LR (Raw): 0x%02X (%3d) -> Mapped: %4d  |@|  RJoy_UD (Raw): 0x%02X (%3d) -> Mapped: %4d\n",
           ps2_test_data.RJoy_LR, ps2_test_data.RJoy_LR,
           PS2_GetJoyValue(ps2_test_data.RJoy_LR),
           ps2_test_data.RJoy_UD, ps2_test_data.RJoy_UD,
           PS2_GetJoyValue(ps2_test_data.RJoy_UD)
           );
//    printf("====================\n");
//    printf("LJoy_LR (Raw): 0x%02X (%3d) -> Mapped: %4d  |@|  LJoy_UD (Raw): 0x%02X (%3d) -> Mapped: %4d\n",
//           ps2_test_data.LJoy_LR, ps2_test_data.LJoy_LR,
//           PS2_GetJoyValue(ps2_test_data.LJoy_LR),
//           ps2_test_data.LJoy_UD, ps2_test_data.LJoy_UD,
//           PS2_GetJoyValue(ps2_test_data.LJoy_UD)
//          );
//    printf("====================\n");

}

/**
  * @brief  PS2手柄通信测试 - 按键检测测试
  * @param  无
  * @return 无
  *
  * 功能: 检测按键按下事件
  */
void PS2_Test_KeyDetection(void)
{
    if(!ps2_test_enabled)
        return;

    AX_PS2_ScanKey(&ps2_test_data);

    // 消抖处理：检测状态是否连续相同
    if(ps2_test_data.btn1 == ps2_last_stable.btn1 && ps2_test_data.btn2 == ps2_last_stable.btn2)
    {
        ps2_debounce_count = 0;
        return;
    }

    ps2_debounce_count++;
    if(ps2_debounce_count < PS2_DEBOUNCE_TIME)
        return;

    ps2_debounce_count = 0;

    // 检测btn1按键
    if(ps2_test_data.btn1 != ps2_last_stable.btn1)
    {
        printf("BTN1 Changed: 0x%02X -> 0x%02X\n", ps2_last_stable.btn1, ps2_test_data.btn1);

        // 检测SELECT
        if((!(ps2_test_data.btn1 & (1<<0))) && (ps2_last_stable.btn1 & (1<<0)))
            printf("  -> SELECT Pressed\n");
        // 检测START
        if((!(ps2_test_data.btn1 & (1<<3))) && (ps2_last_stable.btn1 & (1<<3)))
            printf("  -> START Pressed\n");
        // 检测方向键
        if((!(ps2_test_data.btn1 & (1<<4))) && (ps2_last_stable.btn1 & (1<<4)))
            printf("  -> UP Pressed\n");
        if((!(ps2_test_data.btn1 & (1<<5))) && (ps2_last_stable.btn1 & (1<<5)))
            printf("  -> RIGHT Pressed\n");
        if((!(ps2_test_data.btn1 & (1<<6))) && (ps2_last_stable.btn1 & (1<<6)))
            printf("  -> DOWN Pressed\n");
        if((!(ps2_test_data.btn1 & (1<<7))) && (ps2_last_stable.btn1 & (1<<7)))
            printf("  -> LEFT Pressed\n");
    }

    // 检测btn2按键
    if(ps2_test_data.btn2 != ps2_last_stable.btn2)
    {
        printf("BTN2 Changed: 0x%02X -> 0x%02X\n", ps2_last_stable.btn2, ps2_test_data.btn2);

        if((!(ps2_test_data.btn2 & (1<<0))) && (ps2_last_stable.btn2 & (1<<0)))
            printf("  -> L2 Pressed\n");
        if((!(ps2_test_data.btn2 & (1<<1))) && (ps2_last_stable.btn2 & (1<<1)))
            printf("  -> R2 Pressed\n");
        if((!(ps2_test_data.btn2 & (1<<2))) && (ps2_last_stable.btn2 & (1<<2)))
            printf("  -> L1 Pressed\n");
        if((!(ps2_test_data.btn2 & (1<<3))) && (ps2_last_stable.btn2 & (1<<3)))
            printf("  -> R1 Pressed\n");
        if((!(ps2_test_data.btn2 & (1<<4))) && (ps2_last_stable.btn2 & (1<<4)))
            printf("  -> TRIANGLE Pressed\n");
        if((!(ps2_test_data.btn2 & (1<<5))) && (ps2_last_stable.btn2 & (1<<5)))
            printf("  -> CIRCLE Pressed\n");
        if((!(ps2_test_data.btn2 & (1<<6))) && (ps2_last_stable.btn2 & (1<<6)))
            printf("  -> CROSS Pressed\n");
        if((!(ps2_test_data.btn2 & (1<<7))) && (ps2_last_stable.btn2 & (1<<7)))
            printf("  -> SQUARE Pressed\n");
    }

    ps2_last_stable = ps2_test_data;
}

/**
  * @brief  PS2手柄通信测试 - 摇杆灵敏度测试
  * @param  无
  * @return 无
  *
  * 功能: 测试摇杆的精度和死区
  */
void PS2_Test_JoystickSensitivity(void)
{
    static uint8_t test_count = 0;

    if(!ps2_test_enabled)
        return;

    AX_PS2_ScanKey(&ps2_test_data);

    test_count++;
    if(test_count >= 10)  // 每10次读取输出一次
    {
        test_count = 0;

        int8_t rjoy_lr = PS2_GetJoyValue(ps2_test_data.RJoy_LR);
        int8_t rjoy_ud = PS2_GetJoyValue(ps2_test_data.RJoy_UD);
        int8_t ljoy_lr = PS2_GetJoyValue(ps2_test_data.LJoy_LR);
        int8_t ljoy_ud = PS2_GetJoyValue(ps2_test_data.LJoy_UD);

        // 只在摇杆有动作时输出
        if(rjoy_lr != 0 || rjoy_ud != 0 || ljoy_lr != 0 || ljoy_ud != 0)
        {
            printf("RJoy: [");
            for(int i = -100; i <= 100; i += 20)
            {
                if(i == rjoy_lr) printf("R");
                else if(i == 0) printf("|");
                else printf("-");
            }
            printf("] LR=%4d  ", rjoy_lr);

            printf("LJoy: [");
            for(int i = -100; i <= 100; i += 20)
            {
                if(i == ljoy_lr) printf("L");
                else if(i == 0) printf("|");
                else printf("-");
            }
            printf("] LR=%4d\n", ljoy_lr);
        }
    }
}

/**
  * @brief  PS2手柄通信测试 - 稳定性测试
  * @param  无
  * @return 无
  *
  * 功能: 测试手柄通信的稳定性，检测丢包或通信错误
  */
void PS2_Test_Stability(void)
{
    static uint32_t read_count = 0;
    static uint32_t error_count = 0;

    if(!ps2_test_enabled)
        return;

    AX_PS2_ScanKey(&ps2_test_data);
    read_count++;

    // 简单的数据有效性检查
    // 如果mode不是预期值，可能是通信错误
    if(ps2_test_data.mode != 0x73 && ps2_test_data.mode != 0x79)
    {
        error_count++;
    }

    // 每1000次读取输出一次统计
    if(read_count % 1000 == 0)
    {
        printf("PS2 Stability: Total=%d, Errors=%d, ErrorRate=%.2f%%\n",
               read_count, error_count,
               (float)error_count / read_count * 100.0f);
    }
}

/**
  * @brief  PS2手柄通信测试 - 完整测试
  * @param  无
  * @return 无
  *
  * 功能: 运行所有测试
  */
void PS2_Test_All(void)
{
    printf("\n========== PS2 Controller Test Start ==========\n");

    PS2_Test_Init();

    printf("Reading data for 5 seconds...\n");
    for(int i = 0; i < 500; i++)
    {
        PS2_Test_ReadData();
        system_delay_ms(10);
    }

    printf("========== PS2 Controller Test End ==========\n\n");
}

/**
  * @brief  获取测试状态
  * @param  无
  * @return 1=测试中, 0=未测试
  */
uint8_t PS2_Test_IsEnabled(void)
{
    return ps2_test_enabled;
}

/**
  * @brief  启用/禁用测试
  * @param  enable: 1=启用, 0=禁用
  * @return 无
  */
void PS2_Test_SetEnabled(uint8_t enable)
{
    ps2_test_enabled = enable;
}
