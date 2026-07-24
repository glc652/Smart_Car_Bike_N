/*
 * Agorithm.h
 *
 *  Created on: 2026Äê6ÔÂ5ÈÕ
 *      Author: 19740
 */

#ifndef CODE_AGORITHM_H_
#define CODE_AGORITHM_H_

float LowPassFilter(float current, float last, float alpha);

float StepApproach(float target, float step_size);

float AngleErrorNormalize(float error);

float StepApproachAngleMode(float target, float real_angle, float step_size, int mode);
float LimitFloat(float value, float min, float max);
float StepFloat(float now, float target, float step);

#endif /* CODE_AGORITHM_H_ */
