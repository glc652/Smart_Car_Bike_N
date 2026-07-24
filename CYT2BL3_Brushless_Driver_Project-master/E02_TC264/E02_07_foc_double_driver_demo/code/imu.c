/*
 * imu_ahrs.c
 *
 *  Created on: 2025年11月29日
 *      Author: Lenovo
 */

#include "zf_common_headfile.h"
#include "imu660rb.h"
#include "imu.h"
//变量定义

SensorData sensor;
Bias bias;
ERROR error;
FLOAT_XYZ Acc,Acc_out,Gyro;
//静态变量
//四元数
quaterInfo_t Q_info = {1,0,0,0};
float prior_ki=0.0f;


//上一帧加速度
static double last_ax=0.0f;
static double last_ay=0.0f;
static double last_az=0.0f;

//开方取倒数
double invSqrt(double x)
{
    double halfx = 0.5f * x;
    double y = x;
    long i = *(long*)&y;
    i = 0x5f3759df - (i>>1);
    y = *(double*)&i;
    y = y * (1.5f - (halfx * y * y));
    return y;
}

////一阶互补滤波 对最后的角度
//double angle_calc(double angle_m, double gyro_m,int n)
//{
//    double gyro_now;
//    double temp_angle;
//    double error_angle;
//    static double last_angle;
//    static uint8 first_angle;
//    if(!first_angle)//判断是否为第一次运行本函数
//    {
//        //如果是第一次运行，则将上次角度值设置为与加速度值一致
//        first_angle = 1;
//        last_angle = angle_m;
//    }
//    //法二
//    gyro_now=gyro_m*sys_para.com_fliter[n].ratio1;
//   //根据测量到的加速度值转换为角度之后与上次的角度值求偏差
//    error_angle = (angle_m - last_angle)*sys_para.com_fliter[n].ratio2;
//    //根据偏差与陀螺仪测量得到的角度值计算当前角度值
//    temp_angle =last_angle+ (gyro_now + error_angle)*DT;
//    //保存当前角度值
//    last_angle = temp_angle;
//    return temp_angle;
//}
//去除零漂
void RemoveZeroDrift(uint16 num)
{
    SensorData sensor={{0,0,0},{0,0,0}};
    for (int i=0;i<num;i++)
    {
        imu660rb_get_acc();
        imu660rb_get_gyro();
        sensor.gyro[0]+=(imu660rb_gyro_x);
        sensor.gyro[1]+=(imu660rb_gyro_y);
        sensor.gyro[2]+=(imu660rb_gyro_z);
        system_delay_us(5000);
    }
    bias.bias_gx=sensor.gyro[0]/num;
    bias.bias_gy=sensor.gyro[1]/num;
    bias.bias_gz=sensor.gyro[2]/num;
}

int16 Acc_x[10];
int16 Acc_y[10];
int16 Acc_z[10];
int16 Gyro_x[10];
int16 Gyro_y[10];
int16 Gyro_z[10];

//读取数据存在结构体中
void read_sensor_data(void)
{
    static double lastaccel[3]= {0,0,0};
    imu660rb_get_acc();
    imu660rb_get_gyro();
    sensor.gyro[0]=imu660rb_gyro_transition(imu660rb_gyro_x-bias.bias_gx);
    sensor.gyro[1]=imu660rb_gyro_transition(imu660rb_gyro_y-bias.bias_gy);
    sensor.gyro[2]=imu660rb_gyro_transition(imu660rb_gyro_z-bias.bias_gz);
    sensor.acc[0]=imu660rb_acc_transition(imu660rb_acc_x);
    sensor.acc[1]=imu660rb_acc_transition(imu660rb_acc_y);
    sensor.acc[2]=imu660rb_acc_transition(imu660rb_acc_z);
    //对加速度计数据低通滤波
    sensor.acc[0]=sensor.acc[0]*newweight+lastaccel[0]*oldweight;
    sensor.acc[1]=sensor.acc[1]*newweight+lastaccel[1]*oldweight;
    sensor.acc[2]=sensor.acc[2]*newweight+lastaccel[2]*oldweight;

    for(int i=0; i<3; i++)
    {
        lastaccel[i] = sensor.acc[i];
    }

    //  对陀螺仪数据
    if (sensor.gyro[0]<=0.215f&&sensor.gyro[0]>-0.215f) sensor.gyro[0]=0.0f;
    if (sensor.gyro[1]<=0.215f&&sensor.gyro[1]>-0.215f) sensor.gyro[1]=0.0f;
    if (sensor.gyro[2]<=0.215f&&sensor.gyro[2]>-0.215f) sensor.gyro[2]=0.0f;


    //存在系统结构体中
    sys_para.gyro.roll_gyro=-sensor.gyro[0];
    sys_para.gyro.pitch_gyro=sensor.gyro[1];
    sys_para.gyro.yaw_gyro=sensor.gyro[2];

}

