/*
 * Motor.h
 *
 *  Created on: 2025Äê12ÔÂ14ÈÕ
 *      Author: 19740
 */

#ifndef CODE_MOTOR_H_
#define CODE_MOTOR_H_

#include "stdint.h"
void Motor_Init(void);
void Motor_Set(float PWMduty);
void Motor_Disable(void);

#endif /* CODE_MOTOR_H_ */
