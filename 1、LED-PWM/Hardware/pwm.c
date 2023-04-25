#include <stm32f10x.h>

void pwm_initial(void)
{
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2,ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA,ENABLE);
	
	GPIO_InitTypeDef GPIO_InitStructure;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_1;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA,&GPIO_InitStructure);
	
	TIM_InternalClockConfig(TIM2);
	
	TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStructure;
	TIM_TimeBaseInitStructure.TIM_ClockDivision = TIM_CKD_DIV1;//shi zhong fen ge
	TIM_TimeBaseInitStructure.TIM_CounterMode = TIM_CounterMode_Up;
	TIM_TimeBaseInitStructure.TIM_Period = 100-1;//ARR
	TIM_TimeBaseInitStructure.TIM_Prescaler=640-1;//PSC
	TIM_TimeBaseInitStructure.TIM_RepetitionCounter = 0;
	TIM_TimeBaseInit(TIM2,&TIM_TimeBaseInitStructure);//she zhi hao shi yong na ge ding shi qing,bing she zhi hao suo yao ding shi de shi jian 
	
	TIM_OCInitTypeDef TIM_OCInitStructure;//she ding chan sheng PWM de can shu , xuan ze PWM1 hai shi PWM2 mo shi ,hai you CCR he CC1P de she zhi
	TIM_OCStructInit(&TIM_OCInitStructure);
	TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1;
	TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High;
	TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;
	TIM_OCInitStructure.TIM_Pulse = 0;
	/*
	yi shang 6 hang dai ma yong dao le jie gou ti zhi zhen deng zhi shi
	xu yao qu kan kan wang ke hao hao xue xi yi xia xiang guan zhi shi 
	ba dai ma di ceng li jie tou che
	*/
	/*
	she zhi CCR zhi wei 0 , ye jiu yi wei zhe yi kai shi PWM bo shu chu kou
	shu chu di dian ping xiao deng bu liang
	ran hou sui zhe xia mian i zhi de bu duan zeng da bing fu gei CCR1
	xiao deng liang du zhu jian bian da
	*/
	TIM_OC2Init(TIM2,&TIM_OCInitStructure);
	/*
	gen ju TIM_OCInitStructure zhong she ding de zhi chu shi hua !! TIM2 de tong dao 2 !! 
	gen ju shu ju shou ce ke zhi tong dao 2 jiu shi PA1
	*/
	TIM_Cmd(TIM2,ENABLE);// shi neng TIM2 wai she 
}

void PWM_SetCompare1(uint16_t Compare)
{
	TIM_SetCompare2(TIM2,Compare);
	/*
	TIM_SetCompare han shu nei you zhi zhen
	ke yi ba Compare de zhi qu chu fu gei
	CCR2(TIM2 tong dao 2 de CCR zhi , ru guo gen li cheng yi yang she zhi CCR1 de hua hui ba zhi fu gei TIM2 tong dao 1 de CCR zhi,
  dao zhi xiao deng bu hui you fan ying	)
*/
}
