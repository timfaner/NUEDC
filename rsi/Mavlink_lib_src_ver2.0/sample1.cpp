#include "math.h"
#include "mavlink.h"

#define SET_POSITION 3576
#define SET_VELOCITY 3527
#define GCS_ID 255
#define send_short_msg

class SCM
{
public:
	void init(
		unsigned short (*serialwrite)(unsigned char *buffer,uint16_t lenth),
		void (*Delay)(uint32_t),
		uint32_t (*Clock)(void),
		void (*Debug_text)(char *,...)
		);
protected:
	SCM(){};
	~SCM(){};
	int flag_data_updated[10];//0-LocalPoint,1-Attitude,
	void (*delay)(uint32_t);
	uint32_t (*clock)();
	void (*debug_text)(char *,...);
	unsigned short (*Serialwrite)(unsigned char *buffer,uint16_t lenth);
};

void SCM::init(
	unsigned short (*serialwrite)(unsigned char *buffer,uint16_t lenth),
	void (*Delay)(uint32_t),
	uint32_t (*Clock)(void),
	void (*Debug_text)(char *,...)
	)
{
	Serialwrite=serialwrite;
	clock=Clock;
	debug_text=Debug_text;
	delay=Delay;
}
class Mavlink:public SCM
{
protected:
	Mavlink();
	~Mavlink(){};
	bool S_heartbeat();
	bool S_command_long(MAV_CMD,float,float,float,float,float,float,float);
	bool S_set_mode(uint8_t base,uint32_t custom);
	bool S_set_position_local(uint16_t mask,float,float,float);
	bool data_request(uint8_t );
	bool home_position();
	bool Receive(unsigned char a);

	mavlink_attitude_t attitude;
	mavlink_optical_flow_t opt;
	mavlink_local_position_ned_t localpoint;
protected:
	uint8_t wait_result();
private:
	mavlink_message_t sendmsg;
	mavlink_status_t recvstatus;
	mavlink_message_t recvdata;
	unsigned char copydata[sizeof(mavlink_message_t)];
	unsigned char senddata[sizeof(mavlink_message_t)];

	mavlink_heartbeat_t recv_heartbeat;
	mavlink_command_ack_t comm_result;
	mavlink_statustext_t recv_text;

	int last_cmd;
	uint16_t cut();
};

Mavlink::Mavlink()
{

}

bool Mavlink::S_heartbeat()
{
	/*char test[18]={};*/
	mavlink_msg_heartbeat_pack(GCS_ID,0,&sendmsg,6,8,192,0,4);
	uint16_t valid_lenth=cut();
	/*for(i=0;i<17;i++){
		test[i]=senddata[i];
	}
	debug_text("heart: ");
	debug_text(test);
	debug_text("\n");*/
	return Serialwrite(senddata,valid_lenth);
}
bool Mavlink::S_command_long(MAV_CMD cmd,float a1,float a2,float a3,float a4,float a5,float a6,float a7)
{
	last_cmd=cmd;
	mavlink_msg_command_long_pack(GCS_ID,0,&sendmsg,1,1,cmd,0,a1,a2,a3,a4,a5,a6,a7);
	uint16_t valid_lenth=cut();
	return (Serialwrite(senddata,valid_lenth))&&(!(bool)wait_result());
}
bool Mavlink::S_set_mode(uint8_t base,uint32_t custom)
{
	delay(200);
	last_cmd=11;//SET_MODE
	//mavlink_msg_command_long_pack(GCS_ID,0,&sendmsg,1,1,MAV_CMD_DO_SET_MODE,0,base,custom,0,0,0,0,0);
	mavlink_msg_set_mode_pack(GCS_ID,0,&sendmsg,1,base,custom);
	uint16_t valid_lenth=cut();
	return (Serialwrite(senddata,valid_lenth))&&(!(bool)wait_result());
}
bool Mavlink::S_set_position_local(uint16_t mask,float a1,float a2,float a3)
{
	if(mask==SET_POSITION)
		mavlink_msg_set_position_target_local_ned_pack(GCS_ID,0,&sendmsg,clock(),1,1,1,mask,a1,a2,-a3,0,0,0,0,0,0,0,0);
	if(mask==SET_VELOCITY)
		mavlink_msg_set_position_target_local_ned_pack(GCS_ID,0,&sendmsg,clock(),1,1,1,mask,0,0,0,a1,a2,-a3,0,0,0,0,0);
	uint16_t valid_lenth=cut();
	return Serialwrite(senddata,valid_lenth);
}
bool Mavlink::data_request(uint8_t hz)
{
	delay(200);
	mavlink_msg_request_data_stream_pack(GCS_ID,0,&sendmsg,0,1,0,hz,1);
	uint16_t valid_lenth=cut();
	return Serialwrite(senddata,valid_lenth);
}
bool Mavlink::home_position()
{
	delay(200);
	float angle[4]={1,0,0,0};
	mavlink_msg_set_home_position_pack(GCS_ID,0,&sendmsg,1,1,1,0,0,0,0,angle,0,0,0);
	uint16_t valid_lenth=cut();
	debug_text("set home position\n");
	return Serialwrite(senddata,valid_lenth);
}
uint8_t Mavlink::wait_result()
{
	for(uint8_t i=0;i<20;i++)
	{
		if(comm_result.command==last_cmd)
			return comm_result.result;
		delay(150);
	}
	debug_text("no result");
	return 5;
}

