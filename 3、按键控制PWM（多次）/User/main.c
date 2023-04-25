#include "stm32f10x.h"                  // Device header
#include "delay.h"
#include "pwm.h"
#include "timer.h"

uint8_t i;


int main(void)
{
	pwm_initial();
	timer_initial();
	while (1)
	{
		if(keynum == 0)
		{
			for(i = 0; i <= 100; i++)
			{
				pwm_SetCompare1(i);
				Delay_ms(10);
			}
			for(i = 0; i <= 100; i++)
			{
				pwm_SetCompare1(100-i);
				Delay_ms(10);
			}
		}
		if(keynum == 1) 
		{
			pwm_SetCompare1(0);
		}
	}
}
