#include "stm32f10x.h"
#include "pwm.h"

void motor_initial(void)
{
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOD,ENABLE);
	/*
	使能PD2 根据驱动板原理图可知 PD2是ENA使能口，必须去使能一下
	可能你要问为何这里没有使能PB3的时钟，因为程序只执行main
	main里pwm_initial()里面已经使能过PB的时钟了。
	并且也开启映射，关闭调试口了。这里就不再重复写
	*/
	GPIO_InitTypeDef GPIO_InitStructure;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3;
	/*
	PB3 只有重映射之后才能当普通IO口用。
	PB3,PB4,PA13,PA14,PA15这五个特殊IO口
	只需要GPIO_PinRemapConfig(GPIO_Remap_SWJ_Disable/NoJTRST/JTAGDisable,ENABLE);
	就可以开启重映射中他们的IO口功能
	若要开启定时器等其他映射功能，则应该要加GPIO_PinRemapConfig(GPIO_PartialRemap/FullRemap_TIMx,ENABLE);
	具体问题具体分析，相应的怎么使用应该去查数据手册，Pinout and Pin Description，参考手册 复用功能IO和调试配置里的定时器部分
	和JTAG/SWG部分。
	那么，复用功能又要如何开启呢？
	这个具体可以看我的3、按键控制PWM（多次） 其中的PWM波输出，那个就用到了复用功能
	开启复用只需将TIM2定时器时钟使能，然后输出模式定义为复用推挽/开漏就行。
	*/ 
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOB,&GPIO_InitStructure);
	
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOD,&GPIO_InitStructure);
}
