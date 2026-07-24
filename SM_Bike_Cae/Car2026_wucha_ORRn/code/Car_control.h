/*
 * Car_control.h
 *
 *  Created on: 2025��12��14��
 *      Author: 19740
 */

#ifndef CODE_CAR_CONTROL_H_
#define CODE_CAR_CONTROL_H_

#include "PID.h"
#include "stdint.h"
typedef struct
{
    float Offset;
    float Angle_Last;
    float Current_Angle;
} Turn_Handle;

void All_PID_Init(void);
void Set_Car_Speed(float speed);
void Set_Car_Speed_Old(float speed);
float yaw_control(float yaw_goal);
float yaw_control_Kemu3(float yaw_goal);
float roll_control(float roll_goal);
void gyro_y_control(float gyro_y);
void Ctrl_main(void);
void car_safety(void);
void Control_Reset_For_Navigation(void);
void Balance_Gain_Update(void);

extern Incremental_PID_TypeDef Speed_PID;
extern Pid_TypeDef Gyro_PID;
extern Pid_TypeDef Gyro2_PID;
extern Pid_TypeDef Angle_PID;
extern Pid_TypeDef Angle2_PID;
extern Pid_TypeDef Yaw_PID;
extern Pid_TypeDef Yaw2_PID;

extern float angle_put, yaw_put;
extern float temp1, temp2;
extern uint8_t protection;

extern float Taget_angle;
#endif /* CODE_CAR_CONTROL_H_ */
