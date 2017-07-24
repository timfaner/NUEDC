/*
 * pid_speed.h
 *
 *  Created on: 2017年7月24日
 *      Author: lenovo
 */

#ifndef CG_SRC_PID_SPEED_H_
#define CG_SRC_PID_SPEED_H_

void pid_init(float Kp, float Ki, float Kd);
float pid_start(float target_position, float actual_position);

#endif /* CG_SRC_PID_SPEED_H_ */