bool Mavlink::Receive(unsigned char a)
{
	static int local_position_recv_count=0;
	if(mavlink_parse_char(MAVLINK_COMM_0,a,&recvdata,&recvstatus))
	{
		switch(recvdata.msgid)
		{
		case 0: break;		//mavlink_msg_heartbeat_decode(&recvdata,&recv_heartbeat);
		case 77: mavlink_msg_command_ack_decode(&recvdata,&comm_result);  break;
		case 253:
			mavlink_msg_statustext_decode(&recvdata,&recv_text);
			debug_text(recv_text.text);
			break;
		case 100: mavlink_msg_optical_flow_decode(&recvdata,&opt); break;
		case 32:
			local_position_recv_count++;
			mavlink_msg_local_position_ned_decode(&recvdata,&localpoint);
			flag_data_updated[0]=1;
			if(local_position_recv_count==3){
				local_position_recv_count=0;
				debug_text("LocalPoint: %.3f %.3f %.3f \n",localpoint.x,localpoint.y,localpoint.z);
			}
			break;
		case 30:
			mavlink_msg_attitude_decode(&recvdata,&attitude);
			flag_data_updated[1]=1;
			break;
			//case 1: break;
			//case 1: break;
			//case 1: break;
			//case 1: break;
			//case 1: break;
		}
		return true;
	}
	return false;
}



/*
bool Mavlink::Receive(unsigned char a)
{
	int local_position_recv_count=0;
	if(mavlink_parse_char(MAVLINK_COMM_0,a,&recvdata,&recvstatus))
	{
		if(!*flag_in_flight){//Not in Flight Mode,Parse More Message
			switch(recvdata.msgid)
			{
			case 0: break;		//mavlink_msg_heartbeat_decode(&recvdata,&recv_heartbeat);
			case 77: mavlink_msg_command_ack_decode(&recvdata,&comm_result);  break;
			case 253:
				mavlink_msg_statustext_decode(&recvdata,&recv_text);
				debug_text(recv_text.text);
				break;
			case 100: mavlink_msg_optical_flow_decode(&recvdata,&opt); break;
			case 32:
				local_position_recv_count++;
				mavlink_msg_local_position_ned_decode(&recvdata,&localpoint);
				if(local_position_recv_count==3){
					local_position_recv_count=0;
					debug_text("LocalPoint: %.3f %.3f %.3f \n",localpoint.x,localpoint.y,localpoint.z);
				}
				break;
			case 30: mavlink_msg_attitude_decode(&recvdata,&attitude);break;
				//case 1: break;
				//case 1: break;
				//case 1: break;
				//case 1: break;
				//case 1: break;
			}
			return true;
		}
		else{//In Flight Mode,Parse Necessary Information
			switch(recvdata.msgid)
				{
				case 0: break;		//mavlink_msg_heartbeat_decode(&recvdata,&recv_heartbeat);
				case 77: mavlink_msg_command_ack_decode(&recvdata,&comm_result);  break;
//				case 253:
//					mavlink_msg_statustext_decode(&recvdata,&recv_text);
//					debug_text(recv_text.text);
//					break;
//				case 100: mavlink_msg_optical_flow_decode(&recvdata,&opt); break;
				case 32:
					local_position_recv_count++;
					mavlink_msg_local_position_ned_decode(&recvdata,&localpoint);
					if(local_position_recv_count==3){
						local_position_recv_count=0;
						debug_text("LocalPoint: %.3f %.3f %.3f \n",localpoint.x,localpoint.y,localpoint.z);
					}
					break;
				case 30: mavlink_msg_attitude_decode(&recvdata,&attitude);break;
					//case 1: break;
					//case 1: break;
					//case 1: break;
					//case 1: break;
					//case 1: break;
				}
				return true;
		}
	}
	return false;
}
*/
uint16_t Mavlink::cut()
{
#ifdef send_short_msg
	int i=0;
	uint16_t valid_lenth=sendmsg.len+8;
	memset(senddata,'\0',sizeof(mavlink_message_t));
	memcpy(copydata,&sendmsg,sizeof(mavlink_message_t));
	while(i<valid_lenth)
	{
		senddata[i]=copydata[i+2];
		i++;
	}
	return valid_lenth;
#else
	memcpy(senddata,&sendmsg,sizeof(mavlink_message_t));
	return sizeof(mavlink_message_t);
#endif // send_short_msg
}

