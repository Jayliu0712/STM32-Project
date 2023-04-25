#include "stm32f10x.h"
#include "delay.h"
#include "pwm.h"
#include "motor.h"


uint8_t i;

int main(void)
{	
	pwm_initial();
	motor_initial();
	GPIO_SetBits(GPIOB,GPIO_Pin_3);
	GPIO_SetBits(GPIOD,GPIO_Pin_2);
	while (1)
	{	
		  pwm_SetCompare1(10);
//		GPIO_ResetBits(GPIOB,GPIO_Pin_3);
//		pwm_SetCompare1(10);
//		Delay_s(2);
//		GPIO_SetBits(GPIOB,GPIO_Pin_3);
//		pwm_SetCompare1(10);
//		Delay_s(2);
		/*
		电机正反转
		*/
		/*
		电机调速通过占空比固定，调整周期来实现。这个SetCompare（）内填的就是CCR 也就是占空比
		*/
//	for(i = 0;i <= 100; i++)
//	{
//		pwm_SetCompare1(i);
//		Delay_ms(10);
//	}
//	for(i = 0;i <= 100; i++)
//	{
//		pwm_SetCompare1(100-i);
//		Delay_ms(10);
//	} 
		/*
		以上这段代码，由于是用于呼吸灯的，所以要一直调整占空比来调整亮度，所以不适用于电机
		用于电机上的话，烧录代码后会发现电机会一顿一顿的抽搐，可能是因为占空比一直在变。
		*/
		}
}
