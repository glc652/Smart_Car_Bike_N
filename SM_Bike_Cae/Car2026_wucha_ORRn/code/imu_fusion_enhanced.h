/*
 * imu_fusion_enhanced.h
 * 增强型IMU融合算法 - 编码器融合
 *
 * Created on: 2026-5-30
 * Author: 19740
 *
 * 使用说明：
 * 1. 使用陀螺仪+加速度计进行姿态融合(互补滤波)
 * 2. 使用编码器数据校正Yaw角漂移
 * 3. 使用编码器数据计算前进里程
 */

#ifndef CODE_IMU_FUSION_ENHANCED_H_
#define CODE_IMU_FUSION_ENHANCED_H_

#include "stdint.h"

// 姿态角结构体
typedef struct
{
    float roll;    // 横滚角(左右倾斜)
    float pitch;   // 俯仰角(前后倾斜)
    float yaw;     // 偏航角(转向)
} Attitude_t;

// 角速度结构体
typedef struct
{
    float roll_rate;   // 横滚角速度
    float pitch_rate;  // 俯仰角速度
    float yaw_rate;    // 偏航角速度
} AngularRate_t;

// 加速度结构体
typedef struct
{
    float x;
    float y;
    float z;
} Acceleration_t;

// 位置速度结构体
typedef struct
{
    float x;           // X轴位置(前后方向)
    float y;           // Y轴位置(左右方向)
    float distance;    // 总行驶里程
    float velocity;    // 前进速度
} Position_Velocity_t;

// 初始化增强型IMU融合
void IMU_Fusion_Enhanced_Init(float dt, float complementary_factor);

// 执行IMU+编码器融合更新
void IMU_Fusion_Enhanced_Update(Acceleration_t* acc, AngularRate_t* gyro, float encoder_speed);

// 获取融合后姿态角
Attitude_t* IMU_Fusion_Enhanced_GetAttitude(void);

// 获取位置与速度
Position_Velocity_t* IMU_Fusion_Enhanced_GetPositionVelocity(void);

// 获取Roll角
float IMU_Fusion_Enhanced_GetRoll(void);

// 获取Pitch角
float IMU_Fusion_Enhanced_GetPitch(void);

// 获取Yaw角
float IMU_Fusion_Enhanced_GetYaw(void);

// 获取前进速度
float IMU_Fusion_Enhanced_GetVelocity(void);

// 获取总里程
float IMU_Fusion_Enhanced_GetDistance(void);

// 重置融合状态
void IMU_Fusion_Enhanced_Reset(void);

// 设置陀螺仪零偏
void IMU_Fusion_Enhanced_SetGyroBias(float bias_x, float bias_y, float bias_z);

// 设置编码器权重(用于Yaw校正)
void IMU_Fusion_Enhanced_SetEncoderWeight(float weight);

// 设置编码器距离(直接输入编码器计算的距离)
void IMU_Fusion_Enhanced_SetEncoderDistance(float distance);

// 设置编码器速度(直接输入编码器计算的速度)
void IMU_Fusion_Enhanced_SetEncoderVelocity(float velocity);

// 获取编码器计算的距离
float IMU_Fusion_Enhanced_GetEncoderDistance(void);

// 获取陀螺仪角速度
AngularRate_t* IMU_Fusion_Enhanced_GetAngularRate(void);

#endif /* CODE_IMU_FUSION_ENHANCED_H_ */
