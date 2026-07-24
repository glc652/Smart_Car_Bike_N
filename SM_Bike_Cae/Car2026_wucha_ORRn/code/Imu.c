///*
// * Quaternion_Attitude.c
// *
// * 四元数解算姿态角模型
// * 基于Madgwick算法的IMU融合算法
//
// *
// *  Created on: 2026-05-27
// *      Author: Kiro
// */
////===================================================Include===================================================
//#include "Imu.h"
//#include "zf_device_imu660rb.h"
//#include "math.h"
////===================================================define===================================================
//#define delta_T     0.005f
//#define pI          3.1415926f
//#define GYRO_OFFSET_SAMPLES 1000
//#define GYRO_OFFSET_INTERVAL 5
//#define ACC_NORM_MIN 0.1f
//#define acc_trans_value         (4098)
//#define gyro_trans_value        (14.3)
//
//Gyro_AngleTypeDef Gyro_Angle;
//Gyro_Param_t GyroOffset;
//IMU_Param_t IMU_Data;
//float YawAngle_Trans = 0;
//float Yaw_Angle = 0,Pitch_Angle = 0,Roll_Angle = 0;
//float Yaw_Angle_non = 0;
//typedef struct {
//    float q0, q1, q2, q3;
//} Quaternion_t;
////===================================================declare===================================================
//
//static Quaternion_t q = {1, 0, 0, 0};
//static float I_ex = 0, I_ey = 0, I_ez = 0;
//static float IMU_kp = 0.004f;
//static float IMU_ki = 0.00005f;
//static float roll_kp = 0.0003f;
//static float roll_ki = 0.000005f;
//static float pitch_kp = 0.00002f;
//static float pitch_ki = 0.000001f;
//static float Yaw_Angle_Old = 0;
//
////===================================================�ڲ�����===================================================
//static float My_Rsqrt(float num)
//{
//    float halfx = 0.5f * num;
//    float y = num;
//    long i = *(long*)&y;
//    i = 0x5f375a86 - (i >> 1);
//    y = *(float*)&i;
//    y = y * (1.5f - (halfx * y * y));
//    return y;
//}
////===================================================�ⲿ����===================================================
//void Gyro_Offset_Init(void)
//{
//    GyroOffset.Xdata = 0;
//    GyroOffset.Ydata = 0;
//    GyroOffset.Zdata = 0;
//
//    printf("Calibrating gyro offset...\r\n");
//    for (uint16_t i = 0; i < GYRO_OFFSET_SAMPLES; ++i)
//    {
//        imu660rb_get_gyro();
//        GyroOffset.Xdata += imu660rb_gyro_x;
//        GyroOffset.Ydata += imu660rb_gyro_y;
//        GyroOffset.Zdata += imu660rb_gyro_z;
//        system_delay_ms(GYRO_OFFSET_INTERVAL);
//
//        if (i % 200 == 0)
//            printf("Progress: %d/%d\r\n", i, GYRO_OFFSET_SAMPLES);
//    }
//
//    GyroOffset.Xdata /= GYRO_OFFSET_SAMPLES;
//    GyroOffset.Ydata /= GYRO_OFFSET_SAMPLES;
//    GyroOffset.Zdata /= GYRO_OFFSET_SAMPLES;
//
//    printf("Gyro offset calibration complete:\r\n");
//    printf("X: %.2f, Y: %.2f, Z: %.2f\r\n",
//           GyroOffset.Xdata, GyroOffset.Ydata, GyroOffset.Zdata);
//}
//
//static void IMU_Get_Values(void)
//{
//    float alpha = 0.05f;
//    IMU_Data.acc_x = (((float) imu660rb_acc_x) * alpha) / acc_trans_value + IMU_Data.acc_x * (1 - alpha);
//    IMU_Data.acc_y = (((float) imu660rb_acc_y) * alpha) / acc_trans_value + IMU_Data.acc_y * (1 - alpha);
//    IMU_Data.acc_z = (((float) imu660rb_acc_z) * alpha) / acc_trans_value + IMU_Data.acc_z * (1 - alpha);
//
//    IMU_Data.gyro_x = ((float) imu660rb_gyro_x - GyroOffset.Xdata) * pI / 180 / gyro_trans_value;
//    IMU_Data.gyro_y = ((float) imu660rb_gyro_y - GyroOffset.Ydata) * pI / 180 / gyro_trans_value;
//    IMU_Data.gyro_z = ((float) imu660rb_gyro_z - GyroOffset.Zdata) * pI / 180 / gyro_trans_value;
//
//    IMU_Data.Gyro_y = ((float) imu660rb_gyro_y - GyroOffset.Ydata)/ gyro_trans_value;
//    IMU_Data.Gyro_z = ((float) imu660rb_gyro_z - GyroOffset.Zdata)/ gyro_trans_value;
//
//
//}
//
//static void IMU_AHRS_Update(void)
//{
//    float halfT = 0.5f * delta_T;
//    float gx = IMU_Data.gyro_x;
//    float gy = IMU_Data.gyro_y;
//    float gz = IMU_Data.gyro_z;
//    float ax = IMU_Data.acc_x;
//    float ay = IMU_Data.acc_y;
//    float az = IMU_Data.acc_z;
//
//    float norm = My_Rsqrt(ax * ax + ay * ay + az * az);
//
//    if (norm < ACC_NORM_MIN)
//        return;
//
//    ax = ax * norm;
//    ay = ay * norm;
//    az = az * norm;
//
//    float vx = 2 * (q.q1 * q.q3 - q.q0 * q.q2);
//    float vy = 2 * (q.q0 * q.q1 + q.q2 * q.q3);
//    float vz = q.q0 * q.q0 - q.q1 * q.q1 - q.q2 * q.q2 + q.q3 * q.q3;
//
//    float ex = ay * vz - az * vy;
//    float ey = az * vx - ax * vz;
//    float ez = ax * vy - ay * vx;
//
//    I_ex += halfT * ex;
//    I_ey += halfT * ey;
//    I_ez += halfT * ez;
//
//    gx = gx + roll_kp * ex + roll_ki * I_ex;
//    gy = gy + pitch_kp * ey + pitch_ki * I_ey;
//    gz = gz + IMU_kp * ez + IMU_ki * I_ez;
//
//    q.q0 = q.q0 + (-q.q1 * gx - q.q2 * gy - q.q3 * gz) * halfT;
//    q.q1 = q.q1 + (q.q0 * gx + q.q2 * gz - q.q3 * gy) * halfT;
//    q.q2 = q.q2 + (q.q0 * gy - q.q1 * gz + q.q3 * gx) * halfT;
//    q.q3 = q.q3 + (q.q0 * gz + q.q1 * gy - q.q2 * gx) * halfT;
//
//    norm = My_Rsqrt(q.q0 * q.q0 + q.q1 * q.q1 + q.q2 * q.q2 + q.q3 * q.q3);
//    q.q0 = q.q0 * norm;
//    q.q1 = q.q1 * norm;
//    q.q2 = q.q2 * norm;
//    q.q3 = q.q3 * norm;
//}
//
//void Gyro_Get_All_Angles(void)
//{
//    imu660rb_get_acc();
//    imu660rb_get_gyro();
//    IMU_Get_Values();
//    IMU_AHRS_Update();
//
//    Gyro_Angle.RollAngle  = asin(2 * q.q0 * q.q2 - 2 * q.q1 * q.q3) * 180 / pI;
//    Gyro_Angle.PitchAngle = -atan2(2 * q.q2 * q.q3 + 2 * q.q0 * q.q1, -2 * q.q1 * q.q1 - 2 * q.q2 * q.q2 + 1) * 180 / pI;
//    Gyro_Angle.YawAngle = -atan2(2 * q.q1 * q.q2 + 2 * q.q0 * q.q3, -2 * q.q2 * q.q2 - 2 * q.q3 * q.q3 + 1) * 180 / pI;
//
//    float i = Gyro_Angle.YawAngle - Yaw_Angle_Old;
//    if (i < -180)
//        i += 360;
//    else if (i > 180)
//        i -= 360;
//
//    YawAngle_Trans += i;
//
//    if (YawAngle_Trans > 360.0f || YawAngle_Trans < -360.0f)
//        YawAngle_Trans = fmod(YawAngle_Trans, 360.0f);
//
//    while (YawAngle_Trans > 180.0f)
//        YawAngle_Trans -= 360.0f;
//    while (YawAngle_Trans < -180.0f)
//        YawAngle_Trans += 360.0f;
//
//    Yaw_Angle_Old = Gyro_Angle.YawAngle;
//}
//
//void Gyro_Reset_Attitude(void)
//{
//    q.q0 = 1;
//    q.q1 = 0;
//    q.q2 = 0;
//    q.q3 = 0;
//
//    I_ex = 0;
//    I_ey = 0;
//    I_ez = 0;
//
//    Gyro_Angle.RollAngle = 0;
//    Gyro_Angle.PitchAngle = 0;
//    Gyro_Angle.YawAngle = 0;
//
//    YawAngle_Trans = 0;
//    Yaw_Angle_Old = 0;
//}
//
// float Gyro_RollAngle_Get(void)
// {
//     return Gyro_Angle.RollAngle;
// }
//
//float Gyro_PitchAngle_Get(void)
//{
//    return Gyro_Angle.PitchAngle;
//}
//
//float Gyro_YawAngle_Get(void)
//{
//    return YawAngle_Trans;
//}
