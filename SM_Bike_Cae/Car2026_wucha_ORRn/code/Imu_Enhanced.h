/*
 * Imu_Enhanced.h
 *
 * 增强型姿态系统头文件
 *
 * Created on: 2026-06-01
 * Author: Kiro
 */

#ifndef CODE_IMU_ENHANCED_H_
#define CODE_IMU_ENHANCED_H_

#include "zf_common_headfile.h"

//===================================================Typedef===================================================

typedef struct
{
    float Xdata;
    float Ydata;
    float Zdata;
} Gyro_Param_t;

typedef struct
{
    float acc_x;
    float acc_y;
    float acc_z;
    float gyro_x;
    float gyro_y;
    float gyro_z;
    float Gyro_y;
    float Gyro_z;
} IMU_Param_t;

typedef struct
{
    float RollAngle;
    float PitchAngle;
    float YawAngle;
} Gyro_AngleTypeDef;

//===================================================External Variables===================================================

extern Gyro_AngleTypeDef Gyro_Angle;
extern Gyro_Param_t GyroOffset;
extern IMU_Param_t IMU_Data;
extern float YawAngle_Trans;
extern float Yaw_Angle, Pitch_Angle, Roll_Angle;
extern float Yaw_Angle_non;

//===================================================Function Declarations===================================================

// 初始化和校准
void Gyro_Offset_Init(void);

// 主处理函数
void Gyro_Get_All_Angles(void);

// 获取姿态角
float Gyro_RollAngle_Get(void);
float Gyro_PitchAngle_Get(void);
float Gyro_YawAngle_Get(void);


// 重置
void Gyro_Reset_Attitude(void);

// 参数调整(用于调试)
void IMU_Set_Kp(float kp);
void IMU_Set_Ki(float ki);
void IMU_Set_Roll_Kp(float kp);
void IMU_Set_Roll_Ki(float ki);
void IMU_Set_Pitch_Kp(float kp);
void IMU_Set_Pitch_Ki(float ki);

#endif /* CODE_IMU_ENHANCED_H_ */