class Mavcontrol:public Mavlink
{
public:
	Mavcontrol(){};
	~Mavcontrol(){};
	bool armcheck(void);
	bool keep_heartbeat(void);
	bool mav_takeoff(float);
	bool mav_land(void);
	bool set_vel(float,float,float);
	bool set_point(float,float,float);
	bool emergency_stop();
	bool receive(unsigned char a);
	int*  	getFlagDataUpdated();
	float*  getHeight();
	float** getLocalPosition();
	float** getAttitude();
	float fuck();

private:
};

bool Mavcontrol::armcheck(void)
{

	if(!data_request(50))
	{
		debug_text("request data failed\n");
		return false;
	}
	if(!home_position())
	{
		debug_text("set home position failed\n");
		return false;
	}
	if(!S_set_mode(1,4))
	{
		debug_text("set mode failed\n");
		return false;
	}
	delay(200);
	if(!S_command_long(MAV_CMD_COMPONENT_ARM_DISARM,1,0,0,0,0,0,0))
	{
		debug_text("arm denied\n");
		return false;
	}
	return true;
}
bool Mavcontrol::keep_heartbeat(void)
{
	return S_heartbeat();
}
bool Mavcontrol::mav_takeoff(float height)
{
	if(!S_command_long(MAV_CMD_NAV_TAKEOFF,0,0,0,0,0,0,height))
	{
		debug_text("takeoff denied\n");
		return false;
	}
	return true;
}

bool Mavcontrol::set_vel(float a1,float a2,float a3)
{
	return S_set_position_local(SET_VELOCITY,a1,a2,a3);
}
bool Mavcontrol::set_point(float a1,float a2,float a3)
{
	return S_set_position_local(SET_POSITION,a1,a2,a3);
}
bool Mavcontrol::mav_land(void)
{
	set_point(localpoint.x,localpoint.y,localpoint.z);
	delay(1000);
	return S_command_long(MAV_CMD_NAV_LAND,0,0,0,0,localpoint.x,localpoint.y,0);
}

bool Mavcontrol::emergency_stop()
{
	return true;
}
bool Mavcontrol::receive(unsigned char a)
{
	return Receive(a);
}
int* Mavcontrol::getFlagDataUpdated()
{
	return flag_data_updated;
}
float* Mavcontrol::getHeight()
{
	return &opt.ground_distance;
}
float** Mavcontrol::getLocalPosition()
{
	float *local_position_list[3];//NED x y x
	local_position_list[0]=&localpoint.x;
	local_position_list[1]=&localpoint.y;
	local_position_list[2]=&localpoint.z;
	return local_position_list;
}
float** Mavcontrol::getAttitude()
{
	float *attitude_list[3];//Roll Pitch Yaw
	attitude_list[0]=&attitude.roll;
	attitude_list[1]=&attitude.pitch;
	attitude_list[2]=&attitude.yaw;
	return attitude_list;
}
float Mavcontrol::fuck()
{
	float a=0.111111;
	return a;
}
#include "Mavcontrol.h"
