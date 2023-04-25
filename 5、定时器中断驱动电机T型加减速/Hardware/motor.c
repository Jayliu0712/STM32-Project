#include "stdio.h"
#include "motor.h"
#include "math.h"

speedRampData srd;
int ARR_Val = 72;

static void TimeNVIC_Init(void)
{
	NVIC_InitTypeDef NVIC_InitStructure; 
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_0);// 设置中断组为0	
	NVIC_InitStructure.NVIC_IRQChannel = TIM3_IRQn;// 设置中断来源 	
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;// 设置抢占优先级	 
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;// 设置响应优先级
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);
}

static void MotorGPIO_Init(void)
{
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB,ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOD,ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO,ENABLE);
	
	GPIO_InitTypeDef GPIO_InitStructure;
	
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_4;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOB,&GPIO_InitStructure);
	
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOB,&GPIO_InitStructure);
	GPIO_ResetBits(GPIOB,GPIO_Pin_3);
	
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOD,&GPIO_InitStructure);
	GPIO_ResetBits(GPIOD,GPIO_Pin_2);
}
/*
初始化THB7128电机控制的三个引脚所用的IO口
STEP1（PB4，PWM输出口），DIR1（PB3，运转方向控制），ENA（PD2，使能口）
*/

static void TimeMode_Init(void)
{
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3,ENABLE);
	
	GPIO_PinRemapConfig(GPIO_PartialRemap_TIM3,ENABLE);//由于本开发板的特殊性，PB4要输出PWM必须开启重映射
	GPIO_PinRemapConfig(GPIO_Remap_SWJ_Disable,ENABLE);
	/*
	关闭调试口，可查数据手册，Pinout and Pin Description,可得知引脚主要功能(main function)，
	默认复用功能（default alternate function），映射功能（remap alternate function）
	*/
	TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;
	TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1;
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
	TIM_TimeBaseStructure.TIM_Period = 0xffff;//ARR
	TIM_TimeBaseStructure.TIM_Prescaler = 71;//PSC =  TIM_Prescaler+1
	TIM_TimeBaseInit(TIM3,&TIM_TimeBaseStructure);
	
	TIM_OCInitTypeDef TIM_OCInitStructure;
	
	TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_Toggle;//翻转模式
	TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High;
	TIM_OCInitStructure.TIM_OutputState = ENABLE;
	TIM_OCInitStructure.TIM_OCIdleState = TIM_OCIdleState_Reset;//输出通道空闲状态电平极性设置为0
	TIM_OCInitStructure.TIM_Pulse = 0;//CCR 
	TIM_OC1Init(TIM3,&TIM_OCInitStructure);
//TIM_OC1PreloadConfig(TIM3,TIM_OCPreload_Disable);//禁止重装载，可省略，因为默认禁止重装载
	TIM_ITConfig(TIM3, TIM_IT_CC1, ENABLE);//使能比较中断
	TIM_Cmd(TIM3,ENABLE);
}

void Motor_Init(void)
{
	MotorGPIO_Init();
	TimeNVIC_Init();
	TimeMode_Init();
}
	

void Motor_Move(signed int dirtemp, signed int step, unsigned int accel, unsigned int decel, unsigned int speed)
{
	unsigned int maxs_step;
	unsigned int crossover_step;//加减速交点步数
	if(dirtemp<0)
	{
		srd.dir = CCW;
	}
	else
	{
		srd.dir = CW;
	}
	DIR(srd.dir);//根据正负判断方向，最后用宏定义定义好的DIR（a）输出电机方向
	if(step != 0)
	{
		srd.min_delay = (int)(A /speed);
		srd.step_delay = (long)((( B * sqrt(C/accel))*10));//算C0
		maxs_step = (unsigned int)(speed*speed/(D*accel)); 
		
		crossover_step = (unsigned int)(decel*step/(accel+decel));
		
		if(crossover_step <= maxs_step)
    {
			srd.decel_val = crossover_step - step;
    }
    else
    {
			srd.decel_val = -(maxs_step*accel/decel);
    }
		srd.start_decel = step + srd.decel_val;//start_decel开始减速的步数（正值） decel_val减速的距离（负值，加速度方向相反）
    
    if(srd.step_delay <= srd.min_delay)
    {
        srd.step_delay = srd.min_delay;
        srd.run_state = RUN;
		}
		/* 如果最大速度很慢，我们就不需要进行加速运动，最小延时都大于第一个脉冲的延时，意味着最大速度都没有一开始的速度快，即最大速度很慢，直接匀速运动就好。*/
    else
			{
        srd.run_state = ACCEL;
			}

    // 复位加速度计数值
    srd.accel_count = 0;
		int TIM_CNTCount=TIM3->CNT;
    //设置占空比为50%	
    TIM_SetCompare1(TIM3,TIM_CNTCount+srd.step_delay);
		TIM_ITConfig(TIM3, TIM_IT_CC1 , ENABLE);
    //使能定时器	      
    TIM_Cmd(TIM3, ENABLE); 
    }
		
	}