//初始化函数
void IMU_ahrs_init(void)
{
    error.ex_int=0.0f;
    error.ey_int=0.0f;
    error.ez_int=0.0f;
    bias.bias_gx=0.0f;
    bias.bias_gy=0.0f;
    bias.bias_gz=0.0f;
    //去零漂
    system_delay_ms(1000);
    RemoveZeroDrift(1000);
}




//获取欧拉角
void IMU_quaterToEulerianAngles(void)
{
//    float q0 = Q_info.q0;
//    float q1 = Q_info.q1;
//    float q2 = Q_info.q2;
//    float q3 = Q_info.q3;

      IMU_Value_Get();
    sys_para.eulerAngle.pitch = my_angle.Pitch; // pitch
    sys_para.eulerAngle.roll = my_angle.Roll; // roll

    //利用积分求
    sys_para.eulerAngle.yaw += sensor.gyro[2]*0.005f-0.00018;

    //可以不用姿态限度
    if(sys_para.eulerAngle.roll>90 || sys_para.eulerAngle.roll<-90)
    {
        if(sys_para.eulerAngle.pitch > 0)
        {
            sys_para.eulerAngle.pitch = 180-sys_para.eulerAngle.pitch;
        }
        if(sys_para.eulerAngle.pitch < 0)
        {
            sys_para.eulerAngle.pitch = -(180+sys_para.eulerAngle.pitch);
        }
    }

    if (sys_para.eulerAngle.yaw > 360)
    {
        sys_para.eulerAngle.yaw -= 360;
    }
    else if (sys_para.eulerAngle.yaw < 0)
    {
        sys_para.eulerAngle.yaw += 360;
    }
}
//姿态更新主函数
void IMU_ahrs_update(void)
{
    double vx,vy,vz;//当前机体坐标系下重力单位向量
    double ex,ey,ez;//四元数计算值与加速度计测量值之间的误差
    double q0=Q_info.q0;
    double q1=Q_info.q1;
    double q2=Q_info.q2;
    double q3=Q_info.q3;
    double gx,gy,gz;
    double ax,ay,az;
    read_sensor_data();
    //先把数算好
    float q0q1 = q0*q1;
    float q0q2 = q0*q2;
    float q1q1 = q1*q1;
    float q1q3 = q1*q3;
    float q2q2 = q2*q2;
    float q2q3 = q2*q3;
    //1.数据预处理
    //陀螺仪：滤波、角速度转单位（转为弧度制）

    double gx_g=sensor.gyro[0];
    double gy_g=sensor.gyro[1];
    double gz_g=sensor.gyro[2];
    //转为弧度制
    gx=gx_g*0.0174f;
    gy=gy_g*0.0174f;
    gz=gz_g*0.0174f;

    //加速度计：巴特沃夫滤波，归一化。
    double ax_g=sensor.acc[0];
    double ay_g=sensor.acc[1];
    double az_g=sensor.acc[2];

    double acc_norm=invSqrt(ax_g*ax_g+ay_g*ay_g+az_g*az_g);
    if (acc_norm>0.4f&&acc_norm<1.8f)
    {
        ax=ax_g*acc_norm;
        ay=ay_g*acc_norm;
        az=az_g*acc_norm;
        //更新上一帧加速度
        last_ax=ax;
        last_ay=ay;
        last_az=az;
    }
    else
    {
        return;
    }


    //2.由四元数预测重力方向
    vx=2.0f*(q1q3-q0q2);
    vy=2.0f*(q0q1+q2q3);
    vz=1.0f-2*q1q1-2*q2q2;

    //3.计算四元数预测重力方向与加速度计预测重力方向的观测误差
    //（ax,ay,az）*(vx,vy,vz)
    ex=ay*vz-az*vy;
    ey=az*vx-ax*vz;
    ez=ax*vy-ay*vx;

    //误差进行积分

        error.ex_int+=ex*DT;
        error.ey_int+=ey*DT;
        error.ez_int+=ez*DT;

    //互补滤波
    gx+=sys_para.quarterion.kp*ex+sys_para.quarterion.ki*error.ex_int;
    gy+=sys_para.quarterion.kp*ey+sys_para.quarterion.ki*error.ey_int;
    gz+=sys_para.quarterion.kp*ez+sys_para.quarterion.ki*error.ez_int;

    //5.四元数更新
    double dq0=0.5f*(-q1*gx-q2*gy-q3*gz);
    double dq1=0.5f*(q0*gx-q3*gy+q2*gz);
    double dq2=0.5f*(q3*gx+q0*gy-q1*gz);
    double dq3=0.5f*(-q2*gx+q1*gy+q0*gz);
    q0+=dq0*DT;
    q1+=dq1*DT;
    q2+=dq2*DT;
    q3+=dq3*DT;


    //6.四元数归一化(防数值漂移）
    float norm=invSqrt(q0*q0+q1*q1+q2*q2+q3*q3);
    Q_info.q0=q0*norm;
    Q_info.q1=q1*norm;
    Q_info.q2=q2*norm;
    Q_info.q3=q3*norm;

    //7.转为欧拉角(ZYX顺序)(角度值)
    IMU_quaterToEulerianAngles();

}

