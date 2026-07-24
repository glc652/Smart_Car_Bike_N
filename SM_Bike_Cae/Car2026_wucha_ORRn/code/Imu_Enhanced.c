/*
 * Imu_Enhanced.c
 *
 * 增强型姿态系统 - 稳定性和精度优化版本
 * 结合梯形积分、低通滤波、四元数算法
 *
 * Created on: 2026-06-01
 * Author: Kiro
 */

//===================================================Include===================================================
#include "Imu_Enhanced.h"
#include "zf_device_imu660rb.h"
#include "math.h"
#include "zf_common_headfile.h"

//===================================================Define===================================================
#define delta_T         0.005f          // 采样周期5ms
#define pI              3.1415926f
#define ACC_NORM_MIN    0.1f            // 加速度计归一化最小值

#define acc_trans_value 4098            // 加速度计转换系数
#define gyro_trans_value 14.3           // 陀螺仪转换系数


// 低通滤波参数
#define LPF_CUTOFF_FREQ 15.0f           // 截止频率15Hz
#define LPF_ALPHA       0.0747f         // 滤波系数(预估: 0.005/(1/(2*pi*15)+0.005))

// 梯形积分系数
#define TRAPEZOID_COEFF 0.5f            // 梯形积分系数

//===================================================Typedef===================================================
typedef struct {
    float q0, q1, q2, q3;
} Quaternion_t;

typedef struct {
    float prev_output;
    float alpha;
    uint8_t initialized;
} LowPassFilter_t;

//===================================================Global Variables===================================================
// 四元数
static Quaternion_t q = {1, 0, 0, 0};

// 误差积分
static float I_ex = 0, I_ey = 0, I_ez = 0;

// 互补滤波参数(优化后的参数)
static float IMU_kp = 0.4f;           // 比例增益
static float IMU_ki = 0.015f;         // 积分增益
static float roll_kp = 0.4f;
static float roll_ki = 0.015f;
static float pitch_kp = 0.4f;
static float pitch_ki = 0.015f;

// 陀螺仪偏差
Gyro_Param_t GyroOffset;

// IMU数据
IMU_Param_t IMU_Data;

// 姿态角
Gyro_AngleTypeDef Gyro_Angle;
float YawAngle_Trans = 0;
float Yaw_Angle = 0, Pitch_Angle = 0, Roll_Angle = 0;
float Yaw_Angle_non = 0;

// 上一次Yaw角(用于连续性处理)
static float Yaw_Angle_Old = 0;

// 低通滤波器(用于加速度计)
static LowPassFilter_t acc_filter_x = {0, LPF_ALPHA, 0};
static LowPassFilter_t acc_filter_y = {0, LPF_ALPHA, 0};
static LowPassFilter_t acc_filter_z = {0, LPF_ALPHA, 0};

// 梯形积分用上一次值
// ORRN-style startup convergence timer
static float imu_first_count_time = 0.0f;
static float gyro_y_lpf = 0.0f;
static uint8_t gyro_y_lpf_initialized = 0;

//===================================================Internal Functions===================================================

/**
 * @brief 快速平方根倒数(用于四元数归一化)
 */
static float My_Rsqrt(float num)
{
    float halfx = 0.5f * num;
    float y = num;
    long i = *(long*)&y;
    i = 0x5f375a86 - (i >> 1);
    y = *(float*)&i;
    y = y * (1.5f - (halfx * y * y));
    return y;
}

/**
 * @brief 一阶低通滤波
 * @param filter 滤波器结构体指针
 * @param input 输入值
 * @return 滤波后的输出值
 */

static bool IMU_Is_Static_State(float ax, float ay, float az)
{
    float norm = sqrtf(ax * ax + ay * ay + az * az);
    return (norm >= 0.9f && norm <= 1.1f) ? true : false;
}
static float LPF_Update(LowPassFilter_t *filter, float input)
{
    if (!filter->initialized)
    {
        filter->prev_output = input;
        filter->initialized = 1;
        return input;
    }

    float output = filter->alpha * input + (1.0f - filter->alpha) * filter->prev_output;
    filter->prev_output = output;

    return output;
}