void TIM3_IRQHandler(void)
{
	
	uint32_t TIM_CNTCount = 0;
	uint32_t Temp = 0;
	// 保存下一个延时周期
  unsigned long int new_step_delay;
  // 加速过程中最后一次延时.
  static int last_accel_delay;
  // 移动步数计数器
  static unsigned int step_count = 0;
  // 记录new_step_delay中的余数，提高下一步计算的精度
  static signed int rest = 0;
	static unsigned char i=0;
	
if (TIM_GetITStatus(TIM3, TIM_IT_CC1) != RESET)
{
	TIM_ClearITPendingBit(TIM3, TIM_IT_CC1);
	TIM_CNTCount = TIM3->CNT;
	Temp = TIM_CNTCount + srd.step_delay;
	TIM_SetCompare1(TIM3,Temp);
	i++;     // 定时器中断次数计数值
	if(i==2) // 进入中断2次，说明已经输出一个完整脉冲，即翻转了两次
		{
			i=0;   // 清零定时器中断次数计数值
			switch(srd.run_state) {
				case STOP:
				step_count = 0;
				rest = 0;
				GPIO_ResetBits(GPIOD,GPIO_Pin_2);
				TIM_Cmd(TIM3, DISABLE);
				break;

				case ACCEL:
				step_count++;
				srd.accel_count++;
				new_step_delay = srd.step_delay - (((2 * (long)srd.step_delay) 
                       + rest)/(4 * srd.accel_count + 1));  //注意！此时accel_count为正值，相当于Cn+1 = 0.424*（Cn-（2*Cn+1/4*n+1））
				rest = ((2 * (long)srd.step_delay)+rest)%(4 * srd.accel_count + 1);
      if(step_count >= srd.start_decel) {     //检查是否应该开始减速
				srd.accel_count = srd.decel_val;      //开始减速，将加速计数器中的值变为减速距离的值，即把加速计数器变成减速计数器
				srd.run_state = DECEL;
      }
      
      else if(new_step_delay <= srd.min_delay) {    //还未需要减速则检查是否到达期望的最大速度
        last_accel_delay = new_step_delay;
        new_step_delay = srd.min_delay;
        rest = 0;
        srd.run_state = RUN;
      }
      break;

    case RUN:
		step_count++;
		srd.accel_count++;
		new_step_delay = srd.min_delay;
      if(step_count >= srd.start_decel) {     //检查是否需要开始减速
				srd.accel_count = srd.decel_val;      //开始减速，将加速计数器中的值变为减速距离的值，即把加速计数器变成减速计数器
        new_step_delay = last_accel_delay;
        srd.run_state = DECEL;
      }
      break;

    case DECEL:
      step_count++;
      srd.accel_count++;
      new_step_delay = srd.step_delay - (((2 * (long)srd.step_delay) 
                       + rest)/(4 * srd.accel_count + 1)); 
			/*注意！此时accel_count为负值，相当于Cn+1 = 0.424*（Cn+（2*Cn+1/4*n+1）），与加速过程相反实现减速，具体公式在PROGRAMMING TIPS 本子里有分析*/
      rest = ((2 * (long)srd.step_delay)+rest)%(4 * srd.accel_count + 1);
      if(srd.accel_count >= 0){      //检查是否为最后一步
        srd.run_state = STOP;
      }
      break;
  }
  srd.step_delay = new_step_delay;
	}
}
}
