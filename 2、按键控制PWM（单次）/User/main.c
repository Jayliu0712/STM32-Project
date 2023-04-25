#include "stm32f10x.h"                  // Device header
#include "delay.h"
#include "pwm.h"
#include "key.h"

uint8_t i;


int main(void)
{
	pwm_initial();
	key_initial();
	while (1)
	{
		uint8_t Keynum = 0;
    Keynum = key_num();
		if(Keynum == 1)
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
		if(Keynum == 0) 
		{
			pwm_SetCompare1(0);
		}
	}
}