/**
 * @brief 获取IMU原始数据并进行预处理
 */
static void IMU_Get_Values(void)
{
    // 1. 读取原始数据
    imu660rb_get_acc();
    imu660rb_get_gyro();

    // 2. 加速度计处理: 单位转换 + 低通滤波
    float acc_x_raw = ((float)imu660rb_acc_x) / acc_trans_value;
    float acc_y_raw = ((float)imu660rb_acc_y) / acc_trans_value;
    float acc_z_raw = ((float)imu660rb_acc_z) / acc_trans_value;

    IMU_Data.acc_x = acc_x_raw;
    IMU_Data.acc_y = acc_y_raw;
    IMU_Data.acc_z = acc_z_raw;

    // 3. 陀螺仪处理: 去偏差 + 单位转换(转为弧度制)
    float gyro_x_raw = (float)(imu660rb_gyro_x / 10 * 10) - GyroOffset.Xdata;
    float gyro_y_raw = (float)(imu660rb_gyro_y / 10 * 10) - GyroOffset.Ydata;
    float gyro_z_raw = (float)(imu660rb_gyro_z / 10 * 10) - GyroOffset.Zdata;

    IMU_Data.gyro_x = gyro_x_raw * pI / 180.0f / gyro_trans_value;
    IMU_Data.gyro_y = gyro_y_raw * pI / 180.0f / gyro_trans_value;
    IMU_Data.gyro_z = gyro_z_raw * pI / 180.0f / gyro_trans_value;

    // 4. 陀螺仪Y轴(用于平衡控制)
    float gyro_y_dps = gyro_y_raw / gyro_trans_value;
    if (!gyro_y_lpf_initialized)
    {
        gyro_y_lpf = gyro_y_dps;
        gyro_y_lpf_initialized = 1;
    }
    else
    {
        gyro_y_lpf = gyro_y_lpf + 0.1f * (gyro_y_dps - gyro_y_lpf);
    }

    IMU_Data.Gyro_y = gyro_y_lpf;

    IMU_Data.Gyro_z = gyro_z_raw / gyro_trans_value;

    // 6. 阈值处理(消除噪声)
    if (fabsf(IMU_Data.gyro_x) < 0.00815f) IMU_Data.gyro_x = 0;
    if (fabsf(IMU_Data.gyro_y) < 0.00815f) IMU_Data.gyro_y = 0;
    if (fabsf(IMU_Data.gyro_z) < 0.00815f) IMU_Data.gyro_z = 0;
}

/**
 * @brief 四元数AHRS更新(Madgwick算法)
 */
