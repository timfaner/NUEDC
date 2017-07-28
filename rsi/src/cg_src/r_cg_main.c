
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
/***************pid parameters*************/
#define x_target 60
#define y_target 80
float *Kp, *Ki, *Kd;
/******************************************/

/******************************************/
/*********Renesas get PIX attiaude*********/
volatile float ** ras_get_pix_attitude;
/******************************************/

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

/*************************************************/
/*************spi parameters**********************/
uint8_t rx_data[9];
uint32_t * rx_buffer = rx_data;
volatile uint8_t openmv_data[6] = {255,255,255,255,255,255};
//openmv_data：error mav_statu x y landflag
volatile uint8_t openmv_data_flow[9] = {0,1,2,3,4,5,6,7,8};
/*************************************************/
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
{
    int task_number = 0;
    //initial
    R_MAIN_UserInit();
    //get data from openmv
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

        armcheck();
    systemEventUpdate(EVENT_ARMCHECK);
        mav_takeoff(1.0);
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

//      init(SCI1_Serial_Send,delay_ms,millis,uart_5_printf);
//      flag_data_updated=getFlagDataUpdated();
//      apm_attitude=getAttitude();
//      wait_link();
//      requestDataStream();

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

            if(openmv_data[LAND_FLAG] !=1){ //开始降落
                systemEventUpdate(EVENT_LAND);
//              mav_land();
                // while(land_mark != LANDED){
                //  updata land mark
                //  and updata EVENT_LANDED
                // }
            }
            else{ //降落前的调整
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

/**********reserved******this function perhaps will be used later*****
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
*/

