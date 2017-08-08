/*
 * buzzer.c
 *
 *  Created on: 2017年8月6日
 *      Author: lenovo
 */
#include "buzzer.h"
#include "r_cg_cmt.h"
#include "r_cg_macrodriver.h"


void alarm(void)
{
	unsigned long cycle = 0;
	unsigned long count = 0;
	for(count = 0; count <= 19; count++)
	{
		if(count<=7)
		{
	    	for(cycle = 200; cycle <= 1000; cycle++)
	    	{
	    		PORT0.PODR.BIT.B0 = 1;
	    		delay_us(cycle);
	    		PORT0.PODR.BIT.B0 = 0;
	    		delay_us(cycle);
	    	}
	    	delay_ms(10);
		}
		else
		{
	    	for(cycle = 500; cycle >= 20; cycle--)
	    	{
	    		PORT0.PODR.BIT.B0 = 1;
	    		delay_us(cycle);
	    		PORT0.PODR.BIT.B0 = 0;
	    		delay_us(cycle);
	    	}
	    	delay_ms(4);
		}
	}
}

