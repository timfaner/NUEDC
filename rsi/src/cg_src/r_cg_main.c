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

/***********************************************************************************************************************
Global variables and functions
***********************************************************************************************************************/
/* Start user code for global. Do not edit comment generated here */

/*******************************************/
/***************task flag*******************/
#define task1_flag  0
#define task2_flag  1
#define task3_flag  2
#define task4_flag  3
uint8_t task_choose;
/*******************************************/

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
void rasTaskSwitch(void);
void rasCmdToOpenmv(uint8_t flag);
void rasWirelessAdjustParameters(void);
void rasOpenmvDataHandle(uint32_t * rx_buf);

/******************************************/

uint8_t rx_data[9];
uint32_t * rx_buffer = rx_data;
volatile uint8_t openmv_data[6] = {0};
volatile uint8_t openmv_data_flow[9] = {0,1,2,3,4,5,6,7,8};
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
    R_MAIN_UserInit();
    spi_receive(rx_buffer);
    /* Start user code. Do not edit comment generated here */
    rasTaskSwitch();
//	armcheck();
//	mav_takeoff(1.0);
    while (1U)
    {
    	if(task_choose == task1_flag)
    	{
    		task1();
    	}
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
//			mav_land();
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
void rasTaskSwitch(void)
{
	rasWirelessAdjustParameters();
	SCI5_Send_string("please choose task:");
	delay_ms(5000);
	if(!(PORT7.PIDR.BIT.B6) && !(PORT7.PIDR.BIT.B5))
	{
		SCI5_Send_string("choose task1");
		task_choose = task1_flag;
	}
	else if(!(PORT7.PIDR.BIT.B6) && PORT7.PIDR.BIT.B5 )
	{
		SCI5_Send_string("choose task2");
		task2();
	}
	else if(PORT7.PIDR.BIT.B6 && !(PORT7.PIDR.BIT.B5))
	{
		SCI5_Send_string("choose task3");
		task3();
	}
	else if(PORT7.PIDR.BIT.B6 && PORT7.PIDR.BIT.B5)
	{
		SCI5_Send_string("choose task4");
		task4();
	}
	else
		SCI5_Send_string("choose task error!");
}
void task1(void)
{
	uint8_t *data_to_send, count, *tx_buf;
	rasCmdToOpenmv(task1_flag);
	delay_ms(100);  //wait openmv initialize
	while(1)
	{

		for(count = 0; count < 9; count++)
		{
			sprintf(tx_buf, "%s", openmv_data_flow[count]);
			SCI5_Serial_Send(tx_buf, strlen(*tx_buf));
		}
		rasOpenmvDataHandle(openmv_data);
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
void rasCmdToOpenmv(uint8_t flag)
{
	if(task1_flag == flag)
	{
		PORT7.PODR.BIT.B3 = 0;
		PORT7.PODR.BIT.B4 = 0;
	}
	else if(task2_flag == flag)
	{
		PORT7.PODR.BIT.B3 = 1;
		PORT7.PODR.BIT.B4 = 0;
	}
	else if(task3_flag == flag)
	{
		PORT7.PODR.BIT.B3 = 0;
		PORT7.PODR.BIT.B4 = 1;
	}
	else if(task4_flag == flag)
	{
		PORT7.PODR.BIT.B3 = 1;
		PORT7.PODR.BIT.B4 = 1;
	}
	else
	{
		SCI5_Send_string("task command error!");
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
/* End user code. Do not edit comment generated here */