static void IMU_AHRS_Update(void)
{
    float gx = IMU_Data.gyro_x;
    float gy = IMU_Data.gyro_y;
    float gz = IMU_Data.gyro_z;
    float ax = IMU_Data.acc_x;
    float ay = IMU_Data.acc_y;
    float az = IMU_Data.acc_z;
    float norm;
    float vx, vy, vz;
    float ex, ey, ez;
    float kp_scale;
    float integral_gain;
    float acc_err_limit = 1.0f;
    bool static_state = IMU_Is_Static_State(ax, ay, az);

    if(!acc_filter_x.initialized || !acc_filter_y.initialized || !acc_filter_z.initialized)
    {
        acc_filter_x.prev_output = ax;
        acc_filter_y.prev_output = ay;
        acc_filter_z.prev_output = az;
    }

    /* ORRN style: static state uses stronger smoothing, dynamic state follows faster. */
    float acc_alpha = static_state ? 0.8f : 0.5f;
    ax = acc_alpha * acc_filter_x.prev_output + (1.0f - acc_alpha) * ax;
    ay = acc_alpha * acc_filter_y.prev_output + (1.0f - acc_alpha) * ay;
    az = acc_alpha * acc_filter_z.prev_output + (1.0f - acc_alpha) * az;
    acc_filter_x.prev_output = ax;
    acc_filter_y.prev_output = ay;
    acc_filter_z.prev_output = az;
    acc_filter_x.initialized = 1;
    acc_filter_y.initialized = 1;
    acc_filter_z.initialized = 1;

    norm = sqrtf(ax * ax + ay * ay + az * az);
    if(norm < ACC_NORM_MIN)
    {
        ax = 0.0f;
        ay = 0.0f;
        az = 1.0f;
    }
    else
    {
        ax /= norm;
        ay /= norm;
        az /= norm;
    }

    vx = 2.0f * (q.q1 * q.q3 - q.q0 * q.q2);
    vy = 2.0f * (q.q0 * q.q1 + q.q2 * q.q3);
    vz = q.q0 * q.q0 - q.q1 * q.q1 - q.q2 * q.q2 + q.q3 * q.q3;

    ex = ay * vz - az * vy;
    ey = az * vx - ax * vz;
    ez = ax * vy - ay * vx;

    kp_scale = static_state ? 1.0f : 0.8f;
    if(imu_first_count_time < 0.1f)
    {
        imu_first_count_time += delta_T;
        kp_scale = 100.0f;
    }

    integral_gain = static_state ? 1.0f : 0.1f;
    I_ex += ex * delta_T * integral_gain;
    I_ey += ey * delta_T * integral_gain;
    I_ez += ez * delta_T * integral_gain;

    if(I_ex > acc_err_limit) I_ex = acc_err_limit;
    if(I_ex < -acc_err_limit) I_ex = -acc_err_limit;
    if(I_ey > acc_err_limit) I_ey = acc_err_limit;
    if(I_ey < -acc_err_limit) I_ey = -acc_err_limit;
    if(I_ez > acc_err_limit) I_ez = acc_err_limit;
    if(I_ez < -acc_err_limit) I_ez = -acc_err_limit;

    gx += (roll_kp * kp_scale) * ex + roll_ki * I_ex;
    gy += (pitch_kp * kp_scale) * ey + pitch_ki * I_ey;
    gz += (IMU_kp * kp_scale) * ez + IMU_ki * I_ez;

    q.q0 += (-q.q1 * gx - q.q2 * gy - q.q3 * gz) * delta_T * 0.5f;
    q.q1 += ( q.q0 * gx + q.q2 * gz - q.q3 * gy) * delta_T * 0.5f;
    q.q2 += ( q.q0 * gy - q.q1 * gz + q.q3 * gx) * delta_T * 0.5f;
    q.q3 += ( q.q0 * gz + q.q1 * gy - q.q2 * gx) * delta_T * 0.5f;

    norm = sqrtf(q.q0 * q.q0 + q.q1 * q.q1 + q.q2 * q.q2 + q.q3 * q.q3);
    if(norm > 0.001f)
    {
        q.q0 /= norm;
        q.q1 /= norm;
        q.q2 /= norm;
        q.q3 /= norm;
    }
}

/**
 * @brief 四元数转欧拉角
 *
 * 计算Roll、Pitch、Yaw角度
 */
static void Quaternion_To_Euler(void)
{
    // Roll角(横滚)
    Gyro_Angle.RollAngle = asin(2 * q.q0 * q.q2 - 2 * q.q1 * q.q3) * 180 / pI;

    // Pitch角(俯仰)
    Gyro_Angle.PitchAngle = -atan2(2 * q.q2 * q.q3 + 2 * q.q0 * q.q1,
                                   -2 * q.q1 * q.q1 - 2 * q.q2 * q.q2 + 1) * 180 / pI;

    // Yaw角(航向)
    Gyro_Angle.YawAngle = -atan2(2 * q.q1 * q.q2 + 2 * q.q0 * q.q3,
                                 -2 * q.q2 * q.q2 - 2 * q.q3 * q.q3 + 1) * 180 / pI;
}

