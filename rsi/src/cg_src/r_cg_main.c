/***********************************************************************************************************************
`* DISCLAIMER
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
* Creation Date: 2017/7/22
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
#include "r_cg_rspi.h"
#include "r_cg_sci.h"
/* Start user code for include. Do not edit comment generated here */
#include "pid_speed.h"
#include "stdlib.h"
/* End user code. Do not edit comment generated here */
#include "r_cg_userdefine.h"
#include "systemmonitor.h"

/***********************************************************************************************************************
Global variables and functions
***********************************************************************************************************************/
/* Start user code for global. Do not edit comment generated here */

/*******************************************/
/***************task flag*******************/
//定义硬件接口
#define OPENMV_WORK_ENABLE_PIN PORT9.PODR.BIT.B1
#define RSA_WORK_ENABLE_PIN PORT9.PIDR.BIT.B2

#define OPENMV_TASK_SWICH1 PORT7.PODR.BIT.B3
#define OPENMV_TASK_SWICH2 PORT7.PODR.BIT.B4

#define RSA_TASK_SWICH1 PORT7.PIDR.BIT.B6
#define RSA_TASK_SWICH2 PORT7.PIDR.BIT.B5

/***************task flag*******************/

#define TASK1  1
#define TASK2  2
#define TASK3  3
#define TASK4  4

/*******************************************/
//define openmv_data means
#define ERROR_FLAG 0
#define MAV_STATUS 1
#define X 2
#define Y 3
#define LAND_FLAG 4

#define MAV_STATUS_INIT 0
#define MAV_STATUS_TAKEOFF 1
#define MAV_STATUS_FLYING 2
#define MAV_STATUS_PRELAND 3
#define MAV_STATUS_OVEFRFLY 4
#define MAV_STATUS_ERROR 255

//define system envet
#define EVENT_BOOTUP 1
#define EVENT_STARTBUTTON 2
#define EVENT_ARMCHECK 4
#define EVENT_TAKEOFF 8
#define EVENT_OPENMVBOOTUP 16

#define EVENT_XUNXIAN 1
#define EVENT_PRELAND 1
#define EVENT_OVERFLY 1
#define EVENT_LAND 1
#define EVENT_LANDED 1
//define system error
#define ERROR_TASK_NUMBER 1

/******************************************/
/***************pid parameters*************/
#define tracking_target 90
#define landing_target 90
float *Kp, *Ki, *Kd;
/******************************************/

/******************************************/
/***************functions******************/
void task1(void);
void task2(void);
void task3(void);
void task4(void);
int rasTaskSwitch(void);
void rasCmdToOpenmv(uint8_t flag);
void rasWirelessAdjustParameters(void);
void rasOpenmvDataHandle(uint32_t * rx_buf);
void taskError(uint8_t);

/******************************************/

uint8_t rx_data[9];
uint32_t * rx_buffer = rx_data;
volatile uint8_t openmv_data[6] = {255,255,255,255,255,255};
//openmv_data：error mav_statu x y landflag
volatile uint8_t openmv_data_flow[9] = {0,1,2,3,4,5,6,7,8};
uint16_t system_event;
uint8_t system_error_code;
/* End user code. Do not edit comment generated here */


void R_MAIN_UserInit(void);
/***********************************************************************************************************************
* Function Name: main
* Description  : This function implements main function.
* Arguments    : None
* Return Value : None
***********************************************************************************************************************/
void main(void)
{	int task_number = 0;

    R_MAIN_UserInit();
	spiReceive(rx_buffer);

	systemEventUpdate(EVENT_BOOTUP);



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



	//	armcheck();
	systemEventUpdate(EVENT_ARMCHECK);
	//	mav_takeoff(1.0);
	systemEventUpdate(EVENT_TAKEOFF);

	while (getHeight() < 1.0)
		delay_ms(40);

	rasCmdToOpenmv(task_number);
	OPENMV_WORK_ENABLE_PIN = 1; //通知openmv开始工作 将该引脚置高
	systemEventUpdate(EVENT_OPENMVBOOTUP);
	delay_ms(100);  //wait openmv initialize


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


	//	init(SCI1_Serial_Send,delay_ms,millis,uart_5_printf);
	//	flag_data_updated=getFlagDataUpdated();
	//	apm_attitude=getAttitude();
	//	wait_link();
	//	requestDataStream();
	//
	//	debug_text("\nRx Initialized\n");

    /* End user code. Do not edit comment generated here */
}


