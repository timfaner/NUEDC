/*
 * pid_speed.c
 *
 *  Created on: 2017年7月24日
 *      Author: lenovo
 */

/******************************/
/*********imortant*************/
#include "r_cg_macrodriver.h"
/******************************/
#include "r_cg_sci.h"
#include "pid_speed.h"

struct _pid
{
    float set_position;
    float actual_position;
    float error;
    float error_last;
    float error_second2last;
    float Kp,Ki,Kd;
}pid;

void pid_init(float Kp, float Ki, float Kd)
{
	pid.set_position = 0.0;
	pid.actual_position = 0.0;
	pid.error = 0.0;
	pid.error_last = 0.0;
	pid.error_second2last = 0.0;
	pid.Kp = Kp;
	pid.Ki = Ki;
	pid.Kd = Kd;
}

float pid_start(float target_position, float actual_position)
{
	float speed;
	float increment_position;
    pid.set_position = target_position;
    pid.actual_position = actual_position;
    pid.error = pid.set_position - pid.actual_position;
    increment_position = pid.Kp*(pid.error - pid.error_last) + pid.Ki*pid.error + pid.Kd*(pid.error - 2*pid.error_last + pid.error_second2last);
//    pid.actual_position = pid.actual_position + increment_position;
    pid.error_second2last = pid.error_last;
    pid.error_last = pid.error;
    speed = increment_position*0.7/90.0;
    return speed;
}


