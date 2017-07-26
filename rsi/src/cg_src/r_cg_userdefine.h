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
* Creation Date: 2017/7/22
***********************************************************************************************************************/
#ifndef _USER_DEF_H
#define _USER_DEF_H

/***********************************************************************************************************************
User definitions
***********************************************************************************************************************/
#define FAST_INTERRUPT_VECTOR 0

/* Start user code for function. Do not edit comment generated here */

#include "../Mavlink_lib/Mavlink_lib.h"



/*******************************************/
/***************port define*****************/
//定义硬件接口
#define OPENMV_WORK_ENABLE_PIN  PORT9.PODR.BIT.B1
#define RSA_WORK_ENABLE_PIN 	PORT9.PIDR.BIT.B2

#define OPENMV_TASK_SWICH1		PORT7.PODR.BIT.B3
#define OPENMV_TASK_SWICH2 		PORT7.PODR.BIT.B4

#define RSA_TASK_SWICH1 		PORT7.PIDR.BIT.B6
#define RSA_TASK_SWICH2 		PORT7.PIDR.BIT.B5

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



/* End user code. Do not edit comment generated here */
#endif