/* Start user code for adding. Do not edit comment generated here */
void rasOpenmvDataHandle(uint8_t * rx_buf)
{
	float x_speed, y_speed;
	if(rx_buf[0] == 0)
	{
		SCI5_Send_string("Working well");
		if(rx_buf[1] == 0)
			SCI5_Send_string("Openmv Initialing");
		else if(rx_buf[1] == 1)
			SCI5_Send_string("Taking off");
		else if(rx_buf[1] == 2)//working position of
		{
			SCI5_Send_string("Go ahead");
			x_speed = pid_start(tracking_target,rx_buf[2]);
//			set_vel(x_speed, 0.0 ,0.0);
        	delay_ms(2000);
//        	set_vel(0,0,0);
		}
		else if(rx_buf[1] == 3)//landing position
		{
			SCI5_Send_string("pre_Land");
			x_speed = pid_start(landing_target, rx_buf[2]);
			y_speed = pid_start(landing_target, rx_buf[3]);
//			set_vel(x_speed, y_speed, 0);
        	delay_ms(2000);
//        	set_vel(0,0,0);
		}
		else if(rx_buf[1] == 4)
			SCI5_Send_string("Go backward");
		else if(rx_buf[1] == 255)
			SCI5_Send_string("mav_status error");
		else
			SCI5_Send_string("status data error");

		if(rx_buf[4] == 1)
		{
			SCI5_Send_string("Landing");
//
		}
	}
	else if(rx_buf[0] == 1)
	{
		SCI5_Send_string("Unknown position");
	}
	else if(rx_buf[0] == 2)
	{
		SCI5_Send_string("can not get color data");
		//rise cmd?
	}
	else if(rx_buf[0] == 3)
	{
		SCI5_Send_string("wrong task order");
	}
	else
		SCI5_Send_string("wrong error_flag data");
}

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
{	int land_mark = 0;

	while(1){
		//if(land_mark == landed)break;

		if(openmv_data[ERROR_FLAG] == 0)
		{	systemMonitor(openmv_data,5,MONITOR_DATA_OPENMV_DATA);
			rasOpenmvDataHandle(openmv_data);

			if(openmv_data[LAND_FLAG] !=1){ //开始降落
				systemEventUpdate(EVENT_LAND);
				//mav_land();
				// while(land_mark != LANDED){
				// 	updata land mark
				// 	and updata EVENT_LANDED
				// }
			}
			else{ //降落前的调整
				switch (openmv_data[MAV_STATUS]){
					case MAV_STATUS_INIT:
						break;
					case MAV_STATUS_TAKEOFF:
						systemEventUpdate(EVENT_XUNXIAN);
						//dataFushion();
						//pid
						//send to apm
						break;
					case MAV_STATUS_FLYING:
						systemEventUpdate(EVENT_XUNXIAN);
						//dataFushion();
						//pid
						//send to apm
						break;
					case MAV_STATUS_PRELAND:
						systemEventUpdate(EVENT_PRELAND);
						//dataFushion();
						//pid
						//send to apm
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

void rasWirelessAdjustParameters(void)
{
	unsigned char ch, *sci_Kp, *sci_Ki, *sci_Kd;
	SCI5_Send_string("Parameters adjust");
	SCI5_Serial_Receive(&ch, 1);
	if(ch == 'Y')
	{
		SCI5_Send_string("input Kp");
		SCI5_Serial_Receive(sci_Kp, 5);
		*Kp = atof(*sci_Kp);

		SCI5_Send_string("input Ki");
		SCI5_Serial_Receive(sci_Ki, 5);
		*Ki = atof(*sci_Ki);

		SCI5_Send_string("input Kd");
		SCI5_Serial_Receive(sci_Kd, 5);
		*Kd = atof(*sci_Kd);

		pid_init(*Kp, *Ki, *Kd);
		SCI5_Send_string("Parameters changed");
	}
	else if(ch == 'N')
	{
		SCI5_Send_string("use the last parameters");
		pid_init(*Kp, *Ki, *Kd);
	}
	else
	{
		SCI5_Send_string("cmd error");
	}
}

