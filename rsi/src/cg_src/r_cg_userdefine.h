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
* File Name    : r_cg_userdefine.h
* Version      : Code Generator for RX23T V1.00.01.01 [30 Oct 2015]
* Device(s)    : R5F523T5AxFM
* Tool-Chain   : CCRX
* Description  : This file includes user definition.
* Creation Date: 2017/7/17
***********************************************************************************************************************/
#ifndef _USER_DEF_H
#define _USER_DEF_H

/***********************************************************************************************************************
User definitions
***********************************************************************************************************************/
#define FAST_INTERRUPT_VECTOR 0

/* Start user code for function. Do not edit comment generated here */
#include "../Mavlink_lib/Mavlink_lib.h"
#include "user_main.h"

/*******************************************/
/***************port define*****************/
#define OPENMV_WORK_ENABLE_PIN  PORT9.PODR.BIT.B1
#define RSA_WORK_ENABLE_PIN 	PORT9.PIDR.BIT.B2

#define OPENMV_TASK_SWICH1		PORT7.PODR.BIT.B3
#define OPENMV_TASK_SWICH2 		PORT7.PODR.BIT.B4

#define RSA_TASK_SWICH1 		PORT7.PIDR.BIT.B6
#define RSA_TASK_SWICH2 		PORT7.PIDR.BIT.B5

/*****************const define****************/
#define Pi 3.1416

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
#define EVENT_BOOTUP 			1
#define EVENT_STARTBUTTON 		2
#define EVENT_NOLOCALPOSITION 	4
#define EVENT_ARMCHECK 			8
#define EVENT_TAKEOFF 			16
#define EVENT_OPENMVBOOTUP 		32
//#define EVENT_TRACKING 			64
//#define EVENT_LOSTLINE			128
//#define EVENT_PRELAND 			256
//#define EVENT_LANDING 			512
//#define EVENT_LANDED  			1024
//#define EVENT_OVERFLY 			2048
//#define EVENT_TASK1 			4096
//#define EVENT_TASK2 			8192
//#define EVENT_TASK3 			16384
//#define EVENT_TASK4 			32768


#define EVENT_XUNXIAN 1
#define EVENT_PRELAND 1
#define EVENT_OVERFLY 1
#define EVENT_LAND 1
#define EVENT_LANDED 1


//define system error
#define ERROR_TASK_NUMBER		 65
#define ERROR_SPI_DATA 	  		 66
#define ERROR_UNKNOWN_PLACE 	 67
#define ERROR_CANNOT_GET_DATA 	 68
#define ERROR_WRONG_ORDER  		 69

//define systerm data
#define DATA_OPENMV  			 97
#define DATA_X_OFFSET_RESULT 	 98
#define DATA_Y_OFFSET_RESULT 	 99
#define DATA_PID_X_SPEED 		 100
#define DATA_PID_Y_SPEED		 101
#define DATA_ATTITUDE 			 102
#define DATA_PID_PARAMETERS	 	 103
#define DATA_TASK_NUMBER         104
/* End user code. Do not edit comment generated here */
#endif
