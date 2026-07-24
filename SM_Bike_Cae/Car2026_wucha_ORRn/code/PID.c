/*
 * PID.c
 *
 *  Created on: 2025年12月5日
 *      Author: 19740
 */
//===================================================Include===================================================
#include "PID.h"
#include "vofa.h"
#include "stdio.h"
#include "math.h"
//===================================================变量===================================================

//===================================================内部调用===================================================

//===================================================外部调用===================================================
//=============位置式===========
/*
 * 位置式PID初始化
 * Pid_TypeDef *PID-PID结构体; float Kp-设定的Kp; float Ki-设定的Ki; float Kd-设定的Kd;
 * float outPutMin-输出限幅; float outPutMax-输出限幅
 */
void PID_Init(Pid_TypeDef *PID, float Kp, float Ki, float Kd, float OutputMax, float OutputMin)
{
    PID->Kp = Kp;
    PID->Ki = Ki;
    PID->Kd = Kd;
    PID->previousError = 0;
    PID->OutputMin = OutputMin;
    PID->OutputMax = OutputMax;
}

/*
 * 位置式PID获取值
 * Pid_TypeDef *PID-PID结构体; float setpoint-目标值; float measurement-测量值
 * 返回：float Output-输出
 */
float GetPIDValue(Pid_TypeDef *PID, float measurement, float setpoint)
{
    float P = 0.0f, D=0.0f;
    PID->Error = setpoint - measurement; //计算误差
    P = PID->Error;
    PID->I_Out = PID->I_Out + PID->Error;
    D = PID->Error - PID->previousError;

//    printf("P:%f\n",PID->Kp *P);
    PID->Output = PID->Kp * P+ PID->Ki * PID->I_Out+ PID->Kd * D;
    PID->previousError = PID->Error;
    //限幅
    if(PID->Output > PID->OutputMax)
    {
        PID->Output = PID->OutputMax;
    }
    else if(PID->Output < PID->OutputMin)
    {
        PID->Output = PID->OutputMin;
    }

    return PID->Output;
}
float GetPIDValue2(Pid_TypeDef *PID, float error)
{
    float P = 0.0f, D=0.0f;
    PID->Error = error; //计算误差
    P = PID->Error;
    PID->I_Out = PID->I_Out + PID->Error;
    D = PID->Error - PID->previousError;

//    printf("P:%f\n",PID->Kp *P);
    PID->Output = PID->Kp * P+ PID->Ki * PID->I_Out+ PID->Kd * D;
    PID->previousError = PID->Error;
    //限幅
    if(PID->Output > PID->OutputMax)
    {
        PID->Output = PID->OutputMax;
    }
    else if(PID->Output < PID->OutputMin)
    {
        PID->Output = PID->OutputMin;
    }

    return PID->Output;
}
//=============增量式===========
/*
 * 增量式PID初始化
 * Pid_TypeDef *PID-PID结构体; float Kp-设定的Kp; float Ki-设定的Ki; float Kd-设定的Kd;
 * 返回：float outPut-输出
 */
void Incremental_PID_Init(Incremental_PID_TypeDef *PID, float Kp, float Ki, float Kd, float Out_Put_Max, float Out_Put_Min)
{
    PID->Kp = Kp;
    PID->Ki = Ki;
    PID->Kd = Kd;
    PID->Error = 0;
    PID->Error_Last= 0;
    PID->Error_Pre= 0;
    PID->Out_Put_Min = Out_Put_Min;
    PID->Out_Put_Max = Out_Put_Max;
}

/*
 * 增量式PID获取值
 * Incremental_PID_TypeDef *PID-PID结构体; float Error-误差值
 * 返回：float Output-输出
 */
float Get_Incremental_PID_Value(Incremental_PID_TypeDef *PID, float Error)
{
    float P = 0.0f, I=0.0f, D=0.0f;
    static float Increment = 0;//增量

    PID->Error = Error;//获取误差

    P = PID->Error - PID->Error_Last;//当前误差-上一次误差
    I = PID->Error;
    D = PID->Error - 2*PID->Error_Last + PID->Error_Pre;

    Increment = PID->Kp*P + PID->Ki*I + PID->Kd*D;

    PID->Error_Pre = PID->Error_Last;
    PID->Error_Last = PID->Error;

    PID->Output = PID->Output + Increment;
    //限幅
    if(PID->Output > PID->Out_Put_Max)
    {
        PID->Output = PID->Out_Put_Max;
    }
    else if(PID->Output < PID->Out_Put_Min)
    {
        PID->Output = PID->Out_Put_Min;
    }

    return PID->Output;
}
