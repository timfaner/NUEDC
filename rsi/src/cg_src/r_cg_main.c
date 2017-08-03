/***********************************************************************************************************************
* DISCLAIMER
* This software is supplied by Renesas Electronics Corporation and is only intended for use with Renesas products.
* No other uses are authorized. This software is owned by Renesas Electronics Corporation and is protected under all
* applicable laws, including copyright laws. 
* THIS SOFTWARE IS PROVIDED "AS IS" AND RENESAS MAKES NO WARRANTIESREGARDING THIS SOFTWARE, WHETHER EXPRESS, IMPLIED
* OR STATUTORY, INCLUDING BUT NOT LIMITED TO WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
* NON-INFRINGEMENT.  ALL SUCH WARRANTIES ARE EXPRESSLY DISCLAIMED.TO THE MAXIMUM EXTENT PERMITTED NOT PROHIBITED BY
* LAW, NEITHER RENESAS ELECTRONICS CORPORATION NOR ANY OF ITS AFFILIATED COMPANIES SHALL BE LIABLE FOR ANY DIRECT,
* INDIRECT, SPECIAL, INCIDENTAL OR CONSEQUENTIAL DAMAGES FOR ANY REASON RELATED TO THIS SOFTWARE, EVEN IF RENESAS OR
* ITS AFFILIATES HAVE BEEN ADVISED OF THE POSSIBILITY OF SUCH DAMAGES.
* Renesas reserves the right, without notice, to make changes to this software and to discontinue the availability 
* of this software. By using this software, you agree to the additional terms and conditions found by accessing the 
* following link:
* http://www.renesas.com/disclaimer
*
* Copyright (C) 2015 Renesas Electronics Corporation. All rights reserved.
***********************************************************************************************************************/

/***********************************************************************************************************************
* File Name    : r_cg_main.c
* Version      : Code Generator for RX23T V1.00.01.01 [30 Oct 2015]
* Device(s)    : R5F523T5AxFM
* Tool-Chain   : CCRX
* Description  : This file implements main function.
* Creation Date: 2017/7/17
***********************************************************************************************************************/

/***********************************************************************************************************************
Pragma directive
***********************************************************************************************************************/
/* Start user code for pragma. Do not edit comment generated here */
/* End user code. Do not edit comment generated here */

/***********************************************************************************************************************
Includes
***********************************************************************************************************************/
#include "r_cg_macrodriver.h"
#include "r_cg_cgc.h"
#include "r_cg_cmt.h"
#include "r_cg_sci.h"
/* Start user code for include. Do not edit comment generated here */
#include "stdio.h"
#include "string.h"
#include "stdlib.h"
#include "math.h"

#include "pid.h"
#include "systemmonitor.h"
#include "offset_calculate.h"
/* End user code. Do not edit comment generated here */
#include "r_cg_userdefine.h"

/***********************************************************************************************************************
Global variables and functions
***********************************************************************************************************************/
/* Start user code for global. Do not edit comment generated here */



#define TASK_HEIGHT 0.7
#define TASK1_X_SPEED 0.3
#define TASK1_X_SPEED_OVERFLY -0.15
/*************spi parameters**********************/
uint8_t rx_data[9];
uint32_t * rx_buffer = rx_data;
volatile uint8_t openmv_data[6] = {255,255,255,255,255,255};
//openmv_data锛歟rror mav_statu x y landflag
volatile uint8_t openmv_data_flow[9] = {0,1,2,3,4,5,6,7,8};
/*************************************************/

volatile uint16_t system_event;
volatile uint8_t system_error_code;

/*********pid parameters********/
double x_input= 0.0, x_output, setpoint=0.0,y_input = 0.0, y_output;
double kp=5.0, ki=6.0, kd=3.0;

/***************functions******************/
void task1(void);
void task2(void);
void task3(void);
void task4(void);
int rasTaskSwitch(void);
void rasCmdToOpenmv(uint8_t flag);
//void rasWirelessAdjustParameters(void);
void taskError(uint8_t);
/******************************************/