/**
 * @brief 处理Yaw角连续性
 *
 * 处理Yaw角的±180度跳变，保持连续性
 */
static void Handle_Yaw_Continuity(void)
{
    float yaw_delta = Gyro_Angle.YawAngle - Yaw_Angle_Old;

    // 处理±180度跳变
    if (yaw_delta <= -180)
        yaw_delta += 360;
    else if (yaw_delta > 180)
        yaw_delta -= 360;

    // 累加Yaw角(连续的)
    YawAngle_Trans += yaw_delta;

    // 规范到[-180, 180]范围
    if (YawAngle_Trans > 360.0f || YawAngle_Trans < -360.0f)
        YawAngle_Trans = fmod(YawAngle_Trans, 360.0f);

    while (YawAngle_Trans > 180.0f)
        YawAngle_Trans -= 360.0f;
    while (YawAngle_Trans < -180.0f)
        YawAngle_Trans += 360.0f;

    Yaw_Angle_Old = Gyro_Angle.YawAngle;
}

//===================================================External Functions===================================================

/**
 * @brief 陀螺仪偏差校准
 */
void Gyro_Offset_Init(void)
{
    GyroOffset.Xdata = 0;
    GyroOffset.Ydata = 0;
    GyroOffset.Zdata = 0;
}

/**
 * @brief 获取所有姿态角
 * @note 每5ms调用一次
 */
void Gyro_Get_All_Angles(void)
{
    // 1. 获取并预处理IMU数据
    IMU_Get_Values();

    // 2. 更新四元数(融合陀螺仪和加速度计)
    IMU_AHRS_Update();

    // 3. 四元数转欧拉角
    Quaternion_To_Euler();

    // 4. 处理Yaw角连续性
    Handle_Yaw_Continuity();
}

/**
 * @brief 重置姿态
 */
void Gyro_Reset_Attitude(void)
{
    q.q0 = 1;
    q.q1 = 0;
    q.q2 = 0;
    q.q3 = 0;

    I_ex = 0;
    I_ey = 0;
    I_ez = 0;
    imu_first_count_time = 0.0f;
    gyro_y_lpf = 0.0f;
    gyro_y_lpf_initialized = 0;

    Gyro_Angle.RollAngle = 0;
    Gyro_Angle.PitchAngle = 0;
    Gyro_Angle.YawAngle = 0;

    YawAngle_Trans = 0;
    Yaw_Angle_Old = 0;

    // 重置低通滤波器
    acc_filter_x.initialized = 0;
    acc_filter_y.initialized = 0;
    acc_filter_z.initialized = 0;
}

/**
 * @brief 获取Roll角
 */
float Gyro_RollAngle_Get(void)
{
    return Gyro_Angle.RollAngle;
}

/**
 * @brief 获取Pitch角
 */
float Gyro_PitchAngle_Get(void)
{
    return Gyro_Angle.PitchAngle;
}

/**
 * @brief 获取Yaw角(连续的)
 */
float Gyro_YawAngle_Get(void)
{
    return YawAngle_Trans;
}

/**
 * @brief 调整互补滤波参数(用于调试)
 */
void IMU_Set_Kp(float kp)
{
    IMU_kp = kp;
}

void IMU_Set_Ki(float ki)
{
    IMU_ki = ki;
}

void IMU_Set_Roll_Kp(float kp)
{
    roll_kp = kp;
}

void IMU_Set_Roll_Ki(float ki)
{
    roll_ki = ki;
}

void IMU_Set_Pitch_Kp(float kp)
{
    pitch_kp = kp;
}

void IMU_Set_Pitch_Ki(float ki)
{
    pitch_ki = ki;
}
