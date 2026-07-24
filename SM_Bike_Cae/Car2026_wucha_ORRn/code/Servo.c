/*
 * Servo.c
 *
 *  Created on: 2025年12月11日
 *      Author: 19740
 */
//===================================================Include===================================================
#include "Servo.h"
#include "zf_driver_pwm.h"
#include "PID.h"
#include "zf_device_imu963ra.h"
//===================================================宏定义===================================================
#define SERVO_MOTOR_PWM             (ATOM0_CH1_P33_9)
#define SERVO_MOTOR_FREQ             (300) //50
//#define Servo_Mid                    (55)
//#define Servo_L_MAX                  (0 )
//#define Servo_R_MAX                  (110 )
//#define Servo_Mid_duty               (3420)//570
//#define Servo_L_duty                 (1620)//270
//#define Servo_R_duty                 (5220)//870

#define Servo_Mid_angle             (68)
#define Servo_L_angle               (14)
#define Servo_R_angle               (122)
//===================================================变量===================================================
//===================================================内部函数===================================================
#define SERVO_MOTOR_DUTY(x)         ((float)PWM_DUTY_MAX/(1000.0/(float)SERVO_MOTOR_FREQ)*(0.5+(float)(x)/90.0))

//===================================================外部函数===================================================
/*
 * 初始化舵机
 */
void Servo_Init(void)
{
    pwm_init(SERVO_MOTOR_PWM, SERVO_MOTOR_FREQ, SERVO_MOTOR_DUTY(Servo_Mid_angle));
}

/*
 * 释放舵机（停止输出PWM保持力，可被外力自由转动）
 */
void Servo_Free(void)
{
    Servo_Set(0);
}

/*
 * 设置舵机角度
 * int32 angle_duty 范围为0 左为负 右为正 270-570-870 (-300,300)
 */
void Servo_Set(int32 angle)
{
//    angle_duty += Servo_Mid_duty;
//    if (angle_duty < Servo_L_duty) angle_duty = Servo_L_duty;
//    if (angle_duty > Servo_R_duty) angle_duty = Servo_R_duty;
//    pwm_set_duty(SERVO_MOTOR_PWM, (uint32)angle_duty);
    angle += Servo_Mid_angle;//Servo_Mid_angle;
    if(angle>=Servo_R_angle){angle=Servo_R_angle;}//限幅
    if(angle<=Servo_L_angle){angle=Servo_L_angle;}
    pwm_set_duty(SERVO_MOTOR_PWM, (uint32)SERVO_MOTOR_DUTY(angle));
}