/*************ver2.0********************/
int armflag=0;
int* flag_data_updated;
float** apm_attitude;
float *apm_height = NULL;
unsigned char recv_char;
unsigned long takeoff_time=0;

unsigned long last_heartbeat_time=0;
unsigned long last_cycle_time=0;
unsigned long this_cycle_time=0;
unsigned long runtime=0;
int run_count=0;
/***************************************/

/* End user code. Do not edit comment generated here */

void R_MAIN_UserInit(void);
/***********************************************************************************************************************
* Function Name: main
* Description  : This function implements main function.
* Arguments    : None
* Return Value : None
***********************************************************************************************************************/
void main(void)
{
	int task_number = 0;
//	float task[7] = {4.0, 5.0, 6.34, 6.789, 1.234, 2.4, 4.567};
    /* Start user code. Do not edit comment generated here */
	//initial
	R_MAIN_UserInit();
    //get data from openmv
	


    /* Start user code. Do not edit comment generated here */
    task_number = rasTaskSwitch();
    rasCmdToOpenmv(task_number); //切换openmv任务
	systemMonitor(&task_number,1,MONITOR_DATA_TASK_NUMBER);

	//添加开始开关
	while(RSA_WORK_ENABLE_PIN ==1){
		delay_ms(10);
	}
	systemEventUpdate(EVENT_STARTBUTTON);

	//倒计时
	armcheck();
	systemEventUpdate(EVENT_ARMCHECK);

	mav_takeoff(TASK_HEIGHT);
	systemEventUpdate(EVENT_TAKEOFF);

	while (*(apm_height) < TASK_HEIGHT-0.1)
		delay_ms(40);

	rasCmdToOpenmv(task_number);

	OPENMV_WORK_ENABLE_PIN = 1; //通知openmv开始工作 将该引脚置高
	systemEventUpdate(EVENT_OPENMVBOOTUP);
	delay_ms(100);  //wait openmv initialize

	debug_text("openmv initialized");

    switch (task_number){
    	case TASK1:
    		task1();
    		break;
    	case TASK2:
    		task2();
    		break;
    	case TASK3:
    		task3();
    		break;
    	case TASK4:
    		task4();
    		break;
    	case -1:
    		break;
    }
    /* End user code. Do not edit comment generated here */
}
/***********************************************************************************************************************
* Function Name: R_MAIN_UserInit
* Description  : This function adds user code before implementing main function.
* Arguments    : None
* Return Value : None
***********************************************************************************************************************/
void R_MAIN_UserInit(void)
{
    /* Start user code. Do not edit comment generated here */
	SCI1_Start();
	SCI5_Start();
	R_CMT2_Start();
	R_CMT3_Start();
	R_RSPI0_Start();
	init(SCI1_Serial_Send,delay_ms,millis,uart_5_printf);
	spiReceive(rx_buffer);

	flag_data_updated=getFlagDataUpdated();
	apm_attitude=getAttitude();
	apm_height = get_height();
	wait_link();
	requestDataStream(500,50,50);
	debug_text("\nRx Initialized\n");

	xPID(&x_input, &x_output, &setpoint, kp, ki, kd, DIRECT);
	xSetMode(AUTOMATIC);
	yPID(&y_input, &y_output, &setpoint, kp, ki, kd, DIRECT);
	ySetMode(AUTOMATIC);
    /* End user code. Do not edit comment generated here */
}

/* Start user code for adding. Do not edit comment generated here */
int rasTaskSwitch(void)
{

	if(!(RSA_TASK_SWICH1) && !(RSA_TASK_SWICH2))
	{
		debug_text("\nchoose tast1\n");
		return TASK1;
	}
	else if(!(RSA_TASK_SWICH1) && RSA_TASK_SWICH2 )
	{
		return TASK2;
	}
	else if(RSA_TASK_SWICH1 && !(RSA_TASK_SWICH2))
	{
		return TASK3;
	}
	else if(RSA_TASK_SWICH1 && RSA_TASK_SWICH2)
	{
		return TASK4;
	}
	else
		return -1;
}

