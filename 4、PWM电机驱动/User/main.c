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
		�������ת
		*/
		/*
		�������ͨ��ռ�ձȹ̶�������������ʵ�֡����SetCompare��������ľ���CCR Ҳ����ռ�ձ�
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
		������δ��룬���������ں����Ƶģ�����Ҫһֱ����ռ�ձ����������ȣ����Բ������ڵ��
		���ڵ���ϵĻ�����¼�����ᷢ�ֵ����һ��һ�ٵĳ鴤����������Ϊռ�ձ�һֱ�ڱ䡣
		*/
		}
}
