/*
 * Motor.c
 *
 *  Created on: 2025年12月14日
 *      Author: 19740
 */
//===================================================Include===================================================
#include "small_driver_uart_control.h"
#include "Motor.h"
//===================================================定义===================================================
//===================================================变量===================================================
int8 duty = 0;
//float duty = 0;
bool dir = true;
//===================================================内部调用===================================================
//===================================================外部调用===================================================
/*
 * 电机初始化
 */
void Motor_Init(void){
    small_driver_uart_init();       // 初始化驱动通讯功能
}

/*
 *电机速度设置
 *float PWMduty-PWM波占空比，对应-100.0-100.0的速度
 */
void Motor_Set(float PWMduty){
    if (PWMduty >= 0){
        dir = 1;
        duty = - PWMduty;
    }
    else if (PWMduty < 0){
        dir = 0;
        duty = PWMduty;
    }
    small_driver_set_duty(duty * 100);   // 计算占空比输出
}

void Motor_Disable(void){
    duty = 0;
    small_driver_set_duty(duty * 100 );   // 计算占空比输出
}

////===================================================Include===================================================
//#include "Motor.h"
//
//#include "zf_driver_pwm.h"
//#include "zf_driver_gpio.h"
////===================================================定义===================================================
//#define Motor_Dir             (P02_4)   // 电机方向输出端口
//#define Motor_PWM             (ATOM0_CH5_P02_5) // PWM输出端口
//
////===================================================变量===================================================
////===================================================内部调用===================================================
////===================================================外部调用===================================================
///*
// * 电机初始化
// */
//void Motor_Init(void){
//    pwm_init(Motor_PWM, 1000, 0);                                                 // PWM 通道2 初始化频率1KHz 占空比初始为0
//    gpio_init(Motor_Dir, GPO, GPIO_HIGH, GPO_PUSH_PULL);                          // 初始化电机方向输出引脚
//
//}
//
///*
// *电机速度设置
// *float PWMduty-PWM波占空比，对应-100-100的速度
// */
//void Motor_Set(float PWMduty){
//    int Forward = 0;
//    float Pwm_Temp = 0;
//    float Pwm_Set = 0;
//    //判断输入的PWM的正负
//    if(PWMduty < 0)
//    {
//        Pwm_Temp = -PWMduty;
//        Forward = 0; //向后
//    }
//    else if(PWMduty >= 0)
//    {
//        Pwm_Temp = PWMduty;
//        Forward = 1;
//    }
//
//    Pwm_Set = Pwm_Temp *100.0;//归一化
//
//    if(Pwm_Set > 10000)//限幅
//    {
//        Pwm_Set = 9999;
//    }
//    if(Forward)// 正转
//    {
//        pwm_set_duty(Motor_PWM, (uint32)Pwm_Set);
//        gpio_set_level(Motor_Dir, 1);
//    }
//    else if (!Forward) // 反转
//    {
//        pwm_set_duty(Motor_PWM, (uint32)Pwm_Set);
//        gpio_set_level(Motor_Dir, 0);
//    }
//}
//
///*
// * 电机失能
// */
//void Motor_Disable(void){
//    pwm_set_duty(Motor_PWM, 0);
//}
//
