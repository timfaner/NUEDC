#include "r_cg_cmt.h"
#include "r_cg_rspi.h"
#include "r_cg_sci.h"
#include "r_cg_macrodriver.h"
#include "r_cg_cgc.h"

//发送指定数据 分别是数据数组 数据个数 类型
void systemMonitor(uint8_t *arg,int arg_count,uint8_t data_type){
    static uint8_t send_counter;
    uint8_t package[count +6] = [0];
    unsigned long current_time;
    int i;

    for(i = 0;i < arg_count;i++){
        package[i+6] = *(arg + i);
    }

    package[4] = send_counter++;
    package[5] = data_type;
    current_time = millis();
    for(i = 0;i<4;i++){
        package[i] = (uint8_t) (current_time & 0xff);
        current_time = current_time >> 8;
    }
    SCI5_Serial_Send(package,arg_count+6,data_type);
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

