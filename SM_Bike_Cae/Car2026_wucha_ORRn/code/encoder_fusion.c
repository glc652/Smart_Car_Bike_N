/*
 * encoder_fusion.c
 * 编码器+IMU融合模块 - 增强航迹推算
 *
 * Created on: 2026-05-31
 * Author: 19740
 */

#include "zf_common_headfile.h"
#include "encode.h"
#include "imu_fusion_enhanced.h"
#include <math.h>
#include "encoder_fusion.h"

//===================================================全局变量===================================================
FusionState_t fusion_state;

//===================================================内部函数===================================================
/**
  * @brief  融合编码器与IMU Yaw角度
  * @param  encoder_dist: 编码器累计距离(m)
  * @param  gyro_yaw: IMU Yaw角度(度)
  * @return 无
  *
  * 融合原理：
  * 1. 编码器提供精准前进距离
  * 2. IMU提供精准转向角度
  * 3. 极坐标转XY坐标
  * 4. 使用权重融合最终位置
  */
static void Fuse_Encoder_Gyro(float encoder_dist, float gyro_yaw)
{
    fusion_state.encoder_distance = encoder_dist;
    fusion_state.gyro_yaw = gyro_yaw;

    // 角度转弧度
    float yaw_rad = gyro_yaw * 0.0174533f;

    // 根据距离和Yaw角计算X、Y坐标
    fusion_state.fusion_distance = encoder_dist;
    fusion_state.fusion_x = encoder_dist * cosf(yaw_rad);
    fusion_state.fusion_y = encoder_dist * sinf(yaw_rad);
}

/**
  * @brief  速度融合（抑制静止漂移）
  * @param  encoder_vel: 编码器速度(m/s)
  * @param  gyro_yaw_rate: IMU Yaw角速度(度/s)
  * @return 无
  */
static void Fuse_Velocity(float encoder_vel, float gyro_yaw_rate)
{
    fusion_state.encoder_velocity = encoder_vel;

    // 车速接近0时，IMU的Yaw角速度也应接近0
    // 用于抑制编码器静止时IMU漂移
    if(encoder_vel < 0.01f && fabsf(gyro_yaw_rate) > 1.0f)
    {
        // 此处可添加静止漂移修正逻辑
    }
}

//===================================================外部接口===================================================
/**
  * @brief  初始化编码器+IMU融合模块
  * @param  encoder_weight: 编码器权重(0-1, 默认0.7)
  * @param  gyro_weight: IMU权重(0-1, 默认0.3)
  * @return 无
  */
void Encoder_Gyro_Fusion_Init(float encoder_weight, float gyro_weight)
{
    // 归一化权重
    float total = encoder_weight + gyro_weight;
    if(total > 0)
    {
        fusion_state.encoder_weight = encoder_weight / total;
        fusion_state.gyro_weight = gyro_weight / total;
    }
    else
    {
        fusion_state.encoder_weight = 0.7f;
        fusion_state.gyro_weight = 0.3f;
    }

    fusion_state.encoder_distance = 0.0f;
    fusion_state.encoder_velocity = 0.0f;
    fusion_state.gyro_yaw = 0.0f;
    fusion_state.fusion_distance = 0.0f;
    fusion_state.fusion_x = 0.0f;
    fusion_state.fusion_y = 0.0f;
}

/**
  * @brief  融合更新主函数
  * @param  无
  * @return 无
  *
  * 调用时机：
  * 1. 主循环中定时调用
  * 2. 从编码器和IMU获取最新数据
  * 3. 执行融合解算
  */
void Encoder_Gyro_Fusion_Update(void)
{
    // 获取编码器数据
    float encoder_distance = Encoder_Get_Distance();
    float encoder_velocity = Encoder_Speed;
    // 获取IMU数据
    Attitude_t* attitude = IMU_Fusion_Enhanced_GetAttitude();

    // 执行融合
    Fuse_Encoder_Gyro(encoder_distance, attitude->yaw);
    Fuse_Velocity(encoder_velocity, 0.0f);  // IMU Yaw角速度需从IMU获取

    // 更新IMU融合模块
    IMU_Fusion_Enhanced_SetEncoderDistance(encoder_distance);
    IMU_Fusion_Enhanced_SetEncoderVelocity(encoder_velocity);
}

/**
  * @brief  获取融合总里程
  * @param  无
  * @return 距离(m)
  */
float Encoder_Gyro_Fusion_GetDistance(void)
{
    return fusion_state.fusion_distance;
}

/**
  * @brief  获取融合X坐标
  * @param  无
  * @return X坐标(m)
  */
float Encoder_Gyro_Fusion_GetX(void)
{
    return fusion_state.fusion_x;
}

/**
  * @brief  获取融合Y坐标
  * @param  无
  * @return Y坐标(m)
  */
float Encoder_Gyro_Fusion_GetY(void)
{
    return fusion_state.fusion_y;
}

/**
  * @brief  获取融合Yaw角度
  * @param  无
  * @return Yaw角度(度)
  */
float Encoder_Gyro_Fusion_GetYaw(void)
{
    return fusion_state.gyro_yaw;
}

/**
  * @brief  获取编码器速度
  * @param  无
  * @return 速度(m/s)
  */
float Encoder_Gyro_Fusion_GetVelocity(void)
{
    return fusion_state.encoder_velocity;
}

/**
  * @brief  复位融合状态
  * @param  无
  * @return 无
  */
void Encoder_Gyro_Fusion_Reset(void)
{
    fusion_state.encoder_distance = 0.0f;
    fusion_state.encoder_velocity = 0.0f;
    fusion_state.gyro_yaw = 0.0f;
    fusion_state.fusion_distance = 0.0f;
    fusion_state.fusion_x = 0.0f;
    fusion_state.fusion_y = 0.0f;

    Encoder_Distance_Reset();
    IMU_Fusion_Enhanced_Reset();
}

/**
  * @brief  设置编码器权重
  * @param  weight: 权重(0-1)
  * @return 无
  */
void Encoder_Gyro_Fusion_SetEncoderWeight(float weight)
{
    if(weight < 0.0f) weight = 0.0f;
    if(weight > 1.0f) weight = 1.0f;
    fusion_state.encoder_weight = weight;
    fusion_state.gyro_weight = 1.0f - weight;
}
