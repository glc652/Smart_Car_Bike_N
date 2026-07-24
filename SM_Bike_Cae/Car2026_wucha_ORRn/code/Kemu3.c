/*
 * Kemu3.c
 *
 *  Created on: 2026年6月13日
 *      Author: 19740
 */
#include "Kemu3.h"
#include "zf_common_headfile.h"

float Kemu3_yaw_control(float yaw_goal){
    float current_yaw = Gyro_YawAngle_Get();
    yaw_put = GetPIDValue(&Yaw_PID,AngleErrorNormalize(StepApproach(yaw_goal,2)-current_yaw),0);
    return (yaw_put);
}
void Kemu3_gyro_y_control(float gyro_y){
    float temp = 0;
//    if (ps2_speed_target==MAX_SPEED){
//        temp = GetPIDValue(&Gyro_PID,IMU_Data.Gyro_y,gyro_y); //外环
//    }else{
        temp = GetPIDValue(&Gyro_PID,IMU_Data.Gyro_y,gyro_y); //外环
//    }
    Servo_Set(temp);
}
float Kemu3_roll_control(float roll_goal){
    float temp = 0;
//    if (ps2_speed_target==MAX_SPEED){
//        temp = GetPIDValue(&Angle_PID,Gyro_RollAngle_Get(),roll_goal); //内环
//    }
//    else{
        temp = GetPIDValue(&Angle_PID,Gyro_RollAngle_Get(),roll_goal); //内环
//    }
    return temp;
}

float ps2_yaw_target_beyond = 0.0f;
void kemu3(void){
    //1ms
    if(qidong){
        Kemu3_gyro_y_control(angle_put); // 角速度环
    }else{
        Servo_Set(0);
    }
    //5ms
    if (time % 5 == 0)
    {
//        Balance_Gain_Update(); //自适应修改Kp
        angle_put = Kemu3_roll_control(yaw_put); // 平衡控制
        if (!protection && qidong){
//            Set_Car_Speed(ps2_speed_target); //ps2_speed_target
            Set_Car_Speed_Old(8); //6
        } else {
            Servo_Set(0);
            Motor_Disable();
        }
    }
    //20ms
    if (time % 20 == 0)
    {
        time = 0;
        if(N.Nag_SystemRun_Index==2){
            if (N.Angle_Run >0){
                yaw_put = Kemu3_yaw_control(N.Angle_Run);
            }else{
                yaw_put = Kemu3_yaw_control(N.Angle_Run);
            }
        }
//        else{
//            yaw_put =Kemu3_yaw_control(ps2_yaw_target);
//        }
    }
}




