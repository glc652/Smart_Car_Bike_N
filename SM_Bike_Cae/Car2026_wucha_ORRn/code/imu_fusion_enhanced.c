/*
 * imu_fusion_enhanced.c
 * 增强型IMU融合算法 - 编码器融合定位
 *
 * Created on: 2026-5-30
 * Author: 19740
 */
#include "zf_common_headfile.h"
#include "imu_fusion_enhanced.h"
#include <math.h>
#include "encoder_fusion.h"

//===================================================全局变量===================================================
static Attitude_t attitude = {0};                    // 当前姿态角
static AngularRate_t angular_rate = {0};            // 当前角速度
static Position_Velocity_t position_velocity = {0}; // 位置速度

#define WHEEL_RADIUS    0.035f    // 车轮半径(m)
// 算法配置
static struct
{
    float dt;                       // 更新时间间隔
    float complementary_factor;     // 互补滤波系数
    float gyro_bias_x;              // 陀螺仪零偏
    float gyro_bias_y;
    float gyro_bias_z;
    float wheel_radius;             // 车轮半径(米)
    float encoder_weight;           // 编码器权重(0-1)
    float last_encoder_speed;       // 上一次编码器速度
} config;

//===================================================内部函数===================================================
/**
  * @brief  根据加速度计算Roll、Pitch角
  * @param  acc: 加速度指针
  * @return 无
  */
static void Calculate_Attitude_From_Accel(Acceleration_t* acc)
{
    float acc_roll, acc_pitch;

    // Roll角：通过Y、Z轴加速度计算
    acc_roll = atan2f(acc->y, acc->z) * 57.2958f;  // 转换为角度

    // Pitch角：通过X轴加速度计算
    acc_pitch = atan2f(-acc->x, sqrtf(acc->y * acc->y + acc->z * acc->z)) * 57.2958f;

    // 互补滤波融合陀螺仪角度和加速度角度
    attitude.roll = config.complementary_factor * attitude.roll +
                    (1.0f - config.complementary_factor) * acc_roll;

    attitude.pitch = config.complementary_factor * attitude.pitch +
                     (1.0f - config.complementary_factor) * acc_pitch;
}

/**
  * @brief  陀螺仪角度积分
  * @param  gyro: 角速度指针
  * @return 无
  */
static void Integrate_Gyro(AngularRate_t* gyro)
{
    // 去除零偏后积分
    attitude.roll += (gyro->roll_rate - config.gyro_bias_x) * config.dt;
    attitude.pitch += (gyro->pitch_rate - config.gyro_bias_y) * config.dt;
    attitude.yaw += (gyro->yaw_rate - config.gyro_bias_z) * config.dt;

    // 限制Yaw角在-180~180范围
    if(attitude.yaw > 180.0f)
        attitude.yaw -= 360.0f;
    if(attitude.yaw < -180.0f)
        attitude.yaw += 360.0f;
}

/**
  * @brief  使用编码器数据校正Yaw角与位置
  * @param  encoder_speed: 编码器速度(m/s)
  * @return 无
  *
  * 原理：
  * 1. 根据编码器计算实时前进速度
  * 2. 结合Yaw角，计算X、Y轴位移增量
  * 3. 根据速度与角度关系修正Yaw角速度漂移
  */
static void Correct_With_Encoder(float encoder_speed)
{
    // 更新当前速度
    position_velocity.velocity = encoder_speed;

    // 更新总里程
    position_velocity.distance += encoder_speed * config.dt;

    // 根据Yaw角计算X、Y轴位移
    float yaw_rad = attitude.yaw * 0.0174533f;  // 转换为弧度
    position_velocity.x += encoder_speed * cosf(yaw_rad) * config.dt;
    position_velocity.y += encoder_speed * sinf(yaw_rad) * config.dt;

    // 利用编码器校正Yaw漂移
    // 当速度为0时，说明静止，Yaw角应无变化
    // 缓慢修正陀螺仪累计漂移
    if(encoder_speed < 0.01f)  // 速度接近0
    {
        // 使用编码器缓慢校正Yaw角
        // 权重较小，仅修正长期漂移
        attitude.yaw = attitude.yaw * (1.0f - config.encoder_weight * 0.1f);
    }

    config.last_encoder_speed = encoder_speed;
}

//===================================================外部函数===================================================
/**
  * @brief  初始化增强型IMU融合模块
  * @param  dt: 更新时间间隔(秒)
  * @param  complementary_factor: 互补滤波系数(0-1,推荐0.98)
  * @return 无
  */
void IMU_Fusion_Enhanced_Init(float dt, float complementary_factor)
{
    config.dt = dt;
    config.complementary_factor = complementary_factor;
    config.wheel_radius = WHEEL_RADIUS;
    config.encoder_weight = 0.1f;  // 默认权重
    config.gyro_bias_x = 0.0f;
    config.gyro_bias_y = 0.0f;
    config.gyro_bias_z = 0.0f;
    config.last_encoder_speed = 0.0f;

    attitude.roll = 0.0f;
    attitude.pitch = 0.0f;
    attitude.yaw = 0.0f;

    angular_rate.roll_rate = 0.0f;
    angular_rate.pitch_rate = 0.0f;
    angular_rate.yaw_rate = 0.0f;

    position_velocity.x = 0.0f;
    position_velocity.y = 0.0f;
    position_velocity.distance = 0.0f;
    position_velocity.velocity = 0.0f;
}

