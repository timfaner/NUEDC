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
extern "C" int* getFlagDataUpdated()
{
	return m.getFlagDataUpdated();
}
extern "C" float* getHeight(void)
{
	return m.getHeight();
}
extern "C" float** getLocalPosition(void)
{
	return m.getLocalPosition();
}
extern "C" float** getAttitude(void)
{
	return m.getAttitude();
}
extern "C" float fuck()
{
	return m.fuck();
}

