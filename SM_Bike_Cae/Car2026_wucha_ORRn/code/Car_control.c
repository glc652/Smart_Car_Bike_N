/*
 * Car_control.c
 *
 *  Created on: 2025�?2�?4�?
 *      Author: 19740
 */
//===================================================Include===================================================
#include "Car_control.h"
#include "zf_common_headfile.h"
#include "PID_Flash_Storage.h"
#include "navigation.h"
#include "Servo.h"
//===================================================全局变量===================================================
Incremental_PID_TypeDef Speed_PID;  //速度�?
Incremental_PID_TypeDef Speed2_PID;  //速度�?
Pid_TypeDef Gyro_PID;    //陀螺仪�?
Pid_TypeDef Gyro2_PID;    //陀螺仪�?
Pid_TypeDef Angle_PID;    //角度�?
Pid_TypeDef Angle2_PID;
Pid_TypeDef Yaw2_PID;
Pid_TypeDef Yaw_PID;

#define ANGLE_KP_SPEED_COEF   1690.0f
#define ANGLE_KP_SPEED_MIN    10.0f
#define ANGLE_KP_SPEED_MAX    20.0f
#define ANGLE_KP_MIN          10.4f
#define ANGLE_KP_MAX          20.0f

#define SPEED_TARGET_STEP     0.1f   // �?ms最大变化量
#define GAIN_SPEED_ALPHA      0.08f //速度滤波系数
//===================================================结构�?===============================================
uint8_t protection =0; //倾倒保护标�?
#define roll_bundrary    (80.0f)  //翻滚角度限制55

static float balance_speed_target = star_speed;// 最终想去的速度，递增的结果量
static float balance_speed_cmd = star_speed;// 实际给速度 PID 的目�?递增的过程量
static float balance_speed_gain = 15.0f;//平滑跟随实际速度，步进�?

Turn_Handle Turn =
{
    .Angle_Last    = 0,
    .Current_Angle = 0,
    .Offset        = 0,
};
//===================================================局部函�?===============================================
//根据速度改变角度环的Kp
void Balance_Gain_Update(void)
{
  float v = balance_speed_target;//fabsf(balance_speed_target);

  v = LimitFloat(v, ANGLE_KP_SPEED_MIN, ANGLE_KP_SPEED_MAX);
  balance_speed_gain = balance_speed_gain * (1.0f - GAIN_SPEED_ALPHA)+ v * GAIN_SPEED_ALPHA;
  Angle_PID.Kp = ANGLE_KP_SPEED_COEF / (balance_speed_gain * balance_speed_gain);
  Angle_PID.Kp = LimitFloat(Angle_PID.Kp, ANGLE_KP_MIN, ANGLE_KP_MAX);
}
//===================================================外部函数===================================================
/*
 * 全局PID初始�?
 * 速度�?
 * 速度�?5�?Speed_PID, 0.025, 0.007, 0.010 &Gyro_PID, 0.166, 0, 0.05,&Angle_PID, 6.181, 0.0, 0.025,&Yaw_PID,0.327,0,0.0
 * 速度�?0�?Speed_PID, 0.025, 0.007, 0.010 &Gyro_PID, 0.166, 0, 0.05,&Angle_PID, 13.907, 0.0, 0.025,&Yaw_PID,0.327,0,0.0
 * 速度�?�?Speed_PID, 0.087, 0.144, 0.001 &Gyro_PID, 0.138, 0, 0.0,&Angle_PID, 12.038, 0.0, 0.05,&Yaw_PID,0.407,0,0.05
 * 4�?Speed_PID, 0.025, 0.006, 0.006&Angle_PID, 8.85, 0.0, 0.3&Yaw_PID,0.253,0,0.05
 * 3�?Speed_PID, 0.02773, 0.00862, 0.00684&Angle_PID, 14.15, 0.0, 0.45&Yaw_PID,0.2325,0,0.0
 * 7.7 0.7  0.49   0.03  0.05
 * 0.0187 0.03 0.006�?2.55 0 0.25�?.242 0 0
 * 室外�?�?.0117 0.031 0�?4.85 0 0.25�?.192 0 0.01
 * 速度�?�?Speed_PID, 0.008, 0.015, 0.0 &Gyro_PID, 0.188, 0, 0.0,&Angle_PID, 18.50698, 0.0, 0.25,&Yaw_PID,0.272,0,0.0Q
 * 速度�?�?Speed_PID, 0.14, 0.065, 0.02 &Gyro_PID, 0.1405, 0, 0.0,&Angle_PID, 15.52752, 0.0, 0.25,&Yaw_PID,0.1405,0,0.0
 * 速度�?0�?Speed_PID,0.07641, 0.09331, 0.0 &Gyro_PID,0.1505, 0, 0.0,&Angle_PID, 13.038, 0.0, 0.05,&Yaw_PID,0.22716,0,0.00062
 */
