/*
 * encoder_fusion.h
 * 编码器与IMU融合模块 - 增强姿态导航
 *
 * Created on: 2026-05-31
 * Author: 19740
 *
 * 功能说明：
 * 1. 将编码器距离与IMU Yaw角融合
 * 2. 提供精准的位置和方向估计
 * 3. 支持权重调整以适应不同场景
 */

#ifndef CODE_ENCODER_FUSION_H_
#define CODE_ENCODER_FUSION_H_

#include "stdint.h"
// 融合配置
// 结构体类型声明
typedef struct{
    float encoder_distance;
    float encoder_velocity;
    float gyro_yaw;
    float fusion_distance;
    float fusion_x;
    float fusion_y;
    float encoder_weight;
    float gyro_weight;
} FusionState_t;

// 声明全局变量
extern FusionState_t fusion_state;

// 初始化编码器与IMU融合模块
void Encoder_Gyro_Fusion_Init(float encoder_weight, float gyro_weight);

// 更新融合数据
void Encoder_Gyro_Fusion_Update(void);

// 获取融合后的距离
float Encoder_Gyro_Fusion_GetDistance(void);

// 获取融合后的X坐标
float Encoder_Gyro_Fusion_GetX(void);

// 获取融合后的Y坐标
float Encoder_Gyro_Fusion_GetY(void);

// 获取融合后的Yaw角
float Encoder_Gyro_Fusion_GetYaw(void);

// 获取编码器速度
float Encoder_Gyro_Fusion_GetVelocity(void);

// 重置融合状态
void Encoder_Gyro_Fusion_Reset(void);

// 设置编码器权重
void Encoder_Gyro_Fusion_SetEncoderWeight(float weight);

#endif /* CODE_ENCODER_FUSION_H_ */