//工具函数 积分限幅
double constrain(double value,double max,double min)
{
    if (value>max) value=max;
    else if (value<min) value=min;
    return value;
}
double get_dynamic_kp(double ax,double ay,double az)
{
    //加速度变化率0.000006 0.000002 0.000002
    double d_ax=fabsf(ax-last_ax)/DT;
    double d_ay=fabsf(ay-last_ay)/DT;
    double d_az=fabsf(az-last_az)/DT;
    double activity=d_ax*d_ax*sys_para.ahrs_kp.a+
                   d_ay*d_ay*sys_para.ahrs_kp.b+
                   d_az*d_az*sys_para.ahrs_kp.c;
    //非线性映射(静止时强修复，运动时低修正）
    if (activity>3.0f) activity=3.0f;
    else if (activity<0.1f) activity=0.1f;
    return 3.0f - (activity - 0.1f) * (1.45f / 0.45f);
}

int16 averageFilter(int16 in_data,int16 data[],int16 count)
{
    float sum = 0;
    for(int i=0; i<count; i++)
    {
        data[i]=data[i+1];
        sum = sum + data[i];
    }
    data[count] = in_data;
    sum = sum + data[count];

    return(sum/10);

}


//##########################################################################

#define Kp      1.8f                        // 比例增益控制着加速度计/磁力计的收敛速度//1.42f
#define Ki      0.002f                       //积分增益控制陀螺仪偏差的收敛速度
#define halfT   0.001f                   // 采样周期的一半
#define TIME    0.002f
#define MY_PI 3.14159265358979323846 // 圆周率π的定义
/* 新增积分限幅宏定义（根据实际情况调整数值） */
#define INTEGRAL_LIMIT 0.1f  // 积分项最大绝对值限制，建议初始值0.2~1.0
//static float q0 = 1, q1 = 0, q2 = 0, q3 = 0;    // 表示估计方向的四元数元素
//static float exInt = 0, eyInt = 0, ezInt = 0;    // 比例积分误差
ANGLE my_angle;
int wheel_clear=0,Drive_clear=0;
float alpha = 0.20;  // 滤波系数（0~1，越小越平滑）
float alpha_gyro=0.05;

uint8 Err_Reduce(void)
{
    float Err[3]={0};
    my_angle.Gyro.err[0]=0.0f;
    my_angle.Gyro.err[1]=0.0f;
    my_angle.Gyro.err[2]=0.0f;
    for(int i=0;i<200;i++)
    {
        imu963ra_get_gyro();
        Err[0]+=imu963ra_gyro_x;
        Err[1]+=imu963ra_gyro_y;
        Err[2]+=imu963ra_gyro_z;
        system_delay_ms(10);
    }
    my_angle.Gyro.err[0]=Err[0]/200;
    my_angle.Gyro.err[1]=Err[1]/200;
    my_angle.Gyro.err[2]=Err[2]/200;

    return 0;
}

