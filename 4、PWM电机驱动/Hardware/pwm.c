#include "stm32f10x.h"

void pwm_initial(void)
{
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB|RCC_APB2Periph_AFIO,ENABLE);
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3,ENABLE);
	
	GPIO_PinRemapConfig(GPIO_PartialRemap_TIM3,ENABLE);
	/*
	使能PB4的映射功能 TIM3_CH1
	*/	
	GPIO_InitTypeDef GPIO_InitStructure;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_4;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOB,&GPIO_InitStructure);

	GPIO_PinRemapConfig(GPIO_Remap_SWJ_Disable,ENABLE);
	/*
	关闭调试口，可查数据手册，Pinout and Pin Description,可得知引脚主要功能(main function)，
	默认复用功能（default alternate function），映射功能（remap alternate function）
	*/
	TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStructure;
	TIM_TimeBaseInitStructure.TIM_ClockDivision = TIM_CKD_DIV1;
	TIM_TimeBaseInitStructure.TIM_CounterMode = TIM_CounterMode_Up;
	TIM_TimeBaseInitStructure.TIM_Period = 100-1;
	TIM_TimeBaseInitStructure.TIM_Prescaler = 40-1;
	TIM_TimeBaseInitStructure.TIM_RepetitionCounter = 0;
	TIM_TimeBaseInit(TIM3,&TIM_TimeBaseInitStructure);
	
	TIM_OCInitTypeDef TIM_OCInitStructure;
	TIM_OCStructInit(&TIM_OCInitStructure);
	TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1;
	TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High;
	TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;
	TIM_OCInitStructure.TIM_Pulse = 0;
	
	TIM_OC1Init(TIM3,&TIM_OCInitStructure);//TIM3_CH1 所以OC1,初始化通道一
//	TIM_OC1PreloadConfig(TIM3, TIM_OCPreload_Enable);//这句写不写都没差别，所以是干啥用的？
	
	TIM_Cmd(TIM3,ENABLE);
}

void pwm_SetCompare1(uint16_t compare)
{
	TIM_SetCompare1(TIM3,compare);//TIM3_CH1 所以TIM_SetCompare1 数字1代表的是TMx的通道一
}
