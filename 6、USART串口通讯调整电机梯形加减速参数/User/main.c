#include "stm32f10x.h"                  // Device header
#include "delay.h"
#include "motor.h"
#include "usart.h"
#include "stdio.h"

int main(void)
{
	Motor_Init();
	USART1_Init(115200);
	while(1)
		{
			if(serial_status == 1)
				{	
					GPIO_SetBits(GPIOD,GPIO_Pin_2);
					Motor_Move(data.dirtemp1,data.step1,data.accel1,data.decel1,data.speed1);
					serial_status = 0;
				}		
		}
}