void task1(void)
{
	int land_mark = 0;
	float x_offset, y_offset, x_speed, y_speed;
	debug_text("\nworking task1\n");
	while(1){
		//if(land_ma=rk == landed)break;


		if(openmv_data[ERROR_FLAG] == 0)
		{
			debug_text("error_flag = 0");
			systemMonitor(openmv_data,5,MONITOR_DATA_OPENMV_DATA);
			if(openmv_data[LAND_FLAG] ==1){

				systemEventUpdate(EVENT_LAND);

				mav_land();
			
				// while(land_mark != LANDED){
				// 	updata land mark
				// 	and updata EVENT_LANDED
				// }
			}
			else{ 
				switch (openmv_data[MAV_STATUS]){
					case MAV_STATUS_INIT:
						break;
					case MAV_STATUS_TAKEOFF:
						systemEventUpdate(EVENT_XUNXIAN);
						//dataFushion();
						y_offset = rasY_offsetCalculate(openmv_data[2]);
						//pid
						y_input = y_offset;
						yCompute(&y_input);
						y_speed = y_output;
						//send to apm
						set_new_vel(TASK1_X_SPEED, y_speed, TASK_HEIGHT);
						
						systemDataUpdate(&y_speed,1,DATA_PID_Y_SPEED);
						break;
					case MAV_STATUS_FLYING:
						systemEventUpdate(EVENT_XUNXIAN);

						y_offset = rasY_offsetCalculate(openmv_data[2]);
						//pid
						y_input = y_offset;
						yCompute(&y_input);
						y_speed = y_output;

						//send to apm
						set_new_vel(TASK1_X_SPEED, y_speed, TASK_HEIGHT);
						systemDataUpdate(&y_speed,1,DATA_PID_Y_SPEED);
						break;

					case MAV_STATUS_PRELAND:
						systemEventUpdate(EVENT_PRELAND);
						//dataFushion();
						x_offset = rasX_offsetCalculate(openmv_data[3]);
						y_offset = rasY_offsetCalculate(openmv_data[2]);
						//pid
						y_input = y_offset;
						yCompute(&y_input);
						y_speed = y_output;

						x_input = x_offset;
						xCompute(&x_input);
						x_speed = x_output;
						//send to apm
						
						set_new_vel(x_speed, y_speed, TASK_HEIGHT);
						systemDataUpdate(&y_speed,1,DATA_PID_Y_SPEED);
						systemDataUpdate(&x_speed,1,DATA_PID_X_SPEED);
						break;
					case MAV_STATUS_OVEFRFLY:
						systemEventUpdate(EVENT_OVERFLY);
						set_new_vel(TASK1_X_SPEED_OVERFLY, 0, TASK_HEIGHT);
						break;
					case MAV_STATUS_ERROR:
						break;
				}

			}
		}
		else
			taskError(openmv_data[ERROR_FLAG]);

	}

}
void task2(void)
{

}
void task3(void)
{

}
void task4(void)
{

}

void taskError(uint8_t openmverror){
	// land
}
void rasCmdToOpenmv(uint8_t flag)
{
	if(flag ==TASK1)
	{
		OPENMV_TASK_SWICH1 = 0;
		OPENMV_TASK_SWICH2 = 0;
	}
	else if(flag ==TASK2)
	{
		OPENMV_TASK_SWICH1 = 1;
		OPENMV_TASK_SWICH2 = 0;
	}
	else if(flag ==TASK3)
	{
		OPENMV_TASK_SWICH1 = 0;
		OPENMV_TASK_SWICH2 = 1;
	}
	else if(flag ==TASK4)
	{
		OPENMV_TASK_SWICH1 = 1;
		OPENMV_TASK_SWICH2 = 1;
	}
	else
	{
		systemErrorUpdate(ERROR_TASK_NUMBER);
	}
}
/* End user code. Do not edit comment generated here */
