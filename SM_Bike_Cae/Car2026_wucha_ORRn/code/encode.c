/*
 * encode.c
 *
 *  Created on: 2025-12-14
 *      Author: 19740
 */

//===================================================Include===================================================
#include "encode.h"
#include "zf_driver_encoder.h"
#include "zf_common_headfile.h"

//===================================================define&declare===================================================
#define ENCODER_1                   (TIM2_ENCODER)
#define ENCODER_1_A                 (TIM2_ENCODER_CH1_P33_7)
#define ENCODER_1_B                 (TIM2_ENCODER_CH2_P33_6)

#define speed_filter_length 10
#define pi 3.1415926f
#define WHEEL_RADUS 0.032f    // 轮子半径(m), 实测�?
#define DT 0.005f              // 控制周期5ms
#define PULSES_PER_REV 4100     // Total pulses per wheel revolution

int16 Encoder_Pules = 0;
float Encoder_Speed = 0;
float Car_Speed = 0;
float Encoder_Distance = 0; //编码器换算的距离，单位m
volatile int32 Encoder_Nag_Pulse_Accum = 0;  // Raw encoder pulses accumulated for navigation mileage

//static uint8_t speed_index=0;
//static uint8_t speed_count=0;
//static float speed_buffer[speed_filter_length]={0};
static float distance_per_pulse = 0;
int16 pulse_snapshot = 0;  // 保存脉冲快照用于距离计算

//===================================================Function===================================================
/*
 * 获取编码器脉冲�?
 */
float A_SPEED=0;
void Get_Encoder_Pulses(void){
    static float last_A_SPEED= 0.0f;
    int16 raw_pulse = -encoder_get_count(ENCODER_1);

    pulse_snapshot = raw_pulse;  // �����������
    Encoder_Nag_Pulse_Accum += raw_pulse;

    A_SPEED = LowPassFilter((float)raw_pulse,last_A_SPEED,0.1f);
    last_A_SPEED=A_SPEED;
    Encoder_Pules = raw_pulse/20;

    static float filtered_speed;
    filtered_speed = filtered_speed*0.75 + Encoder_Pules *0.25;
    Encoder_Speed = filtered_speed;
    encoder_clear_count(ENCODER_1);
}

/*
 * Calculate wheel speed
 */
//void Get_wheel_Speed(void){
//    float raw_speed=0.0f;
//    Car_Speed = (float)Encoder_Pules*(2.0f*pi*WHEEL_RADUS)/(DT*PULSES_PER_REV);
//    raw_speed = Car_Speed;
//
//    speed_buffer[speed_index]=raw_speed;
//    speed_index=(speed_index+1)%speed_filter_length;
//    if (speed_count<speed_filter_length){
//        speed_count++;
//    }
//    float sum = 0.0f;
//    for (int i=0;i<speed_count;i++){
//        sum+=speed_buffer[i];
//    }
//    Car_Speed = sum/(float)speed_count;
//}

/*
 * 根据编码器脉冲计算行驶距�?
 */
void Calculate_Distance(void){
    distance_per_pulse = (2.0f * pi * WHEEL_RADUS) / PULSES_PER_REV;
    Encoder_Distance += (float)pulse_snapshot * distance_per_pulse;  // 使用脉冲快照
}

/*
 * 重置编码器距离计数器
 */
void Encoder_Distance_Reset(void){
    Encoder_Distance = 0.0f;
    Encoder_Nag_Pulse_Accum = 0;
}

/*
 * 获取编码器计算的距离
 */
float Encoder_Get_Distance(void){
    return Encoder_Distance;
}

float Encoder_Nag_Get_Delta_Pulse(void)
{
    float delta = (float)Encoder_Nag_Pulse_Accum;
    Encoder_Nag_Pulse_Accum = 0;
    return delta;
}

void Encoder_Nag_Pulse_Reset(void)
{
    Encoder_Nag_Pulse_Accum = 0;
}

//===================================================External Function===================================================
/*
 * 编码器初始化
 */
void Encode_Init(void){
    encoder_quad_init(ENCODER_1, ENCODER_1_A, ENCODER_1_B);
}

/*
 * 编码器处理函�?
 * �?ms调用一�?
 */
void Encoder_Process(void){
    Get_Encoder_Pulses();
    Calculate_Distance();
}
/**
 * 手动转动车轮读取脉冲
 */
void Test_Encoder_Resolution(void){
    // 手动转动车轮一�?
    // 然后读取脉冲�?
    int pulses = encoder_get_count(ENCODER_1);
    printf("One revolution pulses: %d\n", pulses);
}

/*
 * 根据编码器行驶固定距�?
 */
static float target_dist = 0;
static uint8_t is_moving = 0;

void Encoder_Move_Forward(float target_distance, float speed){
    target_dist = target_distance;
    is_moving = 1;

    Set_Car_Speed(speed);
}

/*
 * 检查是否到达目标距�?
 * 在主循环中调用此函数
 */
uint8_t Encoder_Is_Moving(void){
    if(is_moving){
        if(Encoder_Distance >= target_dist){
            is_moving = 0;
            Motor_Disable();
            return 0;  // 已到达目�?
        }
        return 1;  // 还在行驶
    }
    return 0;
}
//测试根据编码器走一定距�?
void Test_Move(void){
    static uint8_t flag = 1;
    if (flag){
        Encoder_Move_Forward(0.8,3);
    }else{
        Motor_Disable();
    }
    if (Encoder_Is_Moving()){
        flag =1;
    }else{
        flag =0;
    }
}
