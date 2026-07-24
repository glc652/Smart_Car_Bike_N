/*
 * ps2_control.c
 *
 *  Created on: 2026年5月30日
 *      Author: 19740
 *  功能: 使用PS2手柄控制单车，用于测试偏航角闭环
 */
//===================================================Include===================================================
#include "ps2.h"
#include "ps2_control.h"
#include "Car_control.h"
#include "Motor.h"
#include "Servo.h"
#include "zf_common_headfile.h"

//===================================================全局变量===================================================
static JOYSTICK_TypeDef ps2_joy = {0};
float ps2_yaw_target = 0.0f;  // ps2给的偏航角
float ps2_speed_target = MIN_SPEED;  // ps2给的速度
static uint8_t ps2_control_enabled = 1;  // 手柄控制使能标志
//===================================================函数===================================================
/**
  * @函数  PS2手柄控制初始化
  * @参数  无
  * @返回值  无
  */
void PS2_Control_Init(void)
{
    AX_PS2_Init();
    ps2_yaw_target = 0.0f;
    ps2_speed_target = star_speed;
    ps2_control_enabled = 1;
}
float target_yaw_Ps2 = 0.0f;
/**
  * @函数  PS2手柄控制处理
  * @参数  无
  * @返回值  无
  *
  * 手柄映射说明:
  * - 左摇杆上下: 控制速度
  * - 右摇杆左右: 控制偏航角 (左=逆时针, 右=顺时针)
  * - START: 开启/关闭手柄控制
  * - SELECT: 重置所有参数
  * - L1/R1: 微调偏航角 (L1=减小, R1=增大)
  * - L2/R2: 微调速度 (L2=减小, R2=增大)
  */
uint8 flag = 0;
void PS2_Control_Process(void)
{
//    static uint8_t target_count =0;
    // 读取手柄数据
    AX_PS2_ScanKey(&ps2_joy);

    // START按键: 开启/关闭手柄控制
//    if(PS2_IsKeyPressed_Edge(ps2_joy.btn1, PS2_GetCurrent()->btn1, PS2_START))
//    {
//        ps2_control_enabled = !ps2_control_enabled;
//    }

    // SELECT按键: 重置所有参数
    if(PS2_IsKeyDown(ps2_joy.btn1, PS2_SELECT))
    {
        ps2_yaw_target = 0.0f;
        ps2_speed_target = star_speed;
        flag =1;
    }

    if(ps2_control_enabled)
    {
//        // 左摇杆上下: 控制速度
//        int8_t left_joy_ud = PS2_GetJoyValue(ps2_joy.LJoy_UD);
//        ps2_speed_target = -(float)left_joy_ud * 0.1f;  // 映射到-10到10的速度范围

        // 右摇杆左右: 控制偏航角
//        if(flag){
            int8_t right_joy_lr = PS2_GetJoyValue(ps2_joy.RJoy_LR);
            ps2_yaw_target += (float)right_joy_lr * 0.0055f;  // 每次增减 0.005
//        }

        if(PS2_IsKeyDown(ps2_joy.btn1, PS2_LEFT))
//            ps2_speed_target = 10.0f;
            ps2_yaw_target = -45.0f;
        if(PS2_IsKeyDown(ps2_joy.btn1, PS2_RIGHT))
//            ps2_speed_target = 20.0f;
            ps2_yaw_target = 45.0f;
        if(PS2_IsKeyDown(ps2_joy.btn1, PS2_DOWN))
            ps2_speed_target = MIN_SPEED;
        if(PS2_IsKeyDown(ps2_joy.btn1, PS2_UP))
            ps2_speed_target = MAX_SPEED;
//        ps2_speed_target =LimitFloat(ps2_speed_target,8.0,20.0);

        // 边界
        if(ps2_yaw_target > 360.0f)
            ps2_yaw_target = 360.0f;
        if(ps2_yaw_target < -360.0f)
            ps2_yaw_target = -360.0f;
        // 使能
        if(PS2_IsKeyDown(ps2_joy.btn2, PS2_TRIANGLE)){
            qidong = 0; //关闭电机
            Buzzer_Beep_Once();
        }
        if(PS2_IsKeyDown(ps2_joy.btn2, PS2_CIRCLE)){
            N.End_f=1;//完成记录
            Buzzer_Beep_Once();
        }
        if (PS2_IsKeyDown(ps2_joy.btn2, PS2_R1)){
//            Encoder_Distance_Reset();  // 重置距离计数
            if(Nag_Load_Flash_To_Array()){
              N.Nag_SystemRun_Index = 2;
              N.Mileage_All = 0;
              N.Nag_Stop_f = 0;
              N.Save_count = 0;
            }else{
                printf("Load ERROR!\n");
            }
//            N.Nag_SystemRun_Index=2; //复现路径
//            N.Flash_page_index = Nag_Start_Page;   // 从起始页开始
//            N.Flash_read_f = 0;                     // 强制重新读取
//            N.size = 0;
//            N.Save_count = 0;
//            N.Mileage_All = 0;
//            N.Nag_Stop_f = 0;
        }
        if (PS2_IsKeyDown(ps2_joy.btn2, PS2_CROSS)){
            N.Nag_SystemRun_Index=1; //开始记录
//            Buzzer_Beep_Once();
        }
        if (PS2_IsKeyDown(ps2_joy.btn2, PS2_L1)){
            ps2_yaw_target = -180.0f;
//            Buzzer_Byeep_Once();
        }
//        if (PS2_IsKeyDown(ps2_joy.btn2, PS2_R2)){
//            target_yaw_Ps2 = 180.0f;
//            Buzzer_Beep_Once();
//        }

    }
    else
    {
        // 禁用时停止
        Motor_Set(0);
        Servo_Set(0);
    }
}

/**
  * @函数  获取当前目标偏航角
  * @参数  无
  * @返回值  目标偏航角
  */
float PS2_Get_Yaw_Target(void)
{
    return ps2_yaw_target;
}

/**
  * @函数  获取当前目标速度
  * @参数  无
  * @返回值  目标速度
  */
float PS2_Get_Speed_Target(void)
{
    return ps2_speed_target;
}

/**
  * @函数  获取手柄控制使能状态
  * @参数  无
  * @返回值  1=使能, 0=禁用
  */
uint8_t PS2_Get_Control_Enabled(void)
{
    return ps2_control_enabled;
}

/**
  * @函数  手动设置目标偏航角
  * @参数  yaw: 目标偏航角
  * @返回值  无
  */
void PS2_Set_Yaw_Target(float yaw)
{
    ps2_yaw_target = yaw;
    if(ps2_yaw_target > 180.0f)
        ps2_yaw_target -= 360.0f;
    if(ps2_yaw_target < -180.0f)
        ps2_yaw_target += 360.0f;
}

/**
  * @函数  手动设置目标速度
  * @参数  speed: 目标速度
  * @返回值  无
  */
//void PS2_Set_Speed_Target(float speed)
//{
//    if(speed > 10.0f)
//        ps2_speed_target = 10.0f;
//    else if(speed < -10.0f)
//        ps2_speed_target = -10.0f;
//    else
//        ps2_speed_target = speed;
//}
