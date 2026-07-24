/*
 * Kemu1.c
 * note:科目1
 *  Created on: 2026年6月5日
 *      Author: 19740
 */
#include "zf_common_headfile.h"
#include "Kemu1.h"
uint32 time = 0;
static float kemu1_motor_cmd = 0.0f;

float Kemu1_yaw_control(float yaw_goal){
    float current_yaw = Gyro_YawAngle_Get();
    yaw_put = GetPIDValue(&Yaw_PID,AngleErrorNormalize(StepApproach(yaw_goal,2)-current_yaw),0);
    return (yaw_put);
}
//float Kemu1_yaw_control2(float yaw_goal){
//    float current_yaw = Gyro_YawAngle_Get();
//    yaw_put = GetPIDValue(&Yaw2_PID,AngleErrorNormalize(StepApproach(yaw_goal,3)-current_yaw),0);
//    return (yaw_put);
//}
//float roll_control2(float roll_goal){
//    float temp = 0;
//    temp = GetPIDValue(&Angle2_PID,Gyro_RollAngle_Get(),roll_goal); //内环
//    return temp;
//}
//void gyro_y_control2(float gyro_y){
//    float temp = 0;
//    temp = GetPIDValue(&Gyro2_PID,IMU_Data.Gyro_y,gyro_y); //外环
//    Servo_Set(temp);
//}
void kemu1(void){
    static uint16 count =0;
    //1ms
    if(qidong){
        gyro_y_control(angle_put); // 角速度环
    }else{
        Servo_Set(0);
    }
    //5ms
    if (time % 5 == 0)
    {
//        Balance_Gain_Update(); //自适应修改Kp
        angle_put = roll_control(yaw_put); // 平衡控制
        if (!protection && qidong){
//            if (count <=500){
//                Set_Car_Speed(15);
//            }else if(count <=600){
//                Set_Car_Speed(15);
//            }else if (count <=800){
//                Set_Car_Speed(10);
//            }else if(count <=1000){
//                Set_Car_Speed(15);
//            }else{
//                Set_Car_Speed(15);
//            }
            Set_Car_Speed_Old(8);
        } else {
            Servo_Set(0);
            Motor_Disable();
        }
    }
    //20ms
    if (time % 20 == 0)
    {
//        count++;
        time = 0;
//        if (count <= 650){
//            yaw_put = GetPIDValue(&Yaw_PID,AngleErrorNormalize(StepApproach(0,2)-Gyro_YawAngle_Get()),0);
//        }else if(count <=700){
//            yaw_put = GetPIDValue(&Yaw_PID,AngleErrorNormalize(StepApproach(60,2)-Gyro_YawAngle_Get()),0);
//        }else if(count <=800){
//            yaw_put = GetPIDValue(&Yaw_PID,AngleErrorNormalize(StepApproach(120,2)-Gyro_YawAngle_Get()),0);
//        }
//        else{
//            yaw_put = GetPIDValue(&Yaw_PID,AngleErrorNormalize(StepApproach(180,2)-Gyro_YawAngle_Get()),0);
//        }
        if(N.Nag_SystemRun_Index==2){
            if (N.Angle_Run >0){
                yaw_put = Kemu1_yaw_control(N.Angle_Run);
            }else{
                yaw_put = Kemu1_yaw_control(N.Angle_Run);
            }
        }
//        else{
//            yaw_put =Kemu1_yaw_control(ps2_yaw_target);
//        }
    }
}
