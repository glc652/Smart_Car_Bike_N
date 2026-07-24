/*
 * init.c
 *
 *  Created on: 2026年2月11日
 *      Author: 19740
 */
#include "init.h"
SYSTEM_PARAMETER sys_para;
void Init_para(void)
{
    //姿态角求解参数初始化
    sys_para.ahrs_kp.a=6.0f;
    sys_para.ahrs_kp.b=4.0f;
    sys_para.ahrs_kp.c=2.0f;
    sys_para.quarterion.ki=0.11f;
    sys_para.quarterion.kp=1.6f;

    //姿态角初始化
    //姿态角初始化
    sys_para.gyro.pitch_gyro=0.0f;
    sys_para.gyro.yaw_gyro=0.0f;
    sys_para.gyro.roll_gyro=0.0f;
    sys_para.eulerAngle.pitch=0.0f;
    sys_para.eulerAngle.yaw=0.0f;
    sys_para.eulerAngle.roll=0.0f;
}