/*
 * 全局PID初始参数
 * 速度15档：Speed_PID, 0.025, 0.007, 0.010 &Gyro_PID, 0.166, 0, 0.05,&Angle_PID, 6.181, 0.0, 0.025,&Yaw_PID,0.327,0,0.0
 * 速度10档：Speed_PID, 0.025, 0.007, 0.010 &Gyro_PID, 0.166, 0, 0.05,&Angle_PID, 13.907, 0.0, 0.025,&Yaw_PID,0.327,0,0.0
 * 速度档：Speed_PID, 0.087, 0.144, 0.001 &Gyro_PID, 0.138, 0, 0.0,&Angle_PID, 12.038, 0.0, 0.05,&Yaw_PID,0.407,0,0.05
 * 4档：Speed_PID, 0.025, 0.006, 0.006&Angle_PID, 8.85, 0.0, 0.3&Yaw_PID,0.253,0,0.05
 * 3档：Speed_PID, 0.02773, 0.00862, 0.00684&Angle_PID, 14.15, 0.0, 0.45&Yaw_PID,0.2325,0,0.0
 * 室外科2：0.0117 0.031 0 ； 4.85 0 0.25 ； 0.192 0 0.01
 * 速度：Speed_PID, 0.008, 0.015, 0.0 &Gyro_PID, 0.188, 0, 0.0,&Angle_PID, 18.50698, 0.0, 0.25,&Yaw_PID,0.272,0,0.0
 * 速度8现场(科目1)：Speed_PID, 0.08, 0.03, 0.03 &Gyro_PID, 0.1405, 0, 0.0,&Angle_PID, 15.52752, 0.0, 0.25,&Yaw_PID,0.1505,0,0.00062
 * 速度：Speed_PID,0.07641, 0.09331, 0.0 &Gyro_PID,0.1505, 0, 0.0,&Angle_PID, 13.038, 0.0, 0.05,&Yaw_PID,0.22716,0,0.00062
 * 速度3.2现场(科目2)：Speed_PID, 0.072, 0.038, 0.013 &Gyro_PID, 0.0, 0, 0.0,&Angle_PID, 8.0, 0.00026, 0.05,&Yaw_PID,0.1,0,0.01
 * 速度6现场(科目3)：Speed_PID, 0.08, 0.03, 0.03 &Gyro_PID, 0.1405, 0, 0.0,&Angle_PID, 17.6, 0.0, 0.25,&Yaw_PID,0.1705,0,0.01
 */
void All_PID_Init(void){
//    速度�?
//    Incremental_PID_Init(&Speed_PID, 0.088, 0.095, 0.0, 30.0f, -30.0f);
    Incremental_PID_Init(&Speed_PID, 0.088, 0.095, 0.0, 10.0f, -10.0f);//科二限幅
    Incremental_PID_Init(&Speed2_PID, 0.07641, 0.09331, 0.0, 15.0f, -20.0f);
//    角度�?
    PID_Init(&Angle_PID, 14.50698, 0.0, 0.25, 300.0f,-300.0f);
    PID_Init(&Angle2_PID, 13.038, 0.0, 0.05, 300.0f,-300.0f);
//    角速度�?
    PID_Init(&Gyro_PID, 0.144, 0, 0.0,300.0f,-300.0f);
    PID_Init(&Gyro2_PID, 0.1705, 0, 0.0,300.0f,-300.0f);
//    方向�?
    PID_Init(&Yaw_PID,0.272,0,0.0,300.0f,-300.0f);
    PID_Init(&Yaw2_PID,0.2916,0,0.00062,300.0f,-300.0f);

    // 初始化Flash存储模块
    PID_Flash_Storage_Init();

    // 尝试从Flash加载PID参数,科目2要打开
    PID_Flash_Load_All_PID(&Speed_PID, &Angle_PID, &Gyro_PID, &Yaw_PID);
}
float temp1,temp2 =0 ;
/*
 * 速度�?
 * 全程有速度变化
 */
