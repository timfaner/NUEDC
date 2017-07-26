
#include "r_cg_cmt.h"
#include "r_cg_rspi.h"
#include "r_cg_sci.h"
#include "r_cg_macrodriver.h"
#include "r_cg_cgc.h"
#include "systemmonitor.h"
#include "stdlib.h"


//发送指定数据 分别是数据数组 数据个数 类型
void systemMonitor(uint8_t *arg,int arg_count,uint8_t data_type){
    static uint8_t send_counter;
    uint8_t* package;

    unsigned long current_time;
    int i;

    package =  malloc(arg_count + 6);

    for(i = 0;i < arg_count;i++){
        *(package+i+6) = *(arg + i);
    }

    *(package+4) = send_counter++;
    *(package+5) = data_type;
    current_time = millis();
    for(i = 0;i<4;i++){
        *(package+i) = (uint8_t) (current_time & 0xff);
        current_time = current_time >> 8;
    }
    SCI5_Serial_Send(package,arg_count+6);
    free(package);
    package = NULL;
}

void systemDataUpdate(float *data,int data_count,uint8_t data_type){
    unsigned char package[12][20];
    unsigned char send[256];
    int i = 0;
    int j = 0;
    int k = 0;
    int *length;
	unsigned char * text;
	
	length = malloc(data_count);
    for(i = 0;i< data_count;i++){
        uart_5_printf(package[i],"%.2f",*(data+i));
        text = package[i];
        while(*text!='\0'){
		    length[i]++;
		    text++;
	    }
        package[i][length[i]] = 255;
    
    }
    for(i = 0;i< data_count;i++){
        for(j = 0;j<length[i]+1;j++){
            send[k] = package[i][j];
            k++;
        }
    }
    systemMonitor(send,k,data_type);
}

void systemEventUpdate(uint16_t new_event){
    uint8_t package[2];

    system_event |= new_event;
    package[0] =(uint8_t) (system_event & 0xff);
    package[0] =(uint8_t) ((system_event>>8) & 0xff);
    systemMonitor(package,2,MONITOR_EVENT);
}

void systemErrorUpdate(uint8_t error){
    uint8_t errorcode;
    errorcode = error;
    systemMonitor(&errorcode,1,MONITOR_ERROR);
}

