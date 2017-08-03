Mavcontrol m;
extern "C" bool S_heartbeat()
{
	return m.keep_heartbeat();
}
extern "C" bool mav_takeoff(float h)
{
	return m.mav_takeoff(h);
}
extern "C" bool mav_land()
{
	return m.mav_land();
}
extern "C" bool set_vel(float a1,float a2,float a3)
{
	return m.set_vel(a1,a2,a3);
}
extern "C" bool receive(unsigned char a)
{
	return m.receive(a);
}
extern "C" bool armcheck(void)
{
	return m.armcheck();
}
extern "C" void init(
		unsigned short (*serialwrite)(unsigned char *buffer,uint16_t lenth),
		void (*Delay)(uint32_t),
		uint32_t (*Clock)(void),
		void (*Debug_text)(char *,...)
		)
		{
			m.init(serialwrite,Delay,Clock,Debug_text);
		}
extern "C" bool requestDataStream(uint8_t interval1,uint8_t interval2,uint8_t interval3)
{
	return m.requestDataStream(interval1,interval2,interval3);
}
extern "C" float* get_height(void)
{
	return m.get_height();
}
extern "C" float* get_velocity_x(void)
{
	return m.get_velocity_x();
}
extern "C" float* get_velocity_y(void)
{
	return m.get_velocity_y();
}
extern "C" 	bool mav_takeoff_stabilize(void)
{
	return m.mav_takeoff_stabilize();
}
extern "C" 	bool set_attitude(float a1,float a2,float a3,float a4)
{
	return m.set_attitude(a1,a2,a3,a4);
}
extern "C" 	bool set_new_vel(float a1,float a2,float a3)
{
	return m.set_vel_new(a1,a2,a3);
}
// 0/1  p i d imax
extern "C" 	bool set_pid(float a1,float a2,float a3,float a4,float a5)
{
	return m.set_pid(a1,a2,a3,a4,a5);
}
extern "C" 	bool set_yaw(float a1)
{
	return m.S_set_yaw(a1);
}
extern "C" int* getFlagDataUpdated()//0-opt 1-attitude
{
	return m.getFlagDataUpdated();
}
extern "C" float** getAttitude(void)
{
	return m.getAttitude();
}
extern "C" float fuck()
{
	return m.fuck();
}
