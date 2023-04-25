#include "stm32f10x.h"
#include "pwm.h"

void motor_initial(void)
{
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOD,ENABLE);
	/*
	ʹ��PD2 ����������ԭ��ͼ��֪ PD2��ENAʹ�ܿڣ�����ȥʹ��һ��
	������Ҫ��Ϊ������û��ʹ��PB3��ʱ�ӣ���Ϊ����ִֻ��main
	main��pwm_initial()�����Ѿ�ʹ�ܹ�PB��ʱ���ˡ�
	����Ҳ����ӳ�䣬�رյ��Կ��ˡ�����Ͳ����ظ�д
	*/
	GPIO_InitTypeDef GPIO_InitStructure;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3;
	/*
	PB3 ֻ����ӳ��֮����ܵ���ͨIO���á�
	PB3,PB4,PA13,PA14,PA15���������IO��
	ֻ��ҪGPIO_PinRemapConfig(GPIO_Remap_SWJ_Disable/NoJTRST/JTAGDisable,ENABLE);
	�Ϳ��Կ�����ӳ�������ǵ�IO�ڹ���
	��Ҫ������ʱ��������ӳ�书�ܣ���Ӧ��Ҫ��GPIO_PinRemapConfig(GPIO_PartialRemap/FullRemap_TIMx,ENABLE);
	������������������Ӧ����ôʹ��Ӧ��ȥ�������ֲᣬPinout and Pin Description���ο��ֲ� ���ù���IO�͵���������Ķ�ʱ������
	��JTAG/SWG���֡�
	��ô�����ù�����Ҫ��ο����أ�
	���������Կ��ҵ�3����������PWM����Σ� ���е�PWM��������Ǹ����õ��˸��ù���
	��������ֻ�轫TIM2��ʱ��ʱ��ʹ�ܣ�Ȼ�����ģʽ����Ϊ��������/��©���С�
	*/ 
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOB,&GPIO_InitStructure);
	
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOD,&GPIO_InitStructure);
}
