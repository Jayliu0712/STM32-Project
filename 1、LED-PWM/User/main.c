#include <stm32f10x.h>
#include <delay.h>
#include <pwm.h>

uint8_t i;

int main(void)
{
	pwm_initial();
	
	while (1)
	{
		for(i = 0;i <= 100; i++)
		{
			PWM_SetCompare1(i);
			Delay_ms(10);
		}
		for(i = 0;i <= 100; i++)
		{
			PWM_SetCompare1(100-i);
			Delay_ms(10);
		}
	}
}