void Set_Car_Speed(float speed){
    static float temp;
    balance_speed_target =speed;
    balance_speed_cmd = StepFloat(balance_speed_cmd,balance_speed_target,SPEED_TARGET_STEP);
    temp = Get_Incremental_PID_Value(&Speed_PID, balance_speed_cmd - Encoder_Speed);
    Motor_Set(temp);
}
//单速度调参使用
void Set_Car_Speed_Old(float speed){
    static float temp;
//    if (ps2_speed_target ==MAX_SPEED){
//        temp = Get_Incremental_PID_Value(&Speed_PID, speed - Encoder_Speed);
//    }else{
        temp = Get_Incremental_PID_Value(&Speed2_PID, speed - Encoder_Speed);
//    }
    temp1 = speed;
    temp2 = Encoder_Speed;
    Motor_Set(temp);
}
float angle_put = 0,yaw_put = 0;
static float last_yaw_error = 0;  // 上一次的偏航误差，用于计算微分项

static float yaw_goal_smooth = 0.0f;
/*
 * 偏航角闭环控�?PID + 斜坡限制
 * 实现平滑的舵机转向，同时保持完整的闭环控�?
 */
float yaw_control(float yaw_goal){
    float current_yaw = Gyro_YawAngle_Get();
    yaw_put = GetPIDValue(&Yaw_PID,AngleErrorNormalize(StepApproach(yaw_goal,6)-current_yaw),0);
    return (yaw_put);
}
float yaw_control_Kemu3(float yaw_goal){
//    if(balance_speed_cmd==8){
//        Yaw_PID.Kp = 0.613;
//    }else if(balance_speed_cmd==10){
//        Yaw_PID.Kp = 0.491;
//    }else if(balance_speed_cmd==15||balance_speed_cmd==20){
//        Yaw_PID.Kp = 0.327;
//    }
    yaw_put = GetPIDValue(&Yaw_PID,AngleErrorNormalize(StepApproachAngleMode(yaw_goal,Gyro_YawAngle_Get(),2,0)-Gyro_YawAngle_Get()),0);
    return (yaw_put);
}
//角度�?
float roll_control(float roll_goal){
    float temp = 0;
    temp = GetPIDValue(&Angle_PID,Gyro_RollAngle_Get(),roll_goal); //内环
//    temp2 = roll_goal;
//    temp1 = Gyro_RollAngle_Get();
    return temp;
}

//角速度�?
void gyro_y_control(float gyro_y){
    float temp = 0;
    temp = GetPIDValue(&Gyro_PID,IMU_Data.Gyro_y,gyro_y); //外环
    Servo_Set(temp);
}

static void reset_position_pid(Pid_TypeDef *pid)
{
    pid->Error = 0.0f;
    pid->previousError = 0.0f;
    pid->I_Out = 0.0f;
    pid->Output = 0.0f;
}

static void reset_incremental_pid(Incremental_PID_TypeDef *pid)
{
    pid->Error = 0.0f;
    pid->Error_Last = 0.0f;
    pid->Error_Pre = 0.0f;
    pid->Output = 0.0f;
}
void Control_Reset_For_Navigation(void)
{
    yaw_put = 0.0f;
    angle_put = 0.0f;
    last_yaw_error = 0.0f;
    yaw_goal_smooth = Gyro_YawAngle_Get();

    reset_position_pid(&Yaw_PID);
    reset_position_pid(&Angle_PID);
    reset_position_pid(&Gyro_PID);
    reset_incremental_pid(&Speed_PID);

    Servo_Set(0);
    Motor_Disable();
}
/*
 * 车辆安全保护
 */
void car_safety(void){
    if (Gyro_RollAngle_Get() > roll_bundrary || Gyro_RollAngle_Get() < -roll_bundrary) {
        Motor_Disable();
        protection =1;
    }
}

int CTRL_flag=0;

