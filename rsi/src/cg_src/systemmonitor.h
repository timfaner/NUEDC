#define MONITOR_ERROR 1
#define MONITOR_EVENT 2
#define MONITOR_DATA_TASK_NUMBER 3
#define MONITOR_DATA_OPENMV_DATA 4
extern uint16_t system_event;
extern uint8_t system_error_code;

void systemMonitor(uint8_t *arg,int count,uint8_t data_type);
void systemEventUpdate(uint16_t);
void systemErrorUpdate(uint8_t);