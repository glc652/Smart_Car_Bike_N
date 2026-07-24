/*
 * encode.h
 *
 *  Created on: 2025-12-14
 *      Author: 19740
 */

#ifndef CODE_ENCODE_H_
#define CODE_ENCODE_H_

#include "zf_common_typedef.h"

void Encode_Init(void);
void Encoder_Process(void);
void Encoder_Distance_Reset(void);
float Encoder_Get_Distance(void);
float Encoder_Nag_Get_Delta_Pulse(void);
void Encoder_Nag_Pulse_Reset(void);
void Test_Encoder_Resolution(void);
void Encoder_Move_Forward(float target_distance, float speed);
uint8_t Encoder_Is_Moving(void);

extern int16 Encoder_Pules;
extern float Encoder_Speed;
extern float Encoder_Distance;
extern volatile int32 Encoder_Nag_Pulse_Accum;
extern int16 pulse_snapshot;
extern float A_SPEED;
#endif /* CODE_ENCODE_H_ */