/**
  * @brief  执行IMU数据融合与编码器融合解算
  * @param  acc: 加速度数据(m/s^2)
  * @param  gyro: 角速度数据(度/秒)
  * @param  encoder_speed: 编码器速度(m/s)
  * @return 无
  *
  * 融合流程：
  * 1. 陀螺仪积分得到姿态角
  * 2. 加速度校正Roll、Pitch
  * 3. 编码器数据校正Yaw角与位置
  */
void IMU_Fusion_Enhanced_Update(Acceleration_t* acc, AngularRate_t* gyro, float encoder_speed)
{
    if(acc == NULL || gyro == NULL)
        return;

    // 保存角速度
    angular_rate = *gyro;

    // 上一时刻角度积分(陀螺仪)
    Integrate_Gyro(gyro);

    // 利用加速度数据校正Roll、Pitch(更准确)
    Calculate_Attitude_From_Accel(acc);

    // 利用编码器数据进行姿态与位置校正
    Correct_With_Encoder(encoder_speed);
}

/**
  * @brief  获取融合后姿态角
  * @param  无
  * @return 指向姿态角结构体的指针
  */
Attitude_t* IMU_Fusion_Enhanced_GetAttitude(void)
{
    return &attitude;
}

/**
  * @brief  获取位置与速度
  * @param  无
  * @return 指向位置速度结构体的指针
  */
Position_Velocity_t* IMU_Fusion_Enhanced_GetPositionVelocity(void)
{
    return &position_velocity;
}

/**
  * @brief  获取Roll角
  * @param  无
  * @return Roll角(度)
  */
float IMU_Fusion_Enhanced_GetRoll(void)
{
    return attitude.roll;
}

/**
  * @brief  获取Pitch角
  * @param  无
  * @return Pitch角(度)
  */
float IMU_Fusion_Enhanced_GetPitch(void)
{
    return attitude.pitch;
}

/**
  * @brief  获取Yaw角
  * @param  无
  * @return Yaw角(度)
  */
float IMU_Fusion_Enhanced_GetYaw(void)
{
    return attitude.yaw;
}

/**
  * @brief  获取当前速度
  * @param  无
  * @return 速度(m/s)
  */
float IMU_Fusion_Enhanced_GetVelocity(void)
{
    return position_velocity.velocity;
}

/**
  * @brief  获取当前里程
  * @param  无
  * @return 里程(米)
  */
float IMU_Fusion_Enhanced_GetDistance(void)
{
    return position_velocity.distance;
}

/**
  * @brief  重置融合状态
  * @param  无
  * @return 无
  */
void IMU_Fusion_Enhanced_Reset(void)
{
    attitude.roll = 0.0f;
    attitude.pitch = 0.0f;
    attitude.yaw = 0.0f;

    position_velocity.x = 0.0f;
    position_velocity.y = 0.0f;
    position_velocity.distance = 0.0f;
    position_velocity.velocity = 0.0f;
}

/**
  * @brief  设置陀螺仪零偏
  * @param  bias_x, bias_y, bias_z: 陀螺仪零偏(度/秒)
  * @return 无
  */
void IMU_Fusion_Enhanced_SetGyroBias(float bias_x, float bias_y, float bias_z)
{
    config.gyro_bias_x = bias_x;
    config.gyro_bias_y = bias_y;
    config.gyro_bias_z = bias_z;
}

/**
  * @brief  设置编码器权重(用于Yaw校正)
  * @param  weight: 权重(0-1,推荐0.1)
  * @return 无
  *
  * 权重说明：
  * - 0.05：编码器影响小，漂移修正慢
  * - 0.10：平衡角度与编码器数据
  * - 0.20：编码器影响大，漂移修正快
  */
void IMU_Fusion_Enhanced_SetEncoderWeight(float weight)
{
    if(weight < 0.0f) weight = 0.0f;
    if(weight > 1.0f) weight = 1.0f;
    config.encoder_weight = weight;
}

/**
  * @brief  设置编码器距离(用于直接输入编码器计算的距离)
  * @param  distance: 编码器距离(m)
  * @return 无
  */
void IMU_Fusion_Enhanced_SetEncoderDistance(float distance)
{
    fusion_state.encoder_distance = distance;
    // 使用编码器距离更新位置
    float yaw_rad = attitude.yaw * 0.0174533f;
    position_velocity.distance = distance;
    position_velocity.x = distance * cosf(yaw_rad);
    position_velocity.y = distance * sinf(yaw_rad);
}

/**
  * @brief  设置编码器速度(用于直接输入编码器计算的速度)
  * @param  velocity: 编码器速度(m/s)
  * @return 无
  */
void IMU_Fusion_Enhanced_SetEncoderVelocity(float velocity)
{
    fusion_state.encoder_velocity = velocity;
    position_velocity.velocity = velocity;
}

/**
  * @brief  获取编码器计算的距离
  * @param  无
  * @return 距离(m)
  */
float IMU_Fusion_Enhanced_GetEncoderDistance(void)
{
    return fusion_state.encoder_distance;
}

/**
  * @brief  获取陀螺仪角速度
  * @param  无
  * @return 指向角速度结构体的指针
  */
AngularRate_t* IMU_Fusion_Enhanced_GetAngularRate(void)
{
    return &angular_rate;
}