void imu660rb_value_get(void)
{
    imu660rb_get_gyro();
    imu660rb_get_acc();

//    my_angle.x_true=180*atan((double)imu963ra_acc_y/imu963ra_acc_z)/MY_PI;
//    my_angle.y_true=180*atan((double)imu963ra_acc_x/imu963ra_acc_z)/MY_PI;
//    my_angle.z_true=180*atan((double)imu963ra_acc_y/imu963ra_acc_x)/MY_PI;

    float raw_gx = imu660rb_gyro_transition(imu660rb_gyro_x - my_angle.Gyro.err[0]);
    float raw_gy = imu660rb_gyro_transition(imu660rb_gyro_y - my_angle.Gyro.err[1]);
    float raw_gz = imu660rb_gyro_transition(imu660rb_gyro_z - my_angle.Gyro.err[2]);

    /* --- [新增] 陀螺仪一阶低通滤波 --- */
    // 公式: Out = alpha * Input + (1 - alpha) * Last_Out
    my_angle.Gyro.gx_filtered = alpha_gyro * raw_gx + (1.0f - alpha_gyro) * my_angle.Gyro.gx_filtered;
    my_angle.Gyro.gy_filtered = alpha_gyro * raw_gy + (1.0f - alpha_gyro) * my_angle.Gyro.gy_filtered;
    my_angle.Gyro.gz_filtered = alpha_gyro * raw_gz + (1.0f - alpha_gyro) * my_angle.Gyro.gz_filtered;

    my_angle.Gyro.x=imu660rb_gyro_transition(imu660rb_gyro_x-my_angle.Gyro.err[0]);//真实角速度
    my_angle.Gyro.y=imu660rb_gyro_transition(imu660rb_gyro_y-my_angle.Gyro.err[1]);
    my_angle.Gyro.z=imu660rb_gyro_transition(imu660rb_gyro_z-my_angle.Gyro.err[2]);

    my_angle.Acc.x=imu660rb_acc_transition(imu660rb_acc_x);
    my_angle.Acc.y=imu660rb_acc_transition(imu660rb_acc_y);
    my_angle.Acc.z=imu660rb_acc_transition(imu660rb_acc_z);


    my_angle.Acc.ax_filtered = alpha * my_angle.Acc.x + (1-alpha) * my_angle.Acc.ax_filtered;//滤波后的加速度计值
    my_angle.Acc.ay_filtered = alpha * my_angle.Acc.y + (1-alpha) * my_angle.Acc.ay_filtered;
    my_angle.Acc.az_filtered = alpha * my_angle.Acc.z + (1-alpha) * my_angle.Acc.az_filtered;

    my_angle.x_true=180*atan((double)my_angle.Acc.ay_filtered/my_angle.Acc.az_filtered)/MY_PI;
    my_angle.y_true=180*atan((double)my_angle.Acc.ax_filtered/my_angle.Acc.az_filtered)/MY_PI;
    my_angle.z_true=180*atan((double)my_angle.Acc.ay_filtered/my_angle.Acc.ax_filtered)/MY_PI;

    my_angle.Gyro.x_PI=my_angle.Gyro.x*(MY_PI/180.0);//弧度制角速度
    my_angle.Gyro.y_PI=my_angle.Gyro.y*(MY_PI/180.0);
    my_angle.Gyro.z_PI=my_angle.Gyro.z*(MY_PI/180.0);

}

//一阶互补
float roll = 0;
float pitch = 0;
float roll_com = 0;
float pitch_com = 0;

float my_abs(float x)
{
    if(x>0)
        return x;
    else
        return -x;
}

void IMU_Value_Get(void)
{
    imu660rb_value_get();//x  前负后正   y    右正左负   z  左正右负
    float order1 = 0.999;//Pitch 0.99
    float order2 = 0.98;//Roll 0.963
    roll_com  = order1 * (roll_com  + (my_angle.Gyro.x * TIME)) + (1 - order1)*my_angle.x_true;
    pitch_com = order2 * (pitch_com + ( my_angle.Gyro.y * TIME)) - (1 - order2)*my_angle.y_true;

    if(my_abs(roll_com) <= 180 || my_abs(pitch_com) <= 180)
    {
        my_angle.Roll   = roll_com;
        my_angle.Pitch  = pitch_com;
    }
    if(my_abs(my_angle.Gyro.z)>=0.22f)
    {
        my_angle.Yaw-=my_angle.Gyro.z*TIME;
    }
    if(my_angle.Yaw>=180)
    {
        my_angle.Yaw-=360;

    }
    else if(my_angle.Yaw<=-180)
    {
        my_angle.Yaw+=360;

    }

}




