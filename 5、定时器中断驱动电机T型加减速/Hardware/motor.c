#include "stdio.h"
#include "motor.h"
#include "math.h"

speedRampData srd;
int ARR_Val = 72;

static void TimeNVIC_Init(void)
{
	NVIC_InitTypeDef NVIC_InitStructure; 
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_0);// �����ж���Ϊ0	
	NVIC_InitStructure.NVIC_IRQChannel = TIM3_IRQn;// �����ж���Դ 	
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;// ������ռ���ȼ�	 
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;// ������Ӧ���ȼ�
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
��ʼ��THB7128������Ƶ������������õ�IO��
STEP1��PB4��PWM����ڣ���DIR1��PB3����ת������ƣ���ENA��PD2��ʹ�ܿڣ�
*/

static void TimeMode_Init(void)
{
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3,ENABLE);
	
	GPIO_PinRemapConfig(GPIO_PartialRemap_TIM3,ENABLE);//���ڱ�������������ԣ�PB4Ҫ���PWM���뿪����ӳ��
	GPIO_PinRemapConfig(GPIO_Remap_SWJ_Disable,ENABLE);
	/*
	�رյ��Կڣ��ɲ������ֲᣬPinout and Pin Description,�ɵ�֪������Ҫ����(main function)��
	Ĭ�ϸ��ù��ܣ�default alternate function����ӳ�书�ܣ�remap alternate function��
	*/
	TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;
	TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1;
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
	TIM_TimeBaseStructure.TIM_Period = 0xffff;//ARR
	TIM_TimeBaseStructure.TIM_Prescaler = 71;//PSC =  TIM_Prescaler+1
	TIM_TimeBaseInit(TIM3,&TIM_TimeBaseStructure);
	
	TIM_OCInitTypeDef TIM_OCInitStructure;
	
	TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_Toggle;//��תģʽ
	TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High;
	TIM_OCInitStructure.TIM_OutputState = ENABLE;
	TIM_OCInitStructure.TIM_OCIdleState = TIM_OCIdleState_Reset;//���ͨ������״̬��ƽ��������Ϊ0
	TIM_OCInitStructure.TIM_Pulse = 0;//CCR 
	TIM_OC1Init(TIM3,&TIM_OCInitStructure);
//TIM_OC1PreloadConfig(TIM3,TIM_OCPreload_Disable);//��ֹ��װ�أ���ʡ�ԣ���ΪĬ�Ͻ�ֹ��װ��
	TIM_ITConfig(TIM3, TIM_IT_CC1, ENABLE);//ʹ�ܱȽ��ж�
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
	unsigned int crossover_step;//�Ӽ��ٽ��㲽��
	if(dirtemp<0)
	{
		srd.dir = CCW;
	}
	else
	{
		srd.dir = CW;
	}
	DIR(srd.dir);//���������жϷ�������ú궨�嶨��õ�DIR��a������������
	if(step != 0)
	{
		srd.min_delay = (int)(A /speed);
		srd.step_delay = (long)((( B * sqrt(C/accel))*10));//��C0
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
		srd.start_decel = step + srd.decel_val;//start_decel��ʼ���ٵĲ�������ֵ�� decel_val���ٵľ��루��ֵ�����ٶȷ����෴��
    
    if(srd.step_delay <= srd.min_delay)
    {
        srd.step_delay = srd.min_delay;
        srd.run_state = RUN;
		}
		/* �������ٶȺ��������ǾͲ���Ҫ���м����˶�����С��ʱ�����ڵ�һ���������ʱ����ζ������ٶȶ�û��һ��ʼ���ٶȿ죬������ٶȺ�����ֱ�������˶��ͺá�*/
    else
			{
        srd.run_state = ACCEL;
			}

    // ��λ���ٶȼ���ֵ
    srd.accel_count = 0;
		int TIM_CNTCount=TIM3->CNT;
    //����ռ�ձ�Ϊ50%	
    TIM_SetCompare1(TIM3,TIM_CNTCount+srd.step_delay);
		TIM_ITConfig(TIM3, TIM_IT_CC1 , ENABLE);
    //ʹ�ܶ�ʱ��	      
    TIM_Cmd(TIM3, ENABLE); 
    }
		
	}

void TIM3_IRQHandler(void)
{
	
	uint32_t TIM_CNTCount = 0;
	uint32_t Temp = 0;
	// ������һ����ʱ����
  unsigned long int new_step_delay;
  // ���ٹ��������һ����ʱ.
  static int last_accel_delay;
  // �ƶ�����������
  static unsigned int step_count = 0;
  // ��¼new_step_delay�е������������һ������ľ���
  static signed int rest = 0;
	static unsigned char i=0;
	
if (TIM_GetITStatus(TIM3, TIM_IT_CC1) != RESET)
{
	TIM_ClearITPendingBit(TIM3, TIM_IT_CC1);
	TIM_CNTCount = TIM3->CNT;
	Temp = TIM_CNTCount + srd.step_delay;
	TIM_SetCompare1(TIM3,Temp);
	i++;     // ��ʱ���жϴ�������ֵ
	if(i==2) // �����ж�2�Σ�˵���Ѿ����һ���������壬����ת������
		{
			i=0;   // ���㶨ʱ���жϴ�������ֵ
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
                       + rest)/(4 * srd.accel_count + 1));  //ע�⣡��ʱaccel_countΪ��ֵ���൱��Cn+1 = 0.424*��Cn-��2*Cn+1/4*n+1����
				rest = ((2 * (long)srd.step_delay)+rest)%(4 * srd.accel_count + 1);
      if(step_count >= srd.start_decel) {     //����Ƿ�Ӧ�ÿ�ʼ����
				srd.accel_count = srd.decel_val;      //��ʼ���٣������ټ������е�ֵ��Ϊ���پ����ֵ�����Ѽ��ټ�������ɼ��ټ�����
				srd.run_state = DECEL;
      }
      
      else if(new_step_delay <= srd.min_delay) {    //��δ��Ҫ���������Ƿ񵽴�����������ٶ�
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
      if(step_count >= srd.start_decel) {     //����Ƿ���Ҫ��ʼ����
				srd.accel_count = srd.decel_val;      //��ʼ���٣������ټ������е�ֵ��Ϊ���پ����ֵ�����Ѽ��ټ�������ɼ��ټ�����
        new_step_delay = last_accel_delay;
        srd.run_state = DECEL;
      }
      break;

    case DECEL:
      step_count++;
      srd.accel_count++;
      new_step_delay = srd.step_delay - (((2 * (long)srd.step_delay) 
                       + rest)/(4 * srd.accel_count + 1)); 
			/*ע�⣡��ʱaccel_countΪ��ֵ���൱��Cn+1 = 0.424*��Cn+��2*Cn+1/4*n+1����������ٹ����෴ʵ�ּ��٣����幫ʽ��PROGRAMMING TIPS �������з���*/
      rest = ((2 * (long)srd.step_delay)+rest)%(4 * srd.accel_count + 1);
      if(srd.accel_count >= 0){      //����Ƿ�Ϊ���һ��
        srd.run_state = STOP;
      }
      break;
  }
  srd.step_delay = new_step_delay;
	}
}
}