//=========================枪式遥控�?================================
/**
 * @brief  枪式遥控器通道值线性映射到角度 -180° ~ 180°
 * @param  channel  原始通道�?(192~1777, 中�?992)
 * @return 角度值，范围 -180 ~ 180
 */
int16_t Remap_Angle_Linear(int16_t channel)
{
    const int16_t CH_MIN   = 192;
        const int16_t CH_MAX   = 1777;
        const int16_t CH_MID   = 992;
        const int16_t ANG_MAX  = 360;          // 最大角度改�?360
        const int16_t DEAD_ZONE = 15;          // 死区宽度（通道值）

        if(channel < CH_MIN) channel = CH_MIN;
        if(channel > CH_MAX) channel = CH_MAX;

        if(channel > CH_MID - DEAD_ZONE && channel < CH_MID + DEAD_ZONE)
            return 0;

        float angle;
        if(channel <= CH_MID - DEAD_ZONE)
        {
            // 左转区域：通道值从 CH_MIN �?CH_MID-DEAD_ZONE，映射到 -ANG_MAX �?0
            int16_t left_max = CH_MID - DEAD_ZONE;
            angle = -ANG_MAX + (channel - CH_MIN) * (ANG_MAX / (float)(left_max - CH_MIN));
        }
        else
        {
            // 右转区域：通道值从 CH_MID+DEAD_ZONE �?CH_MAX，映射到 0 �?ANG_MAX
            int16_t right_min = CH_MID + DEAD_ZONE;
            angle = (channel - right_min) * (ANG_MAX / (float)(CH_MAX - right_min));
        }

        if(angle > ANG_MAX) angle = ANG_MAX;
        if(angle < -ANG_MAX) angle = -ANG_MAX;

        return (int16_t)angle;
}

/**
 * @brief   旋钮步进角度选择：每次拧动并回中后，切换到下一个预设角�?
 * @param   ch   当前通道�?(192~1777)
 * @return  当前目标角度
 */
float KnobStepAngle(int16_t ch)
{
//    static const float angles[] = {45.0f, -180.0f,135.0f,180.0f};
    static const float angles[] = {180};

    static uint8_t idx = 0;                 // 下一个要输出的角度索引（初始指向 -45�?
    static uint8_t state = 0;               // 0: 等待拧动, 1: 已经拧出等待回中
    static float current_angle = 0.0f;      // 当前目标角度（初�?�?

    const int16_t MID = 992;
    const int16_t DEAD = 30;                // 中点死区 (±30)
    const int16_t EXIT = 200;                // 离开中点的最小偏移（确认是有效拧动）

    // 限制通道范围
    if (ch < 192) ch = 192;
    if (ch > 1777) ch = 1777;

    int16_t offset = ch - MID;
    uint8_t is_mid = (offset > -DEAD && offset < DEAD);
    uint8_t is_exit = (offset > EXIT || offset < -EXIT);

    if (state == 0) {
        // 等待第一次拧�?
        if (!is_mid && is_exit) {
            state = 1;      // 标记已经拧出
        }
    } else if (state == 1) {
        // 已经拧出，等待回�?
        if (is_mid) {
            // 回中完成：切换角�?
            current_angle = angles[idx];
            idx = (idx + 1) % 4;   // 指向下一�?
            state = 0;             // 复位，准备下一�?
        }
    }

    return current_angle;
}
float Taget_angle=0;
/*
 * 主控制函�?
 */
void Ctrl_main(void){
//    if(1 == uart_receiver.finsh_flag)                            // 帧完成标志判�?
//    {
//       if(1 == uart_receiver.state)                             // 遥控器失控状态判�?
//       {
//           //舵机控制
//          if(uart_receiver.channel[0]>=1500&&uart_receiver.channel[0]<=1792)
//          {
////              Taget_angle+=45;
//              Taget_angle-=5;
//              Buzzer_Beep_Once();
//          }
//
//           else if(uart_receiver.channel[0]>=192&&uart_receiver.channel[0]<=700)
//           {
////               Taget_angle-=45;
//               Taget_angle+=5;
//               Buzzer_Beep_Once();
//           }
//           else
//           {
//               Taget_angle=0;
//           }
//       }
//       else
//       {
//           printf("Remote control has been disconnected.\r\n"); // 串口输出失控提示
//       }
//       uart_receiver.finsh_flag = 0;                            // 帧完成标志复�?
//   }

}
