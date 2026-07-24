/*
 * imu_ahrs.h
 *
 *  Created on: 2025年11月29日
 *      Author: Lenovo
 */

#ifndef CODE_ANGLE_IMU_AHRS_H_
#define CODE_ANGLE_IMU_AHRS_H_
#include "init.h"

//宏定义
#define newweight 0.2f
#define oldweight 0.8f
#define DT 0.005f
#define gyro_ratio 4.8
#define acc_ratio 1.2

//外部变量声明
extern SYSTEM_PARAMETER sys_para;


//三轴浮点型
typedef struct
{
    int16 X;
    int16 Y;
    int16 Z;
}FLOAT_XYZ;

//四元数信息
typedef struct{
    float q0;
    float q1;
    float q2;
    float q3;
}quaterInfo_t;


//原始传感器数据
typedef struct
{
        double gyro[3];
        double acc[3];
        double acc_angle[3];
      double gyro_angle[3];
}SensorData;
//零漂
typedef struct
{
        double bias_gx;
        double bias_gy;
        double bias_gz;
}Bias;//用于去零漂

typedef struct
{
        double ex_int;
        double ey_int;
        double ez_int;
}ERROR;//用于修正角速度


double angle_calc(double angle_m, double gyro_m,int n);
void IMU_ahrs_init(void);
void RemoveZeroDrift(uint16 num);
void read_sensor_data(void);
void IMU_ahrs_update(void);
double invSqrt(double x);
double constrain(double value,double max,double min);
double get_dynamic_kp(double ax,double ay,double az);
void IMU_quaterToEulerianAngles(void);
int16 averageFilter(int16 in_data,int16 data[],int16 count);


//###################################################################################################

typedef struct
{
    float x;
    float y;
    float z;
    float x_PI;
    float y_PI;
    float z_PI;

    float err[3];

    float gx_filtered;
    float gy_filtered;
    float gz_filtered;


}XGYRO,*XGYR;

typedef struct
{
    float x;
    float y;
    float z;
    float ax_filtered;
    float ay_filtered;
    float az_filtered;
}XACC,*XAC;



typedef struct
{
    float Pitch;//俯仰角
    float Roll;//横滚角
    float Yaw;//偏航角

    float x_true;
    float y_true;
    float z_true;
    XGYRO  Gyro;
    XACC  Acc;
}ANGLE,*ANG;

extern ANGLE my_angle;
extern float roll;
extern float pitch;
extern float roll_com;
extern float pitch_com;
extern int wheel_clear,Drive_clear;
uint8 Err_Reduce(void);
void imu963ra_value_get(void);
void IMU_Update(float gx, float gy, float gz, float ax, float ay, float az);
void IMU_Value_Get(void);


#endif /* CODE_ANGLE_IMU_AHRS_H_ */
