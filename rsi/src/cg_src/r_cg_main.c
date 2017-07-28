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
#include "pid_speed.h"
#include "stdio.h"
#include "string.h"
#include "stdlib.h"
#include "math.h"
#include "systemmonitor.h"
#include "offset_calculate.h"
/* End user code. Do not edit comment generated here */
#include "r_cg_userdefine.h"

/***********************************************************************************************************************
Global variables and functions
***********************************************************************************************************************/
/* Start user code for global. Do not edit comment generated here */

/******************************************/
/*********Renesas get PIX attiaude*********/
volatile float ** ras_get_pix_attitude;
/******************************************/

/*************spi parameters**********************/
uint8_t rx_data[9];
uint32_t * rx_buffer = rx_data;
volatile uint8_t openmv_data[6] = {255,255,255,255,255,255};
//openmv_data锛歟rror mav_statu x y landflag
volatile uint8_t openmv_data_flow[9] = {0,1,2,3,4,5,6,7,8};
/*************************************************/

volatile uint16_t system_event;
volatile uint8_t system_error_code;
/******************************************/
/***************pid parameters*************/
#define x_target 60
#define y_target 80
float *Kp, *Ki, *Kd;
/******************************************/

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
	float test[3]={6.78,7.44,99.0};
	uint32_t current_time;
	uint8_t * current_time_str;
    R_MAIN_UserInit();
    /* Start user code. Do not edit comment generated here */

    spiReceive(rx_buffer);


     current_time = 998;
     uart_5_printf(current_time_str,"11%d\n",222);

     systemDataUpdate(test,3,'A');
     systemDataUpdate(test,3,'E');

//    while (1U)
//    {
//    	run_count++;
//    	this_cycle_time=millis();
//    	if(this_cycle_time-last_heartbeat_time>1500){
//    		S_heartbeat();
//    	}
//    	if(run_count==1000){
//    		runtime=this_cycle_time-last_cycle_time;
//    		last_cycle_time=this_cycle_time;
//    		uart_5_printf("1000 runtime: %d ms\n",runtime);
//    		run_count=0;
//    	}
//    	//delay_ms(1000);
//    	if(takeoff_time){
//    		if(millis()-takeoff_time>15000){
//    			mav_land();
//    			debug_text("timeout autoland\n");
//    			takeoff_time=0;
//    		}
//    	}
//        if(sci5_receive_available()){
//        	SCI5_Serial_Receive(&recv_char,1);
//        	debug_text("rx receive data: ");
//        	SCI5_Serial_Send(&recv_char,1);
//        }
//        if(flag_data_updated[1]){//attitude updated
//        	flag_data_updated[1]=0;
//        	uart_5_printf("        Attitude Roll %f\n",*apm_attitude[0]);
//        }
//        set_vel(0,-0.5,0);
//        switch(recv_char){
//        case 'a':
//        	armcheck();
//        	break;
//        case 't':
//        	mav_takeoff(1.0);
//        	takeoff_time=millis();
//        	break;
//        case 'l':
//        	mav_land();
//        	break;
//        case 'f':
//        	set_vel(0,-0.5,0);
//			delay_ms(2000);
//        	set_vel(0,0,0);
//        	break;
//        case 'r':
//        	set_vel(0.3,0,0);
//        	delay_ms(2000);
//        	set_vel(0,0,0);
//        	break;
//        default:
//        }
//        recv_char='\0';
//
//    }
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
	init(SCI1_Serial_Send,delay_ms,millis,uart_5_printf);
	flag_data_updated=getFlagDataUpdated();
	apm_attitude=getAttitude();
	wait_link();

	debug_text("\nRx Initialized\n");
    /* End user code. Do not edit comment generated here */
}

/* Start user code for adding. Do not edit comment generated here */
int rasTaskSwitch(void)
{

	if(!(RSA_TASK_SWICH1) && !(RSA_TASK_SWICH2))
	{
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
	while(1){
		//if(land_ma=rk == landed)break;

		if(openmv_data[ERROR_FLAG] == 0)
		{
			systemMonitor(openmv_data,5,MONITOR_DATA_OPENMV_DATA);

			if(openmv_data[LAND_FLAG] !=1){ //寮€濮嬮檷钀?
				systemEventUpdate(EVENT_LAND);
//				mav_land();
				// while(land_mark != LANDED){
				// 	updata land mark
				// 	and updata EVENT_LANDED
				// }
			}
			else{ //闄嶈惤鍓嶇殑璋冩暣
				switch (openmv_data[MAV_STATUS]){
					case MAV_STATUS_INIT:
						break;
					case MAV_STATUS_TAKEOFF:
						systemEventUpdate(EVENT_XUNXIAN);
						//dataFushion();
						y_offset = rasY_offsetCalculate(openmv_data[2]);
						//pid
						y_speed = pid_start(y_target, y_offset);
						//send to apm
						set_vel(0.0, y_speed, 0.0);
						break;
					case MAV_STATUS_FLYING:
						systemEventUpdate(EVENT_XUNXIAN);
						//dataFushion();
						y_offset = rasY_offsetCalculate(openmv_data[2]);
						//pid
						y_speed = pid_start(y_target, y_offset);
						//send to apm
						set_vel(0.0, y_speed, 0.0);
						break;
					case MAV_STATUS_PRELAND:
						systemEventUpdate(EVENT_PRELAND);
						//dataFushion();
						x_offset = rasX_offsetCalculate(openmv_data[3]);
						y_offset = rasY_offsetCalculate(openmv_data[2]);
						//pid
						x_speed = pid_start(x_target, x_offset);
						y_speed = pid_start(y_target, y_offset);
						//send to apm
						set_vel(x_speed, y_speed, 0.0);
						break;
					case MAV_STATUS_OVEFRFLY:
						systemEventUpdate(EVENT_OVERFLY);
						//dataFushion();
						//pid
						//send to apm
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
