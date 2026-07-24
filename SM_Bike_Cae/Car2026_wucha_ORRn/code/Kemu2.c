/*
 * Kemu2.c
 *
 *  Created on: 2026年6月5日
 *      Author: 19740
 */
#include "zf_common_headfile.h"
#include "Kemu1.h"
#include "Kemu2.h"

float target_yaw2 = 0;
static float nag_yaw_cmd = 0.0f;
static uint8 nag_yaw_cmd_init = 0;

void Set_Car_Speed_Kemu2(float speed){
    static float temp;
    temp = Get_Incremental_PID_Value(&Speed_PID, speed - Encoder_Speed);
    temp1 = Encoder_Speed;//调试用
    temp2 = speed;
    Motor_Set(temp);
}
void Kemu2_gyro_y_control(float gyro_y){
    float temp = 0;
//    if (ps2_speed_target==MAX_SPEED){
//        temp = GetPIDValue(&Gyro_PID,IMU_Data.Gyro_y,gyro_y); //外环
//    }else{
        temp = GetPIDValue(&Gyro_PID,IMU_Data.Gyro_y,gyro_y); //外环
//    }
    Servo_Set(temp);
}
void kemu2(void){
    if(qidong){
        Servo_Set(angle_put);
    }else{
        Servo_Free();
    }
    //5ms
    if (time % 5 == 0)
    {
        angle_put = roll_control(yaw_put); // 平衡控制
        if (!protection && qidong && !N.End_f){
            Set_Car_Speed_Kemu2(3.2);
//            Motor_Set(7);
        } else {
            Servo_Free();
            Motor_Disable();
        }
        if(N.Nag_SystemRun_Index ==2){
            if(!nag_yaw_cmd_init){
              nag_yaw_cmd = N.Angle_Run;
              nag_yaw_cmd_init = 1;
            }
            float err = N.Angle_Run - nag_yaw_cmd;
            if(err > 180.0f) err -= 360.0f;
            else if(err < -180.0f) err += 360.0f;
            if(err > 0.7f || err < -0.7f) {
              nag_yaw_cmd += (err > 0.0f) ? 0.7f : -0.7f;
              if(nag_yaw_cmd > 180.0f) nag_yaw_cmd -= 360.0f;
              else if(nag_yaw_cmd < -180.0f) nag_yaw_cmd += 360.0f;
            } else {
              nag_yaw_cmd = N.Angle_Run;
            }
        }
    }
    //20ms
    if (time % 20 == 0)
    {
        time = 0;
        float current_yaw = Gyro_YawAngle_Get();
        if(N.Nag_SystemRun_Index==2){ //复现路径
//            if (nag_yaw_cmd>0){
//                yaw_put = GetPIDValue(&Yaw2_PID,AngleErrorNormalize(StepApproachAngleMode(nag_yaw_cmd,current_yaw,2,0)-current_yaw),0);
//            }else{
                yaw_put = GetPIDValue(&Yaw_PID,AngleErrorNormalize(StepApproachAngleMode(nag_yaw_cmd,current_yaw,2,0)-current_yaw),0);
//            }
//                if (nag_yaw_cmd <0){
//                    yaw_put *=1.05;
//                }
        }else{//遥控
            nag_yaw_cmd_init = 0;

//                yaw_put = GetPIDValue(&Yaw_PID,AngleErrorNormalize(StepApproachAngleMode(ps2_yaw_target,current_yaw,2,0)-current_yaw),0);
//
        }
    }
}


