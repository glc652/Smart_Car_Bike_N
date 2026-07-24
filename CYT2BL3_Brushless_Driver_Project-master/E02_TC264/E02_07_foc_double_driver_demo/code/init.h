/*
 * init.h
 *
 *  Created on: 2026年2月11日
 *      Author: 19740
 */

#ifndef CODE_INIT_H_
#define CODE_INIT_H_

#include "zf_common_headfile.h"
//欧拉角信息
typedef struct{
    float pitch;
    float roll;
    float yaw;
}eulerianAngles_t;//欧拉角

//三轴角速度
typedef struct
{
        double yaw_gyro;//绕z轴角速度
        double pitch_gyro;//绕y轴角速度，用于角速度环
        double roll_gyro;//绕x轴角速度。
}threeaxis_velocity;
typedef struct
{
        float a;
        float b;
        float c;
}Dynamic;//动态四元数结算参数
typedef struct
{
        float kp;
        float ki;
}Quaternion;//四元数姿态结算参数
typedef struct
{
        float ratio1;
        float ratio2;
}Complementary_fliter;

typedef struct
{
        //姿态角结算参数
        Dynamic ahrs_kp;
        Quaternion quarterion;
//        //控制参数
//        double Angle_zero;
//        float Speed_set;
        //欧拉角
        eulerianAngles_t eulerAngle;
        threeaxis_velocity gyro;
        float angle[3];

//        //转向相关
//        double bias_yaw;//GPS结算出的方向角和陀螺仪的偏差
//        double bias_location;//GPS结算经纬度飘逸
//        uint32 midvalue;//转向中值
//        float dynamic_angle_kp1;//动态零点参数
//        float dynamic_angle_kp2;//动态零点参数
//        Complementary_fliter com_fliter[3];//GPS与IMU偏航角融合


}SYSTEM_PARAMETER;

void Init_para(void);



#endif /* CODE_INIT_H_ */
