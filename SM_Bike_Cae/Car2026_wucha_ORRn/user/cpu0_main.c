/*********************************************************************************************************************
* TC264 Opensourec Library 开源TC264 （一寻科技官方 SDK 接口开源库）
* Copyright (c) 2022 SEEKFREE 寻科技
*
* 本文件为 TC264 开源库源码文件
*
* TC264 开源库 开源协议
* 本开源库遵循 GPL 通用公共许可证第三版
* 选择使用 GPL3.0 版权协议
*
* 未经版权所有者允许不得修改、转载
* 仅供个人学习使用，不提供商业用途安全保证
* 详细请看 GPL
*
* 你应收到一份 GPL 版权协议
* 未收到请查看 <https://www.gnu.org/licenses/>
*
* 重要提示
* 本开源库使用 GPL3.0 开源库许可证协议 未注明版本
* 声明内容在 libraries/doc 文件夹内 GPL3_permission_statement.txt 文件
* 确保文件夹位置 libraries 文件夹内 对应文件 LICENSE
* 欢迎各位使用官方开源库 使用时请保留寻科技影像标识
*
* 文件功能          cpu0_main
* 公司信息          成都寻科技有限公司
* 版本信息          查看 libraries/doc 文件夹内 version 文件 版本说明
* 开发环境          ADS v1.10.2
* 适用平台          TC264D
* 店铺链接          https://seekfree.taobao.com/
*
* 修改记录
* 日期              作者                备注
* 2022-09-15       pudding            first version
********************************************************************************************************************/
#include "zf_common_headfile.h"
#include "Imu.h"
#include "Menu_PID_Tuning.h"
#include "PID_Flash_Storage.h"
#include "ps2_control.h"
#include "Car_control.h"
#include "encode.h"
#include "navigation.h"
#include "flash.h"


#pragma section all "cpu0_dsram"
// 全局变量/数组定义 放在 #pragma section all restore 之前 全部放到CPU0内存

// **************************** 外部变量 ****************************
JOYSTICK_TypeDef my_joystick;  // 摇杆值结构体
uint8 ctrol_flag =0;
void IMU_Init(void)
{
    while (1)
    {
        if (imu660rb_init())
            system_delay_ms(1000);
        else
            break;
        system_delay_ms(1000);
    }
    system_delay_ms(100);
}

void PIT_Init(void)
{
    pit_ms_init(CCU60_CH0, 1);     // 初始化 CCU6_0_CH0 定时器中断 1ms
    pit_ms_init(CCU60_CH1, 10);    // 初始化 CCU6_0_CH1 定时器中断 10ms
}

// PID菜单系统初始化
void pid_menu_system_init(void)
{
    ips200_init(IPS200_TYPE_SPI);
    ips200_set_dir(IPS200_PORTAIT);
    ips200_set_color(RGB565_BLACK, RGB565_WHITE);
    PID_Tuning_Menu_Init();
    PID_Flash_Storage_Init();
    if(PID_Flash_Load_All_PID(&Speed_PID, &Angle_PID, &Gyro_PID, &Yaw_PID) == 1) {
        printf("PID LOAD FLASH SUCCESSFULL!\n");
    }else{
        printf("PID LOAD FLASH ERROR!\n");
    }
}

void Code_Init(void)
{
    wireless_uart_init();
    pid_menu_system_init();
    LED_Init();
    Key_Init();
    Servo_Init();
    IMU_Init();
    Gyro_Offset_Init();
    Encode_Init();
    Motor_Init();
    PS2_Control_Init();
    Init_Nag();
    All_PID_Init();
    qidong = 1;
    time = 0;
    PIT_Init();
}
//===============MAIN 函数==============
int core0_main(void)
{
    clock_init();                   // 系统时钟初始化
    debug_init();                   // 初始化默认调试串口
//    Flash_Debug_Print_Pages(0, 11, 0, 16);
    // 通用初始化函数
    Code_Init();
//    Flash_Debug_Print_Page(3,0,256);
//   system_delay_ms(3000);
//    N.Nag_SystemRun_Index = 2;

    // 等待所有CPU启动
    cpu_wait_event_ready();
    while (TRUE)
    {
        car_safety();
        Buzzer_Process();
        PID_Menu_Key_Process();
        PID_Tuning_Menu_Show();
        Nag_Flash_Task();
//        printf("ps2_yaw_target:%f\n",ps2_yaw_target);
//        clear_flash_data();
//        Navigation_Print_XY();
//        printf("%d,%f,%f\n",PS2_Get_Control_Enabled(),PS2_Get_Yaw_Target(),PS2_Get_Speed_Target());
//        printf("%f,%f\n", temp2,temp1);
//        printf("%f\n",ps2_yaw_target);
//        printf("%f,%f\n",IMU_Data.Gyro_y,IMU_Data.Gyro_z);
//        printf("Yaw_Angle:%f Pitch_Angle:%f Roll_Angle:%f\r\n",Gyro_YawAngle_Get(),Gyro_PitchAngle_Get(),Gyro_RollAngle_Get());
    }
}

#pragma section all restore
// **************************** 中断服务函数 ****************************

//===================================================PIT 定时器中断===================================================
/*
 * 1ms定时器中断
 */
IFX_INTERRUPT(cc60_pit_ch0_isr, 0, CCU6_0_CH0_ISR_PRIORITY)
{
    interrupt_global_enable(0);                     // 开启中断嵌套
    pit_clear_flag(CCU60_CH0);
    time++;
    if (time%5 ==0){
        Gyro_Get_All_Angles();
        Encoder_Process();
        PS2_Control_Process();
//        Ctrl_main();
    }
    if (time%20 ==0){
        Nag_System();
    }
    if (ctrol_flag ==0){
//        kemu1();
//        kemu2();
        kemu3();
    }
}

/*
 * 10ms定时器中断
 * 每10ms: 获取电池电压
 * 每300ms LED闪烁 指示系统运行状态
 */
IFX_INTERRUPT(cc60_pit_ch1_isr, 0, CCU6_0_CH1_ISR_PRIORITY)
{
//    static uint8_t time3 = 0;
    interrupt_global_enable(0);                     // 开启中断嵌套
    pit_clear_flag(CCU60_CH1);
    //10ms
//    get_battery();
    //300ms
//    if (time3 == 30)
//    {
//        LED_Shine();
//        time3 = 0;
//    }
//    time3 ++;
}
