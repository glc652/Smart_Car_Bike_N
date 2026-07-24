/*
 * GPS.h
 *
 *  Created on: 2026Äê2ÔÂ9ÈÕ
 *      Author: 19740
 */

#ifndef CODE_GPS_H_
#define CODE_GPS_H_

#include "zf_common_typedef.h"
void GPS_Init(void);
void GPS_Getdata(void);
float calculateDistance_angle(float x1, float y1, float x2, float y2);
float calculateOffset(float yaw_imu, float gps_bearing);

extern uint16_t jin_deg, wei_deg;
extern uint32_t jin_min, wei_min;
extern float diffx_offset, diffy_offset, yaw_offset;
extern float diff_x, diff_y;
extern float flow_x, flow_y;

#endif /* CODE_GPS_H_ */