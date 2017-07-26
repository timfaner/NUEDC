/*
 * offset_calculate.c
 *
 *  Created on: 2017年7月26日
 *      Author: lenovo
 */


/******************************/
/*********imortant*************/
#include "r_cg_macrodriver.h"
/******************************/
#include "math.h"
#include "offset_calculate.h"

/******************************************/
/*********Renesas get PIX attiaude*********/
extern volatile float ** ras_get_pix_attitude;
/******************************************/

float rasAngleCalculate(uint8_t offset)
{
    float angle;
    angle = atan(offset/162.71);
    return angle;
}
float rasY_offsetCalculate(uint8_t openmv_x_offset)
{
    float angle, y_offset, *height;
    angle = rasAngleCalculate(openmv_x_offset);
    *ras_get_pix_attitude = getAttitude();
    height = getHeight();
    y_offset = -((*height) * tan( (*ras_get_pix_attitude)[0]))/cos( (*ras_get_pix_attitude)[1] );
    y_offset += ((*height) * sin(angle) * sin(angle + 90 - (*ras_get_pix_attitude)[0]))/(cos( (*ras_get_pix_attitude)[1]) * cos( (*ras_get_pix_attitude)[0]));
    return y_offset;
}
float rasX_offsetCalculate(uint8_t openmv_y_offset)
{
    float angle, x_offset, *height;
    angle = rasAngleCalculate(openmv_y_offset);
    *ras_get_pix_attitude = getAttitude();
    height = getHeight();
    x_offset = (*height) * tan( (*ras_get_pix_attitude)[1]);
    x_offset += -((*height) * sin(angle))/(cos( (*ras_get_pix_attitude)[1]) * sin(90 -  (*ras_get_pix_attitude)[1] + angle));
    return x_offset;
}
