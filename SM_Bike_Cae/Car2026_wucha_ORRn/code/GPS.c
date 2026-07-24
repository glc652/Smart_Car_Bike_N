/*
 * GPS.c
 *
 *  Created on: 2026年2月9日
 *      Author: 19740
 */
//===================================================Include===================================================
#include "GPS.h"
#include "zf_common_headfile.h"

//===================================================Define&Declare===================================================
static uint16 diff_x_deg =0,diff_y_deg =0;
static uint32 diff_x_min=0,diff_y_min =0;
float diff_x = 0, diff_y =0 ;
float diff_y_deg_avarage =0 , diff_y_min_avarage =0, diff_y_avarage =0;
// 原点经纬度
uint16_t jin_deg = 0, wei_deg = 0;
uint32_t jin_min = 0, wei_min = 0;
uint8 yaw_frist_flag = 1;
float flow_x = 0, flow_y = 0, flow_speed = 0;
float diffx_offset = 0, diffy_offset = 0, yaw_offset = 0;

float target_x = 0, target_y = 0, target_yaw = 0, now_diastance = 1000, point_target_yaw = 0, imu_fusion = 0;

//===================================================内部调用===================================================
/**
 * 将经纬度准换成xy坐标
 */
void set_gps_point(uint16 ax_deg,uint32 ax_min,uint16 ay_deg, uint32 ay_min,uint16 bx_deg, uint32 bx_min, uint16 by_deg, uint32 by_min)
{
    diff_x_deg = bx_deg - ax_deg; //计算当前点与原点经度整数部分的差值
    diff_x_min = bx_min - ax_min ;//计算当前点与原点经度小数部分的差值
    diff_y_deg = by_deg - ay_deg;
    diff_y_min = by_min - ay_min;
    diff_y_deg_avarage = (( ay_deg + by_deg ) / 2.0f) * 0.01745329f ; //求出两点间中点的维度，并转换为弧度
    //diff_y_min_avarage = (( ay_min + by_min)  /2000000000.0f) * 0.01745329f ;
    //diff_y_avarage  = diff_y_deg_avarage + diff_y_min_avarage;
    diff_x = diff_x_deg * 100 + (float)(diff_x_min / 1000000.0f); // 经度差（东西方向）
    diff_y = diff_y_deg * 100 + (float)(diff_y_min / 1000000.0f); // 纬度差（南北方向）

//南北方向：纬度每差 1 度，对应的地面距离大约是 111.3194 千米，也就是 111319.4 米。
//东西方向：经度每差 1 度对应的距离和纬度有关，具体公式为 111.3194 * cos(纬度) 千米。这里使用平均纬度的余弦值来进行缩放。
    diff_x *= 1113.194*cos( diff_y_deg_avarage);//东西方向距离  111475.3836
    diff_y *= 1113.194;//南北方向距离110574
}
// 陀螺仪实时校准航向角
//void imu_offset(void)
//{
//    if (qidong == 1 && gnss.speed >= 1.8)
//    {
//        if (fabs(data_put(yaw_fused - gnss.direction)) < 1)
//        {
//            yaw_offset = calculateOffset(yaw_or, gnss.direction);
//        }
//    }
//}

/*
 * 算出 GPS 方位与 IMU 偏航角之间的偏移量
*/
float calculateOffset(float yaw_imu, float gps_bearing)
{
    float offset = gps_bearing - yaw_imu;
    // 将偏移量限制在 [-180°, 180°] 范围内，避免跳变
    if (offset > 180.0f)
        offset -= 360.0f;
    if (offset < -180.0f)
        offset += 360.0f;
    return offset;
}
//===================================================外部调用===================================================
/*
 * GPS模块初始化
 */
void GPS_Init(void){
    gnss_init(GN42A);               // GN42A 为GPS模块
}

/*
 * 获取GPS数据
 */
void GPS_Getdata(void){
    if (gnss_flag){
        gnss_flag = 0;
        gnss_data_parse(); // 开始解析数据
        set_gps_point(jin_deg, jin_min, wei_deg, wei_min, gnss.longitude_deg, gnss.longitude_min, gnss.latitude_deg, gnss.latitude_min);
//        imu_offset(); // 动态校准航向角
    }
    if (yaw_frist_flag)//一开始换算第一点的yaw偏移量
    {
//        yaw_offset = calculateOffset(Yaw_Angle, read_point_yaw());
        yaw_frist_flag = 0;
    }
}

/*
@作用：计算两点的夹角和距离
@参数：无
@返回：两点的夹角
*/
float calculateDistance_angle(float x1, float y1, float x2, float y2)
{
    float yaw_t = 0;
    now_diastance = (x2 - x1) * (x2 - x1) + (y2 - y1) * (y2 - y1);
    yaw_t = 90 - atan2((y2 - y1), (x2 - x1)) * 57.29577951308232f;

    if (yaw_t < 0)
        yaw_t += 360.0f; // 限幅0~360

    return yaw_t;
}